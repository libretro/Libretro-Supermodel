#!/usr/bin/env python3
"""Capture Service Menu evidence from the existing parent NVRAM recipes."""

from __future__ import annotations

import argparse
import dataclasses
import json
import os
import sys
import tomllib
from dataclasses import asdict
from pathlib import Path

import libretro_nvram_samples as sampler


MENUS = (
    ("game-assignments", "opengameassignments"),
    ("country-assignments", "opencountryassignments"),
    ("network-assignments", "opennetassignments"),
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Capture parent Service Menu pages without retaining Save RAM."
    )
    parser.add_argument("--game", action="append", default=[], metavar="SET")
    parser.add_argument(
        "--values", action="store_true",
        help="photograph each selected value from the validated NVRAM recipe",
    )
    parser.add_argument(
        "--sample", action="append", default=[], metavar="SUFFIX",
        help="with --values, photograph only this recipe sample; repeat as needed",
    )
    parser.add_argument("--output-root", type=Path, default=Path(
        "~/Documents/RetroArch/model3-game-settings-catalog"
    ))
    parser.add_argument("--core", type=Path, default=Path(
        "~/Library/Application Support/RetroArch/cores/supermodel_libretro.dylib"
    ))
    parser.add_argument("--overwrite", action="store_true")
    parser.add_argument("--dry-run", action="store_true")
    return parser.parse_args()


def recipe_args(path: Path) -> argparse.Namespace:
    return argparse.Namespace(
        config=path, sample=[], resume_from=None, interactive=None,
        output=None, dry_run=False, overwrite=False, keep_workdirs=False,
    )


def load_macros(path: Path) -> dict[str, list[str]]:
    with path.open("rb") as stream:
        data = tomllib.load(stream)
    raw = data.get("macros", {})
    return {
        name.strip().lower(): sampler.split_tokens(value, f"macro {name}", True)
        for name, value in raw.items()
    }


def menu_samples(macros: dict[str, list[str]]) -> list[sampler.Sample]:
    samples = []
    for suffix, macro in MENUS:
        if macro not in macros:
            continue
        tokens = ["@EnterServ", f"@{macro}", "WAIT(0.3)", "CAPTURE(base)", "CLOSE"]
        actions = sampler.validate_close(sampler.expand_actions(tokens, macros))
        samples.append(sampler.Sample(suffix, tuple(actions)))
    return samples


def value_samples(
    path: Path, macros: dict[str, list[str]], requested: set[str],
) -> list[sampler.Sample]:
    with path.open("rb") as stream:
        entries = tomllib.load(stream)["samples"]
    known = {entry["suffix"] for entry in entries}
    unknown = sorted(requested - known)
    if unknown:
        raise ValueError(
            f"unknown sample(s) for {path.name}: " + ", ".join(unknown)
        )
    samples = []
    for entry in entries:
        suffix = entry["suffix"]
        if requested and suffix not in requested:
            continue
        sequence = entry["sequence"]
        if not isinstance(sequence, list) or len(sequence) != 6:
            raise ValueError(f"unexpected validated sequence layout: {path.name}:{suffix}")
        tokens = []
        for part in sequence[:3]:
            tokens.extend(sampler.split_tokens(part, f"sample {suffix} sequence"))
        tokens.extend(("CAPTURE(selected)", "CLOSE"))
        actions = sampler.validate_close(sampler.expand_actions(tokens, macros))
        samples.append(sampler.Sample(suffix, tuple(actions)))
    return samples


def main() -> int:
    if sys.platform != "darwin":
        raise RuntimeError("this Service Menu capture utility currently supports macOS only")
    args = parse_args()
    if args.sample and not args.values:
        raise ValueError("--sample requires --values")
    scripts = Path(__file__).resolve().parent
    recipes = sorted(scripts.glob("libretro_nvram_samples.*.toml"))
    recipes = [path for path in recipes if not path.name.endswith(".example.toml")]
    requested = set(args.game)
    known = {path.name.removeprefix("libretro_nvram_samples.").removesuffix(".toml")
             for path in recipes}
    unknown = sorted(requested - known)
    if unknown:
        raise ValueError("unknown parent recipe(s): " + ", ".join(unknown))

    output_root = Path(os.path.expandvars(os.path.expanduser(str(args.output_root)))).resolve()
    core = Path(os.path.expandvars(os.path.expanduser(str(args.core)))).resolve()
    helper = output_root / ".tools" / "ScreenCaptureWindow"
    selected = []
    for recipe in recipes:
        settings, _ = sampler.load_campaign(recipe_args(recipe))
        if requested and settings.game not in requested:
            continue
        macros = load_macros(recipe)
        samples = (
            value_samples(recipe, macros, set(args.sample))
            if args.values else menu_samples(macros)
        )
        selected.append((recipe, settings, samples))

    if args.dry_run:
        for recipe, settings, samples in selected:
            labels = ", ".join(sample.suffix for sample in samples)
            print(f"{settings.game}: {labels} ({recipe.name})")
        return 0

    if not core.is_file():
        raise FileNotFoundError(core)
    sampler.build_capture_helper(scripts / "ScreenCaptureWindow.m", helper)
    failures = 0
    for index, (recipe, original, samples) in enumerate(selected, 1):
        output = output_root / original.game
        for directory in ("logs", "screenshots"):
            (output / directory).mkdir(parents=True, exist_ok=True)
        settings = dataclasses.replace(
            original, core=core, output_dir=output, collect_saves=False,
            capture_helper=helper,
        )
        sampler.validate_paths(settings)
        print(f"[{index:02}/{len(selected):02}] {settings.game}: {len(samples)} menu(s)", flush=True)
        results = []
        for sample in samples:
            result = sampler.run_sample(settings, sample, args.overwrite, False)
            results.append(result)
            print(f"       {sample.suffix}: {result.status}", flush=True)
            failures += result.status not in {"ok", "skipped"}
        summary_name = "values-summary.json" if args.values else "baseline-summary.json"
        summary_path = output / summary_name
        serialized = [asdict(result) for result in results]
        if args.sample and summary_path.is_file():
            previous = json.loads(summary_path.read_text(encoding="utf-8"))
            replacements = {item["sample"]: item for item in serialized}
            serialized = [
                replacements.pop(item["sample"], item) for item in previous
            ]
            serialized.extend(replacements.values())
        summary_path.write_text(
            json.dumps(serialized, indent=2) + "\n",
            encoding="utf-8",
        )
    return 1 if failures else 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (FileNotFoundError, OSError, RuntimeError, TypeError, ValueError,
            tomllib.TOMLDecodeError) as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(2)
