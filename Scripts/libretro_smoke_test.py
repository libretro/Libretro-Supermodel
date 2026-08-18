#!/usr/bin/env python3
"""Run isolated RetroArch boot smoke tests for Games.xml ROM sets."""

from __future__ import annotations

import argparse
import csv
import json
import platform
import shutil
import signal
import subprocess
import sys
import tempfile
import time
import xml.etree.ElementTree as ET
from dataclasses import asdict, dataclass
from datetime import datetime
from pathlib import Path


DEFAULT_FPS = 60.0
CORE_SUFFIX = {"Windows": ".dll", "Darwin": ".dylib"}
CRASH_MARKERS = (
    "segmentation fault",
    "bus error",
    "fatal error",
    "uncaught exception",
    "assertion failed",
)
LOAD_FAILURE_MARKER = "[content] failed to load content"
FAILED_STATUSES = {"crash", "load_error", "timeout", "no_screenshot"}


@dataclass(frozen=True)
class Game:
    name: str
    title: str
    version: str


@dataclass
class Result:
    name: str
    title: str
    version: str
    status: str
    exit_code: int | None
    signal: str
    frames: int
    emulated_seconds: float
    real_seconds: float
    fast_forward: bool
    screenshot: str
    save: str
    log: str
    console_log: str
    note: str


def default_core(repo_root: Path) -> Path:
    suffix = CORE_SUFFIX.get(platform.system(), ".so")
    return repo_root / f"supermodel_libretro{suffix}"


def parse_args() -> argparse.Namespace:
    repo_root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(
        description=(
            "Launch each Games.xml set in an isolated RetroArch environment, "
            "stop after a deterministic number of emulated frames, and save the "
            "final frame, logs, generated SRAM, CSV, and JSON summary."
        )
    )
    parser.add_argument("--retroarch", type=Path,
                        help="RetroArch executable (default: first retroarch in PATH)")
    parser.add_argument("--core", type=Path, default=default_core(repo_root),
                        help="Supermodel Libretro core (default: repository build)")
    parser.add_argument("--rom-dir", type=Path, required=True,
                        help="Directory containing <set>.zip ROMs")
    parser.add_argument("--games-xml", type=Path,
                        default=repo_root / "Config" / "Games.xml",
                        help="Games.xml used to enumerate sets")
    parser.add_argument("--system-assets", type=Path,
                        help=("Directory whose Games.xml, Music.xml and Supermodel.ini are "
                              "copied into the isolated system directory; defaults to "
                              "Games.xml's directory"))
    parser.add_argument("--base-config", type=Path,
                        help="Optional RetroArch config copied before isolated path overrides")
    parser.add_argument("--output", type=Path,
                        help=("Persistent results directory (default: "
                              "~/Documents/RetroArch/supermodel-smoke/<timestamp>)"))
    parser.add_argument("--seconds", type=float, default=60.0,
                        help="Emulated seconds before capture (default: 60)")
    parser.add_argument("--fps", type=float, default=DEFAULT_FPS,
                        help="Frames per emulated second (default: 60)")
    parser.add_argument("--wall-timeout", type=float, default=180.0,
                        help="Real-time process timeout per game in seconds (default: 180)")
    parser.add_argument("--fast-forward", action=argparse.BooleanOptionalAction, default=True,
                        help="Run without video and audio sync (default: enabled)")
    parser.add_argument("--game", action="append", default=[], metavar="SET",
                        help="Test only this set; repeat for multiple sets")
    parser.add_argument("--from-game", metavar="SET",
                        help="Start at this set in Games.xml order (for resuming)")
    parser.add_argument("--list", action="store_true",
                        help="List selected games and ROM availability without launching RetroArch")
    parser.add_argument("--keep-workdirs", action="store_true",
                        help="Preserve per-game temporary RetroArch environments for diagnosis")
    return parser.parse_args()


def load_games(path: Path) -> list[Game]:
    root = ET.parse(path).getroot()
    games: list[Game] = []
    for node in root.findall("game"):
        name = node.get("name", "").strip()
        if not name:
            continue
        title = (node.findtext("identity/title") or name).strip()
        version = (node.findtext("identity/version") or "").strip()
        games.append(Game(name, title, version))
    return games


def select_games(games: list[Game], requested: list[str], start: str | None) -> list[Game]:
    known = {game.name: game for game in games}
    unknown = sorted(set(requested) - known.keys())
    if unknown:
        raise ValueError("unknown Games.xml set(s): " + ", ".join(unknown))
    selected = [known[name] for name in requested] if requested else games
    if start:
        positions = {game.name: index for index, game in enumerate(selected)}
        if start not in positions:
            raise ValueError(f"--from-game set is not selected: {start}")
        selected = selected[positions[start]:]
    return selected


def quote_config(value: Path | str) -> str:
    return str(value).replace("\\", "\\\\").replace('"', '\\"')


