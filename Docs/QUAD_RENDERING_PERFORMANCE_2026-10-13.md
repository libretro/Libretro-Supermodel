# Quad Rendering Performance Review

Date: 2026-10-13  
Source: Local changes over `4566c9470ab89ec8e2738bfb43c18281df580971`  
Scope: Scud Race attract sequence in real RetroArch; analysis only

Subsequent investigation: [Shader Instruction And Driver Review](QUAD_SHADER_INSTRUCTION_REVIEW_2026-10-13.md)
adds matched GPU instruction probes, draw-call scaling and pristine standalone
upstream reproduction. The findings and limits below describe the first phase.

Quad Rendering produces a large CPU-visible renderer delay on the tested Apple M4/OpenGL-to-Metal path. The same source and GLSL 4.10 shader body do not reproduce that delay on Radeon Vega 11/Mesa. Mac profiling and temporary shader experiments point toward the geometry-shader draw/driver path. They do not establish the driver's private implementation or an OS-only cause independent of hardware.

## Hosts And Artifact Identity

| Item | macOS | Batocera |
| --- | --- | --- |
| Host | Apple M4, macOS 26.5.1 (25F80), ARM64 | Ryzen 5 3400G / Radeon Vega 11, Batocera 43.1, Linux 6.18.16, x86_64 |
| Frontend | RetroArch 1.22.2, `9be7ec92`, ARM Nightly | RetroArch 1.22.2, May 29 2026, GCC 13.3.0 |
| Effective API/driver | OpenGL 4.1, Metal 90.5 | OpenGL 4.6 core, Mesa 25.3.6 / radeonsi / ACO |
| Shader source profile | GLSL 4.10 core | GLSL 4.10 core; core requests OpenGL 4.1 |
| PPC execution | ARM64 JIT build | Standard `platform=unix` interpreter build |

Production macOS binary SHA-256: `17cdde5e287691f8fae90981bdda1b5ecebccab77ec0f1b5137bbcd1af3fe74c`.

Isolated Linux test binary SHA-256: `57f27415269967ea9b191f03552a6ec5e484fc87ee54798eb25b4ff1fd149882`.

Linux was built from an explicit snapshot of current source/build files, excluding ROMs, saves, campaign samples, local object files and Git metadata. The source-file hashes are retained in `outputs/quad-performance-20261013/linux-source-sha256.json`. Debian 12 amd64 through the established Colima/Rosetta builder used `make -j6 platform=unix`, with the project's normal `-O3 -ffast-math` flags, no architecture-specific tuning and no LTO. The ELF is x86_64; its newest required GLIBC version is 2.35, below Batocera's 2.40. Target `ldd` finds every dependency and real RetroArch loads it successfully. The build label is `unknown` because the source snapshot intentionally excludes Git metadata; the manifest and file hashes identify the source instead.

Scud ROM SHA-256 on both hosts: `dca10905f9c0a9461431a4a1fa0c95e67c49d1ad01e03433466a9960d8bbabf5`.

Both probes use current Config assets. Games.xml SHA-256: `66217e4edce3652d04dc51ac96b75dc6da786c962ec2156e297c100d7b52f7bf`. Music.xml SHA-256: `cf7555ca86e8640ee5bbb8e0c0897b073fc86647a93eda10b05065227610478c`.

## Procedure And Measurement Boundary

Reuse `Scripts/libretro_smoke_test.py` environment creation, UDP fast-forward activation and owned-process termination. Batocera transport and bounded execution use the existing `batocera-remote-core-test` skill runner. Each run has fresh isolated system/configuration/save paths, the canonical core basename and matching `.info`; installed cores and global frontend files are not test targets.

