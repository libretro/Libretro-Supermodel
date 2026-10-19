#!/usr/bin/env python3
"""Acquire release-clone NVRAM samples from the validated parent recipes."""

from __future__ import annotations

import argparse
import dataclasses
import json
import os
import re
import sys
from dataclasses import asdict
from pathlib import Path

import libretro_nvram_samples as sampler


PROFILES: dict[str, tuple[str, tuple[str, ...]]] = {
    "getbassdx": ("bassdx", ("difficulty", "country")),
    "getbassur": ("bassdx", ("difficulty", "country")),
    "getbass": ("bassdx", ("difficulty", "country")),
    "dayto2pe": ("daytona2", ("difficulty", "country", "cabinet", "link", "link-id", "vocal")),
    "dirtdvlsu": ("dirtdvls", ("difficulty", "country", "link", "link-id")),
    "dirtdvlsau": ("dirtdvls", ("difficulty", "country", "link", "link-id")),
    "dirtdvlsj": ("dirtdvls", ("difficulty", "country", "link", "link-id")),
    "dirtdvlsg": ("dirtdvls", ("difficulty", "country", "link", "link-id")),
    "ecau": ("eca", ("difficulty",)),
    "ecaj": ("eca", ("difficulty",)),
    "fvipers2o": ("fvipers2", ("difficulty", "country")),
    "harleya": ("harley", ("difficulty", "country", "cabinet", "link", "link-id")),
    "oceanhuna": ("oceanhun", ("difficulty", "country", "cabinet")),
    "scudau": ("scud", ("difficulty", "cabinet", "link", "link-id")),
    "scuddx": ("scud", ("difficulty", "country")),
    "scuddxo": ("scud", ("difficulty", "country")),
    "scudplus": ("scud", ("difficulty", "country", "cabinet", "link", "link-id")),
    "scudplusa": ("scud", ("difficulty", "country", "cabinet", "link", "link-id")),
    "srally2dx": ("srally2", ("difficulty", "country", "default-view")),
    "swtrilgya": ("swtrilgy", ("difficulty", "country", "cabinet", "deluxe-lever-feedback")),
    "vf3a": ("vf3", ("difficulty", "country")),
    "vf3c": ("vf3", ("difficulty", "country")),
    "vf3tb": ("vf3", ("difficulty", "country")),
    "von254g": ("von2", ("difficulty", "country", "link", "link-id", "display-type")),
    "von2a": ("von2", ("difficulty", "country", "link", "link-id", "display-type")),
    "von2o": ("von2", ("difficulty", "country", "link", "link-id", "display-type")),
    "vs215": ("vs2", ("difficulty", "country")),
    "vs215o": ("vs2", ("difficulty",)),
    "vs29815": ("vs298", ("difficulty",)),
    "vs299a": ("vs2v991", ("difficulty", "country")),
    "vs299": ("vs2v991", ("difficulty", "country")),
    "vs299j": ("vs2v991", ("difficulty",)),
    "vs29915": ("vs2v991", ("difficulty", "country")),
    "vs29915a": ("vs2v991", ("difficulty", "country")),
    "vs29915j": ("vs2v991", ("difficulty",)),
}

GROUP_RE = re.compile(r"-\d\d(?:-|$).*")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--clone", action="append", default=[], choices=sorted(PROFILES))
    parser.add_argument("--group", action="append", default=[],
                        help="acquire only this setting group (repeatable)")
    parser.add_argument("--sample", action="append", default=[],
                        help="acquire only this normalized sample suffix")
    parser.add_argument("--smoke", action="store_true",
                        help="run only step 0 for each setting group")
    parser.add_argument("--overwrite", action="store_true")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--keep-workdirs", action="store_true")
    parser.add_argument("--output-root", type=Path, default=Path(
        "~/Documents/RetroArch/model3-clone-nvram-campaign"
    ))
    parser.add_argument("--catalog-root", type=Path, default=Path(
        "~/Documents/RetroArch/model3-game-settings-catalog"
    ))
    parser.add_argument("--core", type=Path, default=Path(
        "~/Library/Application Support/RetroArch/cores/supermodel_libretro.dylib"
    ))
    return parser.parse_args()


def group_name(suffix: str) -> str:
    return GROUP_RE.sub("", suffix)


def action_tokens(tokens: list[str]) -> tuple[sampler.Action, ...]:
    return tuple(sampler.validate_close(sampler.expand_actions(tokens, {})))


def stable_actions(actions: tuple[sampler.Action, ...]) -> tuple[sampler.Action, ...]:
    """Give clone Service Menus enough time to accept adjacent commands."""
    return tuple(
        sampler.Action("wait", max(float(action.value), 1.0))
        if action.kind == "wait" and float(action.value) < 1.0 else action
        for action in actions
    )