def prepare_environment(work: Path, assets: Path, base_config: Path | None,
                        fast_forward: bool) -> list[str]:
    for directory in ("system/supermodel", "saves", "states", "logs", "config", "remaps"):
        (work / directory).mkdir(parents=True, exist_ok=True)
    for asset in assets.iterdir():
        if asset.is_file() and asset.suffix.lower() in {".xml", ".ini", ".bmp"}:
            shutil.copy2(asset, work / "system" / "supermodel" / asset.name)

    overrides_path = work / "smoke-test-overrides.cfg"
    overrides = {
        "config_save_on_exit": "false",
        "system_directory": work / "system",
        "savefile_directory": work / "saves",
        "savestate_directory": work / "states",
        "log_dir": work / "logs",
        "log_to_file": "true",
        "log_to_file_timestamp": "false",
        "log_verbosity": "true",
        "core_options_path": work / "config" / "core-options.cfg",
        "input_remapping_directory": work / "remaps",
        "content_history_path": work / "config" / "content_history.lpl",
        "content_favorites_path": work / "config" / "content_favorites.lpl",
        "video_vsync": "false" if fast_forward else "true",
        "audio_sync": "false" if fast_forward else "true",
        "video_gpu_screenshot": "true",
        "fps_show": "false",
        "framecount_show": "false",
        "video_fullscreen": "false",
        "video_windowed_fullscreen": "false",
        "video_window_custom_size_enable": "true",
        "video_windowed_position_width": "496",
        "video_windowed_position_height": "384",
        "video_scale": "1.000000",
        "video_window_save_positions": "false",
        "pause_nonactive": "false",
        "savestate_auto_load": "false",
        "savestate_auto_save": "false",
    }
    lines = ["# Isolated Supermodel smoke-test overrides"]
    lines.extend(f'{key} = "{quote_config(value)}"' for key, value in overrides.items())
    overrides_path.write_text("\n".join(lines) + "\n", encoding="utf-8")

    if not base_config:
        return ["-c", str(overrides_path)]
    base_copy = work / "base-retroarch.cfg"
    shutil.copy2(base_config, base_copy)
    # RetroArch gives --appendconfig priority over its base configuration.
    # Keeping the files separate avoids duplicate-key first-value semantics.
    return ["-c", str(base_copy), f"--appendconfig={overrides_path}"]


def terminate_process(process: subprocess.Popen[bytes]) -> None:
    if process.poll() is not None:
        return
    process.terminate()
    try:
        process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        process.kill()
        process.wait(timeout=5)


def copy_if_present(source: Path, destination: Path) -> str:
    if not source.is_file():
        return ""
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, destination)
    return str(destination)


def run_game(args: argparse.Namespace, game: Game, output: Path, frames: int) -> Result:
    rom = args.rom_dir / f"{game.name}.zip"
    frame_out = output / "frames" / f"{game.name}.png"
    log_out = output / "logs" / f"{game.name}.log"
    console_out = output / "logs" / f"{game.name}.console.log"
    save_out = output / "saves" / f"{game.name}.srm"
    if not rom.is_file():
        return Result(
            game.name, game.title, game.version, "missing_rom", None, "",
            frames, args.seconds, 0.0, args.fast_forward,
            "", "", "", "", f"ROM not found: {rom}",
        )

    temporary = tempfile.TemporaryDirectory(prefix=f"supermodel-smoke-{game.name}-")
    work = Path(temporary.name)
    config_args = prepare_environment(work, args.system_assets, args.base_config,
                                      args.fast_forward)
    frame_tmp = work / f"{game.name}.png"
    core_log_tmp = work / "logs" / f"{game.name}.log"
    command = [
        str(args.retroarch), "-v", *config_args, "-L", str(args.core),
        f"--log-file={core_log_tmp}", f"--max-frames={frames}",
        "--max-frames-ss", f"--max-frames-ss-path={frame_tmp}", str(rom),
    ]
    started = time.monotonic()
    timed_out = False
    process: subprocess.Popen[bytes] | None = None
    with console_out.open("wb") as console:
        try:
            process = subprocess.Popen(command, stdout=console, stderr=subprocess.STDOUT,
                                       cwd=work)
            exit_code = process.wait(timeout=args.wall_timeout)
        except subprocess.TimeoutExpired:
            timed_out = True
            terminate_process(process)
            exit_code = process.returncode
        except KeyboardInterrupt:
            if process:
                terminate_process(process)
            raise
    elapsed = time.monotonic() - started

    screenshot = copy_if_present(frame_tmp, frame_out)
    log = copy_if_present(core_log_tmp, log_out)
    save_candidates = sorted((work / "saves").glob("**/*.srm"))
    save = copy_if_present(save_candidates[0], save_out) if save_candidates else ""
    signal_name = ""
    if exit_code is not None and exit_code < 0:
        try:
            signal_name = signal.Signals(-exit_code).name
        except ValueError:
            signal_name = str(-exit_code)

    diagnostic_lines: list[str] = []
    for candidate in (core_log_tmp, console_out):
        if candidate.is_file():
            diagnostic_lines += candidate.read_text(encoding="utf-8",
                                                    errors="replace").splitlines()
    diagnostic_text = "\n".join(diagnostic_lines).lower()
    marker = next((item for item in CRASH_MARKERS if item in diagnostic_text), "")
    core_error = next((line.strip() for line in diagnostic_lines
                       if "[libretro ERROR]" in line), "")
    if timed_out:
        status, note = "timeout", f"exceeded {args.wall_timeout:g}s real-time limit"
    elif LOAD_FAILURE_MARKER in diagnostic_text and not marker and not signal_name:
        status, note = "load_error", core_error or "RetroArch could not load the content"
    elif exit_code != 0 or signal_name or marker:
        status, note = "crash", marker or signal_name or f"exit code {exit_code}"
    elif not screenshot:
        status, note = "no_screenshot", "RetroArch exited cleanly but saved no final frame"
    else:
        status, note = "ok", "inspect final frame for service/setup loops"

    if args.keep_workdirs:
        kept = output / "work" / game.name
        if kept.exists():
            shutil.rmtree(kept)
        shutil.copytree(work, kept)
    temporary.cleanup()
    return Result(game.name, game.title, game.version, status, exit_code, signal_name,
                  frames, args.seconds, round(elapsed, 3), args.fast_forward,
                  screenshot, save, log, str(console_out), note)