Common settings: New3D, `multi_gpu`, native or 2x internal resolution, supersampling 1, core frameskip 0, timing overlay enabled, networking and NVRAM overrides disabled, 60 Hz timing. Windowed frontend, no user shaders, no threaded video, no video/audio synchronization, disabled audio and unlimited fast-forward. Auto overrides, remaps, histories and runtime logs are disabled. Initial NVRAM follows the same default with fresh saves. Logs confirm 496x384 and 992x768 resolved FBOs and the effective Quad selection.

Each frontend invocation requests 1830 frames. Analysis selects core telemetry windows 6–30: discard 305 warm-up core frames, then measure 1525 core frames in 25 windows of 61 frames each. Every selected window reports 61 drawn frames. This avoids comparing different frontend-startup overshoots. Reported p50/p95 are statistics of **61-frame window averages**, not percentiles of individual frame times.

These are repeated fresh-boot attract runs, without a deterministic replay or restored state and without corresponding image checks. Core-frame alignment improves comparison but does not prove identical displayed scenes. The results localize this large repeatable delay; they are not a general game-library benchmark or image-fidelity qualification. Cross-host absolute FPS also reflect different CPUs, architectures, JIT availability and frontend builds. Compare ON/OFF within each host rather than attributing their absolute difference solely to the OS.

`Render` wraps Model 3 renderer work and includes CPU submission and driver waits. It is not a GPU timer. With `multi_gpu`, PPC and renderer work overlap; their times must not be added as independent callback stages. `actual` is uncapped measured throughput, not a selected game-speed target.

## Results

Aligned medians; times in milliseconds. macOS rows use the controlled `.info` repeat.

| Host / Resolution | Quad | PPC | Render | retro_run | Actual FPS |
| --- | --- | ---: | ---: | ---: | ---: |
| Apple M4 / Native | Disabled | 2.8 | 1.1 | 3.1 | 320.6 |
| Apple M4 / Native | Enabled | 3.0 | 26.5 | 27.5 | 37.3 |
| Apple M4 / 2x | Disabled | 2.8 | 2.8 | 3.5 | 282.3 |
| Apple M4 / 2x | Enabled | 3.1 | 27.8 | 28.4 | 35.8 |
| Vega 11 / Native | Disabled | 13.3 | 1.8 | 14.3 | 69.5 |
| Vega 11 / Native | Enabled | 13.5 | 1.8 | 14.5 | 68.6 |
| Vega 11 / 2x | Disabled | 13.6 | 1.8 | 14.8 | 67.3 |
| Vega 11 / 2x | Enabled | 13.5 | 1.8 | 14.9 | 66.9 |

Two earlier macOS native pairs reproduce Render 1.1 ms OFF versus 25.9/26.2 ms ON, with PPC 2.8–3.0 ms. The Batocera native repeat gives Render 1.6 ms OFF versus 1.8 ms ON and `retro_run` 14.5 ms for both. All six final Batocera runs exit 0, complete 30 telemetry windows and activate fast-forward; the encompassing remote runner also exits 0.

On macOS native resolution, Render window-p95 increases from 3.3 to 81.7 ms and callback window-p95 from 3.4 to 82.4 ms. Batocera native Render window-p95 is 2.4 ms OFF and 2.3 ms ON, with callback window-p95 14.6 and 15.3 ms. The complete series and other percentiles are retained in `aligned-analysis.json`.

Batocera host CSVs record approximately 19–20% total CPU use and 133–136% cumulative RetroArch process CPU after the first five wall samples. The GPU busy counter reads 100% for both modes, while reported active clocks vary and often remain low. These counters do not discriminate the modes and cannot establish the GPU bottleneck. `ps %cpu` is cumulative, and this wall-time host summary is not exactly aligned to the core-frame window. Missing memory-busy counters remain unavailable.

## Mac Diagnostic Evidence

The production source, repository core and installed macOS core were not changed by these experiments. Private shader copies were compiled and linked into temporary binaries using the production build commands and existing non-shader objects.