def exceptional_sample(clone: str, group: str, step: int) -> sampler.Sample | None:
    common_start = ["TEST", "WAIT(2.0)"]
    if clone == "dayto2pe" and group == "difficulty":
        tokens = common_start + [
            "SERVICE*8", "TEST", "WAIT(1.0)", "SERVICE*4", f"TEST*{step}",
            "WAIT(1.0)", "SERVICE*9", "TEST", "WAIT(1.0)",
            "SERVICE*5", "TEST", "WAIT(5.0)", "CLOSE",
        ]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone == "dayto2pe" and group == "link":
        leave = (12, 4, 2, 11)[step]
        tokens = common_start + [
            "SERVICE*8", "TEST", "WAIT(1.0)", "SERVICE", f"TEST*{step}",
            "WAIT(1.0)", f"SERVICE*{leave}", "TEST", "WAIT(1.0)",
            "SERVICE*5", "TEST", "WAIT(5.0)", "CLOSE",
        ]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone == "dayto2pe" and group == "link-id":
        tokens = common_start + [
            "SERVICE*8", "TEST", "WAIT(1.0)", "SERVICE", "SERVICE", f"TEST*{step}",
            "WAIT(1.0)", "SERVICE*11", "TEST", "WAIT(1.0)",
            "SERVICE*5", "TEST", "WAIT(5.0)", "CLOSE",
        ]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone == "dayto2pe" and group == "cabinet":
        tokens = common_start + [
            "SERVICE*8", "TEST", "WAIT(1.0)", "SERVICE*3", f"TEST*{step}",
            "WAIT(1.0)", "SERVICE*10", "TEST", "WAIT(1.0)",
            "SERVICE*5", "TEST", "WAIT(5.0)", "CLOSE",
        ]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone == "dayto2pe" and group == "vocal":
        tokens = common_start + [
            "SERVICE*8", "TEST", "WAIT(1.0)", "SERVICE*6", f"TEST*{step}",
            "WAIT(1.0)", "SERVICE*7", "TEST", "WAIT(1.0)",
            "SERVICE*5", "TEST", "WAIT(5.0)", "CLOSE",
        ]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone in {"scudplus", "scudplusa"} and group == "country":
        tokens = common_start + [
            "SERVICE*8", "TEST", "WAIT(1.0)", "SERVICE*4", f"TEST*{step}",
            "WAIT(1.0)", "SERVICE*11", "TEST", "WAIT(1.0)",
            "SERVICE*4", "TEST", "WAIT(5.0)", "CLOSE",
        ]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone in {"scuddx", "scuddxo"}:
        select, leave = (2, 4) if group == "difficulty" else (1, 5)
        tokens = common_start + [
            "SERVICE*8", "TEST", "WAIT(0.5)", f"SERVICE*{select}", f"TEST*{step}",
            "WAIT(0.5)", f"SERVICE*{leave}", "TEST", "WAIT(0.5)",
            "SERVICE*4", "TEST", "WAIT(5.0)", "CLOSE",
        ]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone == "vf3tb":
        select, leave = (3, 14) if group == "difficulty" else (12, 5)
        tokens = common_start + [
            "SERVICE*9", "TEST", "WAIT(0.5)", f"SERVICE*{select}", f"TEST*{step}",
            "WAIT(0.5)", f"SERVICE*{leave}", "TEST", "WAIT(0.5)",
            "SERVICE*6", "TEST", "WAIT(5.0)", "CLOSE",
        ]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone in {"vf3a", "vf3c"} and group == "difficulty":
        tokens = common_start + [
            "SERVICE*9", "TEST", "WAIT(1.0)", "SERVICE*3", f"TEST*{step}",
            "WAIT(1.0)", "SERVICE*13", "TEST", "WAIT(1.0)",
            "SERVICE*6", "TEST", "WAIT(5.0)", "CLOSE",
        ]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone == "srally2dx" and group in {"difficulty", "default-view"}:
        select, leave = (2, 5) if group == "difficulty" else (4, 3)
        tokens = ["TEST", "WAIT(0.7)", "SERVICE*9", "TEST", "WAIT(0.5)",
                  f"SERVICE*{select}", f"TEST*{step}", "WAIT(0.5)",
                  f"SERVICE*{leave}", "TEST", "WAIT(0.5)",
                  "SERVICE*3", "TEST", "WAIT(3.0)", "CLOSE"]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone == "scudau" and group in {"link", "link-id", "cabinet", "difficulty"}:
        select = {"link": 1, "link-id": 2, "cabinet": 3, "difficulty": 4}[group]
        leave = 9 - select
        tokens = common_start + [
            "SERVICE*8", "TEST", "WAIT(1.0)", f"SERVICE*{select}",
            f"TEST*{step}", "WAIT(0.5)", f"SERVICE*{leave}", "TEST",
            "WAIT(0.5)", "SERVICE*4", "TEST", "WAIT(5.0)", "CLOSE",
        ]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone == "getbass" and group == "difficulty":
        tokens = ["TEST", "WAIT(0.7)", "SERVICE*9", "TEST", "WAIT(0.5)",
                  "SERVICE*1", f"TEST*{step}", "WAIT(0.5)",
                  "SERVICE*3", "TEST", "WAIT(0.5)",
                  "SERVICE*5", "TEST", "WAIT(3.0)", "CLOSE"]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    if clone in {"vs299j", "vs29915j"} and group == "difficulty":
        # These Japanese baselines reopen Game Assignments on COUNTRY rather
        # than EXIT, so Difficulty is two Service presses away, not three.
        tokens = common_start + [
            "SERVICE*8", "TEST", "WAIT(1.0)", "SERVICE*2", f"TEST*{step}",
            "WAIT(1.0)", "SERVICE*10", "TEST", "WAIT(0.5)",
            "SERVICE*6", "TEST", "WAIT(5.0)", "CLOSE",
        ]
        return sampler.Sample(f"{group}-step-{step:02d}", action_tokens(tokens))
    return None


