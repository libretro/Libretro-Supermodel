# Supermodel (Sega Model 3) - Libretro Core

A Libretro port of [Supermodel](https://github.com/trzy/Supermodel), the Sega
Model 3 arcade emulator, for RetroArch and other Libretro frontends. The core
is written in C++17 and builds for Linux, Windows, macOS, Android, iOS, tvOS,
Raspberry Pi and other ARM64 Linux systems.

## Features

- **Native Libretro integration:** RetroArch owns video, audio, input, saves
  and timing. Video is rendered with OpenGL (a desktop GL 4.1 core profile for
  New3D, a compatibility context for Legacy3D, or OpenGL ES 3.0 on mobile and
  ARM boards); GL entry points are resolved through
  libretro-common `glsym`, so no system GL extension library is needed.
- **Model 3 timing:** the core runs at the Model 3 refresh rate of 57.53 Hz with
  44.1 kHz stereo audio, delivered synchronously through `audio_batch_cb`.
- **Save states and NVRAM:** full save-state support (also used by RetroArch
  run-ahead and netplay) and persistent NVRAM in the frontend's `.srm` save.
- **ARM64 JIT recompiler:** a PowerPC-to-ARM64 dynamic recompiler on Android
  arm64, Raspberry Pi / ARM64 Linux and Apple Silicon macOS builds. It can be
  turned off to use the interpreter.
- **Emulation threading:** sound and drive boards on worker threads, optionally
  overlapping PowerPC emulation with graphics processing.
- **Two 3D renderers:** New3D (default, accurate) and the older Legacy3D, which
  can be faster on old or integrated GPUs (desktop builds; see
  [Legacy3D on GLES](#legacy3d-on-gles)).
- **Video options:** internal resolution from half to 4x native, supersampling,
  CRT colour profiles, a widescreen hack (optionally with a wide background
  layer), 2D upscaling filters and crosshairs.
- **Input:** RetroPad, keyboard, mouse and lightgun mappings for every Model 3
  control type, with dedicated layouts for driving, gun and Star Wars Trilogy
  controls (see [Controls](#controls)).
- **Force feedback:** drive-board force feedback is played on the controller's
  rumble motors.
- **Linked cabinets:** experimental Model 3 network play through RetroArch
  Netplay (see [Linked-cabinet networking](#linked-cabinet-networking)).

## Required assets

The core keeps its configuration in
`[RetroArch System Directory]/supermodel/Config/`:

- `Games.xml` identifies the ROM sets.
- `Supermodel.ini` holds Supermodel's own settings.
- `Music.xml` is optional and enables the corresponding music metadata.

Official copies of `Games.xml` and `Supermodel.ini` are built into the core and
extracted to that directory on first use when the files are missing. Existing
files are never overwritten, so they can be edited. The core options listed in
RetroArch take precedence over the matching `Supermodel.ini` settings.

ROMs are the standard MAME-style Model 3 ZIP sets named in `Games.xml`.

## Saves and NVRAM

NVRAM (high scores and the game's Service Menu settings) is saved by the
frontend as `<content>.srm`. To import a standalone Supermodel save, place
`<rom-set>.nv` directly in the frontend save directory. The core reads `.nv`
only when no `.srm` data was supplied. If both are present, `.srm` takes
precedence and the ignored `.nv` path is reported in the log. An invalid `.nv`
is ignored and replaced by default machine settings in a new `.srm`. The core
never writes or overwrites the native `.nv` file.

`Automatic Initial NVRAM Setup` affects only a newly created save. For games
that would otherwise stop at a network check or require cabinet hardware that
is not emulated (Daytona USA 2, Dirt Devils, Scud Race and Star Wars Trilogy
sets), it supplies the cabinet settings those games need to boot. Existing saves
are never modified; delete a game's `.srm` to regenerate them.

## Controls

The Service and Test buttons are on L3/R3 by default, or on the shoulder
buttons through `Service / Test Button Mapping`.

### Driving games

Steering is on the left analog stick and the pedals on the analog triggers;
L/R shift gears sequentially. `Driving Controls Layout` can add a four-gear
gate, a sequential lever or an H-pattern gearbox on the right stick. The
steering response and the steering, accelerator and brake output ranges can be
adjusted. The four view-change buttons (VR1-VR4) are on B, A, Y and X.

### Gun games

The Lost World, Ocean Hunter and L.A. Machineguns use one cursor per player.
`Gun Input Mode` selects its sources:

- **Hybrid (Lightgun / Mouse / Analog)** (default): RetroArch lightgun
  coordinates, relative mouse movement and the left analog stick all move the
  cursor; the last source moved takes priority.
- **Mouse + Analog Stick:** the same without the lightgun, so the mouse moves
  the cursor relatively instead of following the system pointer.
- **Lightgun**, **Mouse** or **Analog Stick Cursor:** only the selected source.

The left shot is the lightgun Trigger, the left mouse button or RetroPad B;
the right shot is the lightgun Reload or Aux A, the right mouse button or
RetroPad A. The lightgun's Start/Select and mouse buttons 4/5 also act as
Start and Coin.

### Star Wars Trilogy Arcade

The arcade yoke offers the same five modes in `Star Wars Trilogy Input Mode`.
Lightgun coordinates are absolute, mouse movement is relative, and the left
analog stick keeps the yoke's absolute, self-centring behaviour. The cabinet's
duplicated Trigger 2 / Event 2 pair comes from a second mouse or lightgun on
port 2. The core uses port 2 only after it has proved to be a separate device,
because some frontends report the same pointer on every port. Mouse buttons
4/5 and the lightgun's Start/Select act as Start and Coin. The Upright
cabinet's reversed horizontal axis is applied automatically from the cabinet
type stored in NVRAM.

### Multiple mice and lightguns

Players 1 and 2 are independent, but two separate mice or lightguns need a
frontend input driver that reports multiple pointing devices (for example
`udev` on Linux or `raw` on Windows).

## Linked-cabinet networking

`Network Board` controls whether supported games see the emulated network
board. Experimental linked play through RetroArch Netplay is available for
Daytona USA 2, Dirt Devils, Harley-Davidson, Le Mans 24, Scud Race, Sega Rally 2,
Ski Champ, Spikeout, Spikeout Final Edition and Virtual On 2. During Netplay the
core uses only the official Libretro Netpacket interface; it does not open its
own sockets or perform peer discovery.

RetroArch gives participant ID 0 to the host, which the core uses as the first
Model 3 cabinet and network initiator. Therefore:

- start the RetroArch Netplay host before its clients;
- set the host's cabinet to `Master` in the game's Service Menu;
- never set a client's cabinet to `Master`;
- set every Type 1 client to `Slave`; Type 2 clients may also use a
  `Slave / Satellite` role;
- give every cabinet its own car or machine number.

Every peer must use the same ROM set, core version and `Linked Cabinets` value.
Two- and three-cabinet sessions have been tested; larger sessions are
experimental. Changing the network options requires restarting the content.

## Performance

For demanding titles (e.g. Sega Rally 2, Daytona USA 2, Le Mans 24) use a
release build. The core uses synchronous audio, so if the host cannot sustain
the full 57.53 Hz the sound will stutter. On weaker hardware these options help
most, roughly in order:

- `PowerPC CPU Frequency`: underclocking is the largest single saving, at the
  cost of cycle accuracy;
- `Emulation Threading`: keep the default `Multi-threaded + GPU`;
- `JIT Recompiler (ARM64)`: keep it enabled on ARM64;
- `Internal Resolution`: Half or Native;
- `Frame Skip`.

`Frame Timing Overlay` shows per-frame PowerPC, render, GPU and total timings
for profiling.

## Build instructions

The core is built with the single `Makefile` in the repository root. When no
`platform` is given, it is detected from the host: `unix` on x86 Linux,
`linux-aarch64` on ARM64 Linux, `osx` on macOS and `win` on MSYS2.

```bash
make -j$(nproc)                    # build for the host
make DEBUG=1 -j$(nproc)            # debug build
make clean
```

| Platform | Command | Output |
|---|---|---|
| Linux x86_64 | `make` | `supermodel_libretro.so` |
| ARM64 Linux, native (e.g. on a Raspberry Pi) | `make` (`platform=linux-aarch64`) | `supermodel_libretro.so` |
| Raspberry Pi, cross-compiled | `make platform=rpi64 board=rpi5` (or `board=rpi4`) | `supermodel_libretro_aarch64.so` |
| Generic ARM64 Linux, cross-compiled | `make platform=aarch64` | `supermodel_libretro.so` |
| Windows (MinGW) | `make platform=win` | `supermodel_libretro.dll` |
| macOS | `make platform=osx` | `supermodel_libretro.dylib` |
| Android | `make platform=android [arch=arm64\|arm\|x86_64\|x86]` | `supermodel_libretro_android.so` |
| iOS / tvOS | `make platform=ios-arm64` / `platform=tvos-arm64` | `supermodel_libretro_ios.dylib` / `_tvos.dylib` |

### Linux

```bash
sudo apt install build-essential libgl1-mesa-dev libglu1-mesa-dev zlib1g-dev
make -j$(nproc)
cp supermodel_libretro.so ~/.config/retroarch/cores/
```

### Raspberry Pi and other ARM64 Linux

Building on the device itself needs no options: the Makefile selects
`linux-aarch64` with OpenGL ES 3 and the ARM64 JIT.

To cross-compile from an x86_64 Linux machine:

```bash
sudo apt install g++-aarch64-linux-gnu zlib1g-dev:arm64
make platform=rpi64 board=rpi5 -j$(nproc)    # Cortex-A76
make platform=rpi64 board=rpi4 -j$(nproc)    # Cortex-A72
make platform=rpi64 -j$(nproc)               # Cortex-A53 (default)
make platform=aarch64 -j$(nproc)             # generic ARM64, no CPU tuning
```

Copy the result to the device's RetroArch `cores` directory as
`supermodel_libretro.so`.

#### Legacy3D on GLES

On GLES platforms New3D is the only renderer by default. Legacy3D can be built
in as the fixed renderer with `RENDERER=legacy` (for example
`make platform=rpi64 board=rpi5 RENDERER=legacy`); on a Raspberry Pi 5 it can
give steadier frame times than New3D. macOS, Android, iOS and tvOS do not
support `RENDERER=legacy`.

### Windows

Natively with [MSYS2](https://www.msys2.org/), in the "MSYS2 MinGW 64-bit"
terminal:

```bash
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-make mingw-w64-x86_64-zlib
make platform=win -j$(nproc)
```

Or cross-compiled from Linux:

```bash
sudo apt install binutils-mingw-w64-x86-64 g++-mingw-w64-x86-64 libz-mingw-w64-dev
make platform=win -j$(nproc)
```

Copy `supermodel_libretro.dll` to RetroArch's `cores` directory.

### macOS

On a Mac with the Xcode command line tools, `make platform=osx` builds for the
host architecture. Apple Silicon builds include the ARM64 JIT.

From Linux, [osxcross](https://github.com/tpoechtrager/osxcross) (expected in
`/opt/osxcross`, or set `OSXCROSS_ROOT`) builds a universal x86_64 + arm64
binary that uses the interpreter:

```bash
make platform=osx -j$(nproc)
make platform=osx OSXCROSS_ROOT=/custom/path -j$(nproc)
```

The minimum macOS version is 10.15. Legacy3D is not included. Copy
`supermodel_libretro.dylib` to `~/Library/Application Support/RetroArch/cores/`.

### Android

Requires the Android NDK, found at `~/Android/Sdk/ndk/28.2.13676358` or through
`NDK_ROOT`. The default architecture is arm64, which includes the JIT; the
target API level is 24.

```bash
make platform=android -j$(nproc)                 # arm64
make platform=android arch=arm -j$(nproc)        # armv7-a with NEON
make platform=android arch=x86_64 -j$(nproc)
NDK_ROOT=/path/to/ndk make platform=android -j$(nproc)
```

The core can also be built with `ndk-build` from the `jni/` directory. Install
the resulting `.so` in RetroArch with `Load Core > Install or Restore a Core`.

### iOS and tvOS

Built on a Mac with Xcode. These builds use OpenGL ES 3 and the interpreter:
the JIT needs an entitlement that a Libretro core cannot request on stock iOS
or tvOS.