def write_summaries(output: Path, results: list[Result]) -> None:
    rows = [asdict(result) for result in results]
    (output / "summary.json").write_text(json.dumps(rows, indent=2) + "\n", encoding="utf-8")
    with (output / "summary.csv").open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, fieldnames=list(rows[0]) if rows else ["name"])
        writer.writeheader()
        writer.writerows(rows)


def main() -> int:
    args = parse_args()
    retroarch = args.retroarch.expanduser() if args.retroarch else shutil.which("retroarch")
    if not retroarch:
        raise FileNotFoundError("retroarch (install it or pass --retroarch)")
    args.retroarch = Path(retroarch).resolve()
    args.core = args.core.expanduser().resolve()
    args.rom_dir = args.rom_dir.expanduser().resolve()
    args.games_xml = args.games_xml.expanduser().resolve()
    args.system_assets = (args.system_assets or args.games_xml.parent).expanduser().resolve()
    args.base_config = args.base_config.expanduser().resolve() if args.base_config else None
    if args.seconds <= 0 or args.fps <= 0 or args.wall_timeout <= 0:
        raise ValueError("--seconds, --fps, and --wall-timeout must be positive")
    games = select_games(load_games(args.games_xml), args.game, args.from_game)
    if args.list:
        for game in games:
            state = "present" if (args.rom_dir / f"{game.name}.zip").is_file() else "MISSING"
            print(f"{game.name:12} {state:7} {game.title} ({game.version})")
        return 0
    for required in (args.retroarch, args.core, args.games_xml, args.system_assets):
        if not required.exists():
            raise FileNotFoundError(required)
    if args.base_config and not args.base_config.is_file():
        raise FileNotFoundError(args.base_config)

    if args.output:
        output = args.output.expanduser().resolve()
    else:
        stamp = datetime.now().strftime("%Y%m%d-%H%M%S")
        output = Path.home() / "Documents" / "RetroArch" / "supermodel-smoke" / stamp
    output.mkdir(parents=True, exist_ok=False)
    for directory in ("frames", "logs", "saves"):
        (output / directory).mkdir(exist_ok=True)
    frames = max(1, round(args.seconds * args.fps))
    print(f"Results: {output}")
    print(f"Selected: {len(games)} sets; capture after {frames} frames "
          f"({args.seconds:g}s emulated)")

    results: list[Result] = []
    try:
        for index, game in enumerate(games, 1):
            print(f"[{index:02}/{len(games):02}] {game.name}: {game.title}", flush=True)
            result = run_game(args, game, output, frames)
            results.append(result)
            write_summaries(output, results)
            print(f"       {result.status} in {result.real_seconds:.1f}s", flush=True)
    except KeyboardInterrupt:
        print("Interrupted; completed results were preserved.", file=sys.stderr)
        return 130

    counts: dict[str, int] = {}
    for result in results:
        counts[result.status] = counts.get(result.status, 0) + 1
    print("Summary: " + ", ".join(f"{key}={value}" for key, value in sorted(counts.items())))
    print(f"Review final frames in: {output / 'frames'}")
    print(f"Generated isolated SRAM in: {output / 'saves'}")
    return 1 if any(result.status in FAILED_STATUSES for result in results) else 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (FileNotFoundError, RuntimeError, ValueError, ET.ParseError) as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(2)
