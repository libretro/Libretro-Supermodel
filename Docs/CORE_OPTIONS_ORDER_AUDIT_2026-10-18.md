# Core Options Ordering And Final Menu Audit — 2026-10-18

## Result And Scope

The three user-approved items are complete: Framework F04 presentation ordering,
full registered-menu audit and reconciliation of the current roadmap. The normal
macOS ARM core is rebuilt, installed under `supermodel_libretro.dylib` and checked
in actual RetroArch. No commit, push, tag or release was performed.

Only existing definition blocks in `Src/OSD/libretro/libretro_core_options.h` were
moved. No runtime sorting layer, parser, input mapping, native machine behavior,
option key, category, text, choice, label, default or conditional guard changed.
The same sequence feeds V2, V1 and legacy V0 registration. System, its native
operator sequence and all dynamically stored definition indices are unchanged.

## User-Approved Video Follow-Up

The user subsequently approved three consecutive Video task blocks, without new
submenus: rendering engine, image quality/presentation, then cadence/diagnostics.
This supersedes the earlier broad-before-specialized Video sequence and moves
timing/overlay controls after the graphics controls.

The current sequence, omitting options not compiled into a build, is:

1. 3D Renderer, Quad Rendering, Legacy3D Multi-Texture.
2. Internal Resolution, Supersampling, 2D Layer Upscaling Filter, Widescreen Mode,
   CRT Colour, Disable White Flash.
3. A/V Timing, Timing / FPS Overlay.

Only eleven existing definition/guard blocks and their grouping comments were
reordered. The normal macOS ARM build, format/dependency gate and 25 required
exports pass. Final API V2/V1/V0 registrations cover 255 definitions and retain
all per-key metadata; only eight registered Video positions change in this
build. System and every other category retain their previous order. All 177
native operator controls, choices, labels and defaults remain unchanged.
The reused review audit passes all 36 applicable workbook rows and four
conditional-header configurations. No permanent test or Makefile target was
added. This follow-up uses registration evidence; it does not repeat the earlier
actual frontend inspection or claim new gameplay qualification.

The standard installed `supermodel_libretro.dylib` matches the new build:
`ca0360e3f0b64bff689157f64f7e7a1a97bee8c2ee75fc8c985d4d0389eef235`.
Active cores/info/System paths were reread. The .info, frontend configuration
and external System assets remain byte-identical. No routine old-core binary
backup was created; the scoped source snapshot supports rollback by rebuilding.
An already-loaded frontend must reload the core for the new order.

Current follow-up evidence: `outputs/video-order-20261018/`, including
`source-scope.json`, `menu-audit.json`, `header-matrix.json`, `deployment.json`
and build/ABI logs. The original whole-menu audit and frontend evidence below
retain their original scope and fingerprints. No commit or push.

## Reference Adaptation

Framework 0.1.30 F04 defines general-before-specialized controls, preserved task
and dependency groups, and game-prefixed groups last. SM2's
`Docs/FRAMEWORK_FUNCTIONAL_GROUPS_INTERCHANGE_2026-10-17.md` and current
`src/libretro/core_options.h` supply the task-group reference; TGPulse's current
menu registration and `tools/test_libretro_menu_wording.py` supply the full
registered-metadata and grouped-order audit reference. Their actual implementations
were inspected before adaptation. Reference SHA-256 fingerprints are retained in
`outputs/menu-order-20261018/references.json`.

Supermodel already has one shared static sequence and dynamically inserts its
System controls without sorting. Moving the existing static blocks implements
the reviewed order directly; copying another core's runtime sorter is unnecessary.
SM2-only controls, TGPulse MVD/display groups, source gains, Crosshair Style and
Manx TT Cabinet Audio are not invented here.

## Delivered Groups

| Menu | Sequence And Model 3 Adaptation |
| --- | --- |
| System | Existing initialization, networking, NVRAM master and native operator ordering retained exactly. |
| Video | User-approved follow-up: 3D Renderer, Quad Rendering, Legacy3D Multi-Texture; Internal Resolution, Supersampling, 2D Layer Upscaling Filter, Widescreen Mode, CRT Colour, Disable White Flash; A/V Timing, Timing / FPS Overlay. Conditional definitions retain their original guards. |
| Audio | Sound Enable, Sound Volume, Music Volume, SCSP DSP Engine. Existing enable/levels/processing task order retained. Sound Volume controls SCSP, not the final mixed output; it is not treated as a synthetic master gain. |
| Input | Controller Profile Type, Gamepad Rumble; Show Crosshair, Gun Input Mode, Off-Screen Reload Shortcut, Mouse Edge Off-Screen Reload; 4-Speed Shifter; Steering Response then Steering Output Range; Accelerator then Brake Output Range; the two Star Wars Trilogy controls last, ordered by their displayed names. |
| CPU | Emulation Threading, Frame Skip, PowerPC CPU Frequency, JIT Recompiler where available. General host controls precede PowerPC execution controls; published categories are retained. |