def selected_samples(clone: str, parent_samples: list[sampler.Sample],
                     groups: tuple[str, ...], smoke: bool) -> list[sampler.Sample]:
    counters = {group: 0 for group in groups}
    selected: list[sampler.Sample] = []
    for sample in parent_samples:
        group = group_name(sample.suffix)
        if group not in counters:
            continue
        step = counters[group]
        counters[group] += 1
        if smoke and step != 0:
            continue
        replacement = exceptional_sample(clone, group, step)
        selected_sample = replacement or sampler.Sample(
            f"{group}-step-{step:02d}", sample.actions
        )
        selected.append(sampler.Sample(
            selected_sample.suffix, stable_actions(selected_sample.actions)
        ))
    missing = [group for group, count in counters.items() if count == 0]
    if missing:
        raise ValueError(f"{clone}: parent recipe lacks groups: {', '.join(missing)}")
    return selected


def main() -> int:
    if sys.platform != "darwin":
        raise RuntimeError("clone NVRAM acquisition currently supports macOS only")
    args = parse_args()
    scripts = Path(__file__).resolve().parent
    output_root = args.output_root.expanduser().resolve()
    catalog_root = args.catalog_root.expanduser().resolve()
    core = args.core.expanduser().resolve()
    clones = args.clone or list(PROFILES)
    failures = 0

    for clone_index, clone in enumerate(clones, 1):
        parent, groups = PROFILES[clone]
        if args.group:
            groups = tuple(group for group in groups if group in args.group)
            if not groups:
                continue
        recipe = scripts / f"libretro_nvram_samples.{parent}.toml"
        recipe_args = argparse.Namespace(
            config=recipe, sample=[], resume_from=None, interactive=None,
            output=None, dry_run=False, overwrite=False, keep_workdirs=False,
        )
        original, parent_samples = sampler.load_campaign(recipe_args)
        samples = selected_samples(clone, parent_samples, groups, args.smoke)
        if args.sample:
            requested = set(args.sample)
            samples = [sample for sample in samples if sample.suffix in requested]
            missing = requested - {sample.suffix for sample in samples}
            if missing:
                raise ValueError(
                    f"{clone}: unknown selected samples: {', '.join(sorted(missing))}"
                )
        output = output_root / clone
        settings = dataclasses.replace(
            original,
            game=clone,
            core=core,
            standard_srm=(catalog_root / clone / "saves" /
                          f"{clone}-menu-structure.srm"),
            output_dir=output,
            capture_helper=output_root / ".tools" / "ScreenCaptureWindow",
            startup_wait=max(original.startup_wait, 10.0),
            button_hold=max(original.button_hold, 0.12),
            default_wait=max(original.default_wait, 0.40),
        )
        sampler.validate_paths(settings)
        print(f"[{clone_index:02d}/{len(clones):02d}] {clone}: {len(samples)} samples", flush=True)
        if args.dry_run:
            sampler.print_dry_run(settings, samples)
            continue
        results = []
        for sample_index, sample in enumerate(samples, 1):
            result = sampler.run_sample(
                settings, sample, args.overwrite, args.keep_workdirs
            )
            results.append(result)
            failures += result.status not in {"ok", "skipped"}
            print(f"  [{sample_index:02d}/{len(samples):02d}] {sample.suffix}: {result.status}", flush=True)
        output.mkdir(parents=True, exist_ok=True)
        summary_name = ("repair-summary.json" if args.sample else
                        "smoke-summary.json" if args.smoke else "summary.json")
        (output / summary_name).write_text(
            json.dumps([asdict(result) for result in results], indent=2) + "\n",
            encoding="utf-8",
        )
    return 1 if failures else 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (FileNotFoundError, OSError, RuntimeError, TypeError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(2)