| Temporary Variant | Native Render p50 | Purpose And Limit |
| --- | ---: | --- |
| Production Quad | 26.5 ms | Native reference behavior |
| Force fragment triangle fallback, retain full geometry stage | 27.0 ms | Avoid expensive native quad interpolation at runtime; preserve the shader interface and mutable branch |
| Same fallback, remove geometry cross/area arithmetic | 26.3 ms | Retain adjacency input, four emitted vertices and output fields, with dynamic diagnostic area values |

The lean diagnostic logs the actual mutable fallback uniform as `value=1`. Neither variant is an image-equivalent replacement or a proposed production fix. Their retained geometry interfaces/output footprint can still contribute to driver cost. They show that merely simplifying the interpolation arithmetic does not remove the large delay.

Fourfold pixel count from native to 2x changes production Quad Render from 26.5 to 27.8 ms, while PPC stays near 3 ms. Together with the temporary variants, this argues against fragment arithmetic/fill rate as the dominant explanation for this particular penalty.

A separate three-second CPU sample of the production Quad run, excluded from performance ratios, finds 426 main-thread samples inside `glDrawArrays_ACC_GL3Exec`. Prominent branches enter `AppleMetalOpenGLRenderer::gldRenderVertexArray`, create Metal command buffers and wait on a semaphore (217 samples in one such branch). This directly locates substantial CPU-visible time in driver draw/submission and queue waits. It does **not** prove that geometry shaders run in software, that PPC emulation is responsible, or which private driver scheduling mechanism causes the waits.

The native quad geometry/fragment shader body was previously reconciled with upstream `7d3b6f4`/PR #339 in [Upstream Integration](UPSTREAM_INTEGRATION.md). This investigation has not separately benchmarked a pristine standalone build on the same Mac, so a port/driver interaction is not excluded.

## Retained Failures And Reproduction

An initial Mac diagnostic used a noncanonical binary name without matching core info. RetroArch crashed in `GET_STATUS`/`command_get_status`/`strlcpy`; that run is excluded. Using a temporary canonical basename and matching info fixes the harness prerequisite. It is not evidence of a Quad crash.

The first Batocera harness read only console output, but that frontend wrote timing records into isolated `logs/retroarch.log`. Its first OFF run exited successfully; the harness correctly stopped for missing parsed telemetry. Both the failure and actual file log are retained. The corrected bundle reads the file logs and completes all six cases in a separate marked directory.

Evidence root: `outputs/quad-performance-20261013/`. It contains source/configuration hashes, raw logs, summaries, the separate CPU sample, host CSVs, build log and dated reproduction scripts/shader copies. Recompute the aligned report with:

```sh
python3 outputs/quad-performance-20261013/reproduction/analyze.py
```

Retained runners are dated diagnostic snapshots, with explicit host paths to adapt before re-running; they are not new permanent Makefile checks. Remote bundle reproduction uses the existing skill's `plan` and `all` commands, root `/userdata/system/codex-core-tests/supermodel-quad-perf-20261013-r2`, timeout 900 seconds and read-only `/userdata/roms/model3/scud.zip`. Use a fresh marked root for another run. Remote evidence directories remain for review; no cleanup was requested. Owned RetroArch/collector processes exited. The shared build VM was already running and was left running.

## Outcome And Follow-Up

The large tested slowdown is localized to the Apple Quad geometry/driver path, with the exact private mechanism unresolved. AMD/Mesa provides a useful counterexample to a universal 25 ms Quad cost. No production workaround, compiler tuning, option default change or installed-core replacement is delivered by this analysis.

Keep the published Disabled default. A subsequent optimization should first isolate geometry output/submission cost on the same Mac, with a pristine same-revision upstream baseline where feasible. Any alternative geometry preparation or batching must preserve native quad interpolation and pass controlled image comparison plus triangle/Quad backend checks. The arithmetic-only diagnostic variants must not be shipped. This investigation and its measurement boundaries are reported to the Framework under F16/F17; other cores require their own applicable baseline.