The global options are not blindly alphabetized. The four Lightgun controls keep
their reviewed order, Steering Response remains next to its Output Range, and
pedal ranges form the next consecutive subgroup. Star Wars Input Mode precedes
its related inversion fix. No Horizontal/Vertical Range pair exists in this core.
Conditional 3D Renderer remains first when compiled, and Legacy3D/New3D settings
retain their hardware distinctions.

## Verification

This table records the original whole-menu verification. The later Video-only
follow-up and its current delivery are qualified separately above.

| Check | Result |
| --- | --- |
| Normal Build | macOS ARM build passes; only the existing linker alignment warning remains. Existing format, 25-export and dependency gate passes. |
| Final V2 Metadata | 255/255 complete per-key definitions equal the pre-order capture. Exactly 28 global registration positions change; categories and 177 native operator definitions remain identical. |
| System | All 225 registered System definitions retain their complete metadata and exact sequence. No native definition index changes. |
| V1 / V0 | Same key sequence as V2. V1 descriptions/help/choices/labels/defaults match; V0 descriptions and default-first raw-value sequences match its available API representation. No legacy-frontend runtime qualification is inferred. |
| Menu Review | 36/36 current applicable non-NVRAM rows match the authoritative workbook, including effective Name (New), comments and categories. Two rows are build-conditional and covered by the header matrix. |
| Approved Exceptions | Order 195 keeps Internal Resolution without Restart Required and its actual on-Resume help. Order 205 keeps SCSP-specific Sound Volume help. The previous Rumble exception is not restored. |
| Network Follow-Up | Reused destination audit passes all 22 supported pairs, full reviewed help equality, values/labels/defaults/aliases, adjacency and unsupported placeholders. Old Network Board registration remains absent. |
| Conditional Definitions | Before/after header probes compile four feature configurations: full renderer options, Legacy3D-only, current New3D feature set and minimal feature set. Every compiled definition retains all metadata and follows the intended sequence. These are registration probes, not full platform/renderer builds. |
| Identifier / Wording Scan | All final user-facing definitions and value labels pass raw ROM-identifier and stale-wording scans. Native operator text is preserved, not harmonized against the shared non-NVRAM review. |
| Actual Frontend | RetroArch ARM Nightly 1.22.2 / Ozone, The Lost World, installed core. All Input, Video, Audio and CPU pages inspected; System and the unsupported network pair inspected. Eight screenshots saved; no Core Option values changed. Final owned process exits 0. |
| Workbook Preservation | Shared review and Model 3 workbook contents/Order IDs are not edited. |

The authoritative shared Menu Review workbook SHA-256 is
`4ceb9be04d7ad32201c78ae53ac2b103987f8867df69675230996dab7c9c125a`.
The destination audit reuses the existing ROM-free capture tool and F14 audit;
it records expected task sequences separately from the actual registrations and
compares the full pre-order metadata by key. Audit scripts remain task evidence,
not new permanent Makefile targets.

Two preliminary UI attempts did not provide acceptance: the reused temporary QA
bundle lacked Info.plist; a subsequent CUA connection timed out while its bounded
runner terminated. A valid, uniquely identified temporary QA bundle and removal
of its owned empty instance resolved selection. The final isolated run is the
acceptance evidence. No other running user frontend was closed.

## Original Whole-Menu Delivery And Evidence

Core SHA-256:
`7d0ba9c6a7375e184fa409a71e0ff4a632a64e7ce1af0dd9dea35d9e24d7073b`.

Active RetroArch configuration and symlink targets were reread before deployment.
The installed standard core matches the build; the matching .info is unchanged.
External System assets, personal options/remaps and existing saves are not edited
by this ordering change. The final QA run uses isolated configuration/save paths;
audio output is disabled. Scoped source snapshots support rebuild rollback; no
routine binary backup is created.

Evidence: `outputs/menu-order-20261018/`, including `menu-audit.json`,
`registration-audit.json`, `header-matrix.json`, `frontend-verification.json`,
`deployment.json`, build/gate logs and the final frontend screenshots. The prior
networking report and hashes remain provenance; this activity does not rerun
native NVRAM acquisition or claim new synchronized gameplay, physical input,
audio output, linked-board Save State or other-platform qualification.

## Roadmap Reconciliation

`Docs/ROADMAP.md` is the sole current implementation roadmap. The previous text
is retained in `Docs/ROADMAP_HISTORY_2026-10-18.md` as historical evidence, not
active instructions. Current counts are 35 release clones / 472 acquired values
and 396 clone setting rows. Old 34/461 counts and Board/ANS semantics are not
presented as current tasks. Prototype work is excluded unless explicitly requested;
old failed prototype acquisitions are historical rather than completeness blockers.
Remaining visual cycles, platform package gates, optional standalone GUI work and
publication authorization remain separate from these three completed items.
