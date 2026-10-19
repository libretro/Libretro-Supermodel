# Supermodel — Sega Model 3 Libretro Core

[![Libretro Core CI](https://github.com/Zer0one/Libretro-Supermodel/actions/workflows/libretro-ci.yml/badge.svg?branch=main)](https://github.com/Zer0one/Libretro-Supermodel/actions/workflows/libretro-ci.yml)

A C++17 Libretro port of Supermodel for running Sega Model 3 arcade games in
RetroArch. The active development branch is `main`.

This port builds on [libretro/Libretro-Supermodel](https://github.com/libretro/Libretro-Supermodel)
and the frontend work of [sgiannop/Libretro-Supermodel](https://github.com/sgiannop/Libretro-Supermodel),
using [standalone Supermodel](https://github.com/trzy/Supermodel) as the machine
and renderer reference. Upstream changes are reviewed selectively; see the
[integration ledger](UPSTREAM_INTEGRATION.md) for the exact scope.

## Downloads And Installation

[GitHub Actions](https://github.com/Zer0one/Libretro-Supermodel/actions/workflows/libretro-ci.yml)
builds Linux x86_64, Windows x86_64, macOS ARM64 and macOS Intel artifacts from
this branch. Open a successful run and download the artifact for your platform
(a GitHub account is required). Native macOS builds are separate binaries for
each architecture.

Tagged packages are available under
[Releases](https://github.com/Zer0one/Libretro-Supermodel/releases).
The current port release is `v0.3a-20261012-git-ed608de-libretro.1`, based on the selectively integrated
2026-10-12 upstream snapshot. See the [release notes](RELEASE_NOTES.md) for
changes, platform coverage and limitations.

1. Copy `supermodel_libretro.so`, `.dll` or `.dylib` to RetroArch's configured
   **Cores** directory.
2. Copy `supermodel_libretro.info` to its configured **Core Info** directory.
3. Copy the package's `system/supermodel/` files to
   `[RetroArch System Directory]/supermodel/`.
4. Load a compatible ROM-set ZIP using the Supermodel core.

Packages include the core, `.info`, `Games.xml`, `Music.xml`, license notices,
source provenance and SHA-256 checksums. ROMs are not included. Linux and
Windows packages can be checked with `sha256sum -c SHA256SUMS`; on macOS use
`shasum -a 256 -c SHA256SUMS`.

## Features

- Game-specific controller profiles, keyboard, mouse and Lightgun input;
  remappable Service/Test controls and native drive-board rumble.
- Optional per-game NVRAM settings and validated first-boot initialization,
  with normal frontend-owned `.srm` persistence.
- Experimental linked-cabinet play through the official Libretro Netpacket
  interface, including independent native role selection.
- OpenGL New3D rendering, optional native Quad interpolation on desktop
  OpenGL 4.1, internal resolution scaling and widescreen enhancements.
- Native Libretro audio callbacks, C++17 threading, selectable 60 FPS/native
  Model 3 timing and a diagnostic timing overlay.
- Core Options API 2, 1 and legacy registration, with reviewed menu grouping,
  help text and game-specific visibility.

## Recent Development Changes

The development branch selectively incorporates newer standalone corrections:

- Native wheel drive-board servo gain, vibration decoding, idle stop and command
  ordering, with compatibility for states lacking the optional servo block.
- PowerPC interpreter arithmetic/carry, FPSCR and conversion edge-case fixes.
- Quad triangle interpolation, bounded polygon/list traversal, LOD handling,
  decal depth/order and LOS sampling after transparency rendering. The port's
  OpenGL 4.1 support, asynchronous LOS and live renderer reattachment are retained.

These changes are newer than the tagged release above. Local macOS ARM64 checks
cover targeted CPU/native renderer cases, representative game startup/state/reset
and live video-option transitions; other-platform gameplay and physical rumble
are not implied. See the [integration ledger](UPSTREAM_INTEGRATION.md) for scope,
upstream provenance and exclusions. Standalone GUI and Pro-1000 expansion are
outside this project's scope.

## System Files And Saves

The preferred support-file location is
`[RetroArch System Directory]/supermodel/`. The older `supermodel/Config/`
layout remains a compatibility fallback.

| File | Purpose |
| --- | --- |
| `Games.xml` | ROM identification and machine definitions. An embedded copy is extracted when no external file exists. |
| `Music.xml` | Optional music metadata. |
| `Supermodel.ini` | Optional advanced, read-only override. An embedded default is extracted when missing. Existing files are never overwritten. |

Existing external catalogues are authoritative. When upgrading, install the
package's `Games.xml` if you want the updated definitions; the core does not
silently replace an existing copy. The current catalogue removes `mgtrkbad`,
restores Scud Australia's normal operator menu and expects `vs298` component
`epr-20917.17` with CRC `969E4BDA`.

RetroArch normally stores NVRAM as `<content>.srm`. To import a standalone
Supermodel save, place `<rom-set>.nv` in the configured save directory. It is
read only when no `.srm` data was supplied; `.srm` takes precedence. Invalid
native saves are ignored, and the core never writes the imported `.nv` file.

**Automatic Initial NVRAM Setup** affects newly created saves and supplies
validated initial settings, including Single/Stand Alone/No Link where
applicable. **NVRAM Settings** is separately disabled by default and exposes
only reviewed fields for the loaded set. Existing valid saves are preserved by
initialization; enabled overrides deliberately apply the selected fields.

Effective operator or network changes are combined into one machine reset on
the next emulation frame. In a paused Quick Menu this occurs on **Resume**;
**Restart** also consumes pending choices once. In non-pausing Netplay menus,
emulation continues and changes can therefore reset immediately. Reliable
menu-exit deferral is not implemented in that context. Unchanged or equivalent
choices do not reset the machine.

## Controls

Profiles and labels follow the loaded game's controls. **Controller Profile
Type** defaults to Normal + Test/Service, including **L3 = Service** and
**R3 = Test**; Normal
omits those cabinet slots. Single-View profiles map both D-Pad Up and Down to
Change View where there is no conflicting action. See the
[control profiles](CONTROL_PROFILES.md) for per-game mappings.

Standard Gun input combines gamepad, mouse and Lightgun sources. The Lost
World supports the optional reload shortcuts; the other Model 3 gun titles do
not require that reload action. Existing dedicated input modes remain
available. Automatic crosshairs show the core reticle for Lost World and hide
it for the two gun titles with native reticles.

Rumble forwards emulated drive-board commands to the frontend's rumble API.
It does not synthesize a separate cornering effect.

## Video And Audio

Desktop New3D requests an **OpenGL 4.1 core profile**. GLES targets use
**OpenGL ES 3.0**. Quad Rendering uses New3D's quadrilateral interpolation and
is available on desktop OpenGL 4.1, including macOS; it is disabled by default
and requires a content restart. It is unavailable on GLES and ignored by
Legacy3D.

Quad Rendering can be substantially slower on Apple's OpenGL-to-Metal path.
The completed [performance investigation](QUAD_RENDERING_PERFORMANCE_2026-10-13.md)
and [shader follow-up](QUAD_SHADER_INSTRUCTION_REVIEW_2026-10-13.md) reproduce the
penalty and document the tested alternatives. No safe production workaround
was adopted; keep Quad Rendering disabled when performance is insufficient.

**Aspect Ratio** offers Auto (default), 4:3 and 16:9. Auto uses the presentation
selected by Widescreen Mode; explicit 4:3 suppresses expansion. With Widescreen
Mode Disabled, 16:9 stretches the original complete image. Aspect Ratio changes
apply when returning to the game without a machine reset.

**Widescreen Mode** keeps native rendering when Disabled. Either Expand mode
widens the 3D view and reports 16:9 only when Aspect Ratio is Auto or 16:9. There is no verified native
Model 3 widescreen setting on which to base automatic aspect selection.

| Mode | Result |
| --- | --- |
| Disabled (Default) | Original 496 × 384 rendering. Aspect Ratio 16:9 stretches the complete image; Auto and 4:3 retain 4:3. |
| Expand 3D View Only | Wider full-screen 3D viewports in a 16:9 framebuffer, keeping original 2D geometry. |
| Expand 3D View + Stretch 2D Background | Also widens the lower background layer; HUD/upper overlays retain their original geometry. |

These labels retain the saved values `disabled`, `enabled` and `wide_background`
respectively. They do not change rendering behavior or the default. Unlike
TGPulse's Stretch 2D mode, Supermodel stretches only its lower background layer.

Aspect Ratio and Widescreen Mode changes apply when returning to the game, without a machine reset or content reload. Use RetroArch's Core Provided aspect setting to honor the core selector. Final output stretching and
scaling remain frontend controls. **2D Layer Upscaling Filter** affects tile
layers above native resolution; native rendering uses nearest-neighbor.

**SCSP DSP Engine** defaults to the MAME-derived implementation, with the
legacy ElSemi engine retained as a compatibility choice. This setting and the
2D filter require a content restart. Sound and DSB music volume range from
0 to 200%.

**A/V Timing** defaults to 60 FPS, submitting 735 stereo samples per frame at
44,100 Hz. Native 57.524160 Hz timing uses a fractional 766/767-sample cadence
and requires multi-threaded emulation. RetroArch owns presentation, VSync,
audio synchronization and final resampling. Insufficient emulation speed can
cause audio stuttering.

**Timing / FPS Overlay** separates engine, GPU, frontend submission and
presentation costs. Engine/callback capacity are diagnostic estimates, not
measured displayed FPS. The current branch also fixes persistent silent audio
after a machine reset by restoring the startup audio-queue initialization.

## Linked-Cabinet Networking

Networking is experimental and requires a frontend with Libretro Netpacket
support. The core opens no sockets and performs no peer discovery.

Exactly two controls are visible: **Linked Cabinets** and **Auto Network Role
Settings (ANRS)**. Unsupported games show Not Supported. There is no separate
Network Board switch.

**Linked Cabinets** defaults to Disabled (`-1`; `0` and `1` are accepted
aliases). All currently reviewed Model 3 boards are optional, so Disabled
disconnects the emulated board. Numeric choices specify the total participants,
including a special-role cabinet where supported.

| Game Family | Selectable Total |
| --- | --- |
| Daytona USA 2 | 2–17 |
| Scud Race | 2–8 |
| Dirt Devils, Le Mans 24, Sega Rally 2, Virtual On 2 | 2–5 |
| Harley-Davidson, Ski Champ, Spikeout | 2–4 |

These are reviewed theoretical limits; every total and role combination has
not been qualified in multiplayer gameplay.

**ANRS** defaults to the native Single, Stand Alone or No Link role (`0`).
Disabled (`-1`) restores manual management. Numeric presets select native
Master/Slave and the game's identity; `r` selects a supported Live, Relay or
Satellite role. Native Master is independent of the RetroArch host/client ID.
Topology and native role remain independent, so the user must choose a valid
combination.

ANRS operates even with NVRAM Settings disabled. It hides only managed native
role/identity fields and keeps their selectors synchronized. Disabling it
returns the last applied values to manual management; unrelated fields and
native integrity are preserved. Scud Australia's Link ID and Car Number now
have directly verified mappings.

Machine resets preserve the frontend lobby, transport callbacks and connected
clients. Peers require compatible content, protocol-v3 cores, participant
totals and timing. Actual Scud two-peer checks establish roster completion and
initial data exchange, including native Master on the frontend client.
Synchronized racing, all role combinations, maximum totals, linked-board Save
States and cross-platform runtime behavior remain unqualified. See the
[networking report](NETWORK_TWO_OPTION_2026-10-17.md) for evidence and limits.

## Building

The unified Makefile contains desktop, Android and ARM Linux targets. The
normal CI matrix validates Linux x86_64, Windows x86_64 and separate macOS ARM64
and Intel builds, exported Libretro API, dependencies and package contents.
Android/Raspberry Pi/other ARM targets are not covered by that CI matrix;
build-target availability does not establish runtime compatibility or speed.

### Linux x86_64

```sh
sudo apt-get install build-essential libgl-dev libglu1-mesa-dev zlib1g-dev
make platform=unix -j"$(nproc)"
```

Output: `supermodel_libretro.so`.

### macOS

Install Xcode Command Line Tools, then build natively:

```sh
make platform=osx -j"$(sysctl -n hw.ncpu)"
```

Output: `supermodel_libretro.dylib` for the host architecture. CI builds Intel
and Apple Silicon separately. The Makefile also retains an osxcross path for
Linux cross-compilation; it is not the current native CI workflow.

### Windows x86_64

In an MSYS2 **MINGW64** shell:

```sh
pacman -S --needed make mingw-w64-x86_64-gcc mingw-w64-x86_64-zlib
make platform=win -j"$(nproc)"
```

Output: `supermodel_libretro.dll`. Linux-to-Windows cross-compilation requires
MinGW-w64 and matching zlib headers/libraries.

### Additional Targets

```sh
# Android NDK; arch can also be arm, x86_64 or x86
NDK_ROOT=/path/to/ndk make platform=android arch=arm64 -j4

# ARM Linux cross-compilation with the matching toolchain and dependencies
make platform=rpi64 -j4
make platform=aarch64 -j4

# Debug build / cleanup
make platform=osx DEBUG=1 -j4
make platform=osx clean
```

Use `.github/scripts/validate-libretro-core.sh CORE FORMAT_REGEX` for the
binary gate and `python3 .github/scripts/package-libretro-core.py CORE OUTPUT`
to assemble a package into an empty directory. `--check` verifies an existing
package.

## Project Status And References

The [roadmap](ROADMAP.md) distinguishes implemented features, runtime evidence
and remaining work. The [Service Menu and NVRAM summary](Service_Menu_NVRAM_Summary.xlsx)
is the single current campaign workbook, covering the native inventory,
implemented options/defaults/encodings, initialization templates and evidence.
[Diagnostic recipes](diagnostic-menus/) retain the native menu routes.

The released-clone binary campaign is complete at 472 values across 35 clones.
Ten visual value cycles for `vs298` remain blocked by the local old ROM dump;
this does not invalidate accepted samples. Prototype NVRAM campaigns remain
outside the established scope. Workbook notes distinguish direct clone proof,
parent evidence, binary validation and actual frontend reloads.

Shared frontend conventions follow the Libretro Core Framework as a
documentation reference, not a runtime dependency. This repository's
[menu audit](CORE_OPTIONS_ORDER_AUDIT_2026-10-18.md),
[Core Options interchange](CORE_OPTIONS_UPDATE_FRAMEWORK_INTERCHANGE_2026-10-13.md)
and roadmap record the implemented scope and Supermodel-specific limits.

Subsequent integration, feature work, diagnosis and validation were developed
with assistance from **ChatGPT 5.6 Sol** as part of a study project. This
historical disclosure does not replace independent review or testing.

The project is distributed under [GPLv3+](LICENSE.txt); see also
[third-party notices](THIRD_PARTY_NOTICES.txt).
