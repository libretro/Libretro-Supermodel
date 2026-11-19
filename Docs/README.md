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
- **Timing:** `Model 3 Timing Mode` offers 60 Hz, the default of standalone
  Supermodel, which runs smoothly with correct audio pitch on ordinary 60 Hz and
  120 Hz displays, or the real Model 3 rate of 57.524 Hz for displays that can
  follow it (VRR). Audio is always exactly 44.1 kHz stereo, delivered
  synchronously through `audio_batch_cb`.
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

Like other Libretro cores, the core keeps its files in its own folder in the
RetroArch system directory, `[RetroArch System Directory]/supermodel/`:

- `Games.xml` identifies the ROM sets.
- `Supermodel.ini` holds Supermodel's own settings.
- `Music.xml` is optional and enables the corresponding music metadata.

Official copies of `Games.xml` and `Supermodel.ini` are built into the core and
extracted to that folder on first use when the files are missing. Existing
files are never overwritten, so they can be edited; after a core update, delete
or replace `Games.xml` by hand to pick up its newer definitions. If the folder is not
writable, the built-in copies are used directly. Files in the older
`supermodel/Config/` folder are still found. The core options listed in
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

`Automatic Initial NVRAM Setup` affects only a newly created save. For the
supported ROM sets (Daytona USA 2, Dirt Devils, Harley-Davidson, LA
Machineguns, Le Mans 24, The Lost World, The Ocean Hunter, Scud Race, Sega
Rally 2, Ski Champ, Spikeout, Star Wars Trilogy and Virtual On 2) it starts
from that set's factory settings, selects the single-cabinet link mode and
sets Star Wars Trilogy to the Upright cabinet, so the game boots without a
network check or cabinet hardware that is not emulated. For the gun games it
also stores a gun calibration spanning the whole screen, which is the range
the core's cursor covers, so the sight follows the pointer without a Service
Menu calibration. Existing saves are never modified; delete a game's `.srm`
to regenerate them.

`NVRAM Settings` (off by default) adds the game's own Service Menu settings as
core options: Country, Difficulty, Link Mode, Car Number, Cabinet Type and a
few game-specific ones such as Le Mans 24's Special Car or the Daytona USA 2
vocal; the gun games get `Provide Calibration Data`, which rewrites the
whole-screen gun calibration. The options appear only for the loaded game.
While the switch is on, the displayed values are written into NVRAM at every
content start, so they override changes made in the Service Menu; turn it off
to let the Service Menu rule again. Changing one of these options while a game
runs writes it at once and resets the machine, as RetroArch's Restart would. Supported: Bass Fishing, Daytona USA 2,
Dirt Devils, ECA, Fighting Vipers 2, Harley-Davidson, LA Machineguns, Le Mans
24, The Lost World, Magical Truck Adventure, The Ocean Hunter, Scud Race, Ski
Champ, Spikeout, Sega Rally 2, Star Wars Trilogy, Virtua Fighter 3, Virtual
On 2 and Virtua Striker 2 (all versions).

## Video and audio options

`Widescreen Hack` expands the 3D camera's horizontal field of view into a 16:9
frame (683x384 at native resolution) instead of stretching the 4:3 image. Only
full-screen 3D views are widened and the 2D layers keep their geometry; some
games show glitches at the sides. `Widescreen + Wide Background`
also stretches the lower 2D background layer to fill the sides; HUD and upper
overlays keep their aspect ratio. Plain 4:3-to-16:9 stretching is left to
RetroArch's aspect-ratio settings. The change applies immediately; the wider
frame has about 38% more pixels, so judge performance separately.

`Aspect Ratio` is Auto by default: 16:9 while the Widescreen Hack is on, 4:3
otherwise. 4:3 keeps the original image whatever the hack says; 16:9 with the
hack off stretches the 4:3 image to fill a wide screen. It applies immediately,
provided RetroArch's aspect ratio setting is Core Provided.

`Quad Rendering` (desktop OpenGL only) renders the Model 3's quadrilaterals as
quads through a geometry shader instead of splitting them into two triangles,
which removes the texture-warping seams some games show on large polygons. It is
off by default, takes effect after restarting the content, is ignored by
Legacy3D and can be noticeably slower, in particular on macOS.

`2D Layer Upscaling Filter` (Nearest, Biquintic, Bilinear or Bicubic) is
Supermodel's own filter for the 2D tile layers at internal resolutions above
native; at 496x384 Nearest is always used. `CRT Colour` applies Supermodel's
colour and gamma adaptation for the original arcade monitors (D93 and D65
profiles) and is ignored by Legacy3D. `SCSP DSP Engine` selects the
MAME-derived sound DSP (default) or the legacy ElSemi engine for games with
audio problems on the new one, such as Fighting Vipers 2. These three take
effect after restarting the content.

## Controls

Test is on L3 and Service on R3 (player 2: its own L3/R3), or Test on L and
Service on R through `Service / Test Button Mapping`.

### Driving games

Steering is on the left analog stick and the pedals on the analog triggers;
L/R shift gears sequentially. By default the right stick is also an H-pattern
gearbox (up-left, down-left, up-right and down-right select gears 1-4) and Y
selects neutral; `Driving Controls Layout` can instead use the right stick as
a four-gear gate or a sequential lever, or leave it unused. The view buttons
are on the D-Pad (VR1 red, VR2 blue, VR3 yellow and VR4 green on up, down,
left and right; single view change on up). The handbrake (Sega Rally 2) and
the rear brake (Harley-Davidson) are on B; Harley-Davidson's music select is
on A. The steering response (linear or progressive) and the steering,
accelerator and brake output ranges can be adjusted; these options apply only
to driving games. The 63% steering and 75.3% pedal presets map the full stick
and trigger travel to the 30-80-D0 and 00-C0 ranges of the original controls.

### Other cabinets

- **Virtual On 2:** each analog stick drives one of the cabinet's two sticks;
  L2/R2 fire and L/R are the turbo buttons.
- **Sega Bass Fishing / Get Bass:** the left stick is the rod, the right stick
  the rod's stick, R2 reels and L2 sets the line tension (Get Bass); B or Start
  casts.
- **Ski Champ:** the left stick moves the skis, L/R are the poles, and Y, B
  (or Start) and A are the three select buttons.
- **Magical Truck Adventure:** each player's lever is the vertical axis of
  their own pad's left stick and the pedal is B.
- **Fighting games:** the D-Pad and the left stick both move; B, A, Y and X
  are Punch, Kick, Guard and Escape (Spikeout: Shift, Beat, Charge and Jump;
  Virtua Striker: Short Pass, Long Pass and Shoot on B, A and Y).

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
RetroPad A. R and L double B and A when the shoulders are not Test/Service.
The lightgun's Start/Select and mouse buttons 4/5 also act as Start and Coin.

The Lost World has one trigger and reloads with a shot off-screen, which a
real lightgun does by itself (trigger while pointing away). With
`The Lost World: Off-Screen Reload Shortcut` (on by default) the lightgun
Reload input, the right mouse button and RetroPad A (or L) also fire an
off-screen shot without aiming away; `The Lost World: Mouse Edge Reload`
treats a mouse shot within 5% of the screen edge the same way.

`Show Crosshairs` is Automatic by default: player 1's crosshair in The Lost
World, whose gun needs an aim marker, and none in L.A. Machineguns and The
Ocean Hunter, which draw their own. The other values force a fixed choice.

### Star Wars Trilogy Arcade

The arcade yoke offers the same five modes in `Star Wars Trilogy Input Mode`.
Lightgun coordinates are absolute, mouse movement is relative, and the left
analog stick keeps the yoke's absolute, self-centring behaviour. The cabinet's
duplicated Trigger 2 / Event 2 pair comes from a second mouse or lightgun on
port 2. The core uses port 2 only after it has proved to be a separate device,
because some frontends report the same pointer on every port. Mouse buttons
4/5 and the lightgun's Start/Select act as Start and Coin. The Upright
cabinet's reversed horizontal axis is applied automatically from the cabinet
type stored in NVRAM; `Star Wars Trilogy Upright X-Axis Inversion` turns that
off when the frontend or the device already inverts the axis.

### Multiple mice and lightguns

Players 1 and 2 are independent, but two separate mice or lightguns need a
frontend input driver that reports multiple pointing devices (for example
`udev` on Linux or `raw` on Windows).

## Linked-cabinet networking

Experimental linked play through RetroArch Netplay is available for Daytona
USA 2, Dirt Devils, Harley-Davidson, Le Mans 24, Scud Race, Sega Rally 2, Ski
Champ, Spikeout, Spikeout Final Edition and Virtual On 2. During Netplay the
core uses only the official Libretro Netpacket interface; it does not open its
own sockets or perform peer discovery.

Two options appear for these games. `Linked Cabinets` is the total number of
cabinets in the session, including a relay or satellite where the game has
one; 1 Cabinet / Disabled (the default) disconnects the emulated network board.
`Auto Network Role Settings` writes this cabinet's link role and number into
the game's NVRAM, so no Service Menu visit is needed: Master for the RetroArch
host, Slave with its own number for each client, or a Live, Relay or Satellite
role where the game has one; Disabled leaves the NVRAM alone. Changing either
option resets the machine. Both show Not Supported for other games.

| Game | Cabinets |
| --- | --- |
| Daytona USA 2 | 2 to 17 |
| Scud Race | 2 to 8 |
| Dirt Devils, Le Mans 24, Sega Rally 2, Virtual On 2 | 2 to 5 |
| Harley-Davidson, Ski Champ, Spikeout | 2 to 4 |

RetroArch gives participant ID 0 to the host, which the core uses as the first
Model 3 cabinet and network initiator. Therefore:

- start the RetroArch Netplay host before its clients;
- set the host's cabinet to `Master`;
- never set a client's cabinet to `Master`;
- set every Type 1 client to `Slave`; Type 2 clients may also use a
  `Slave / Satellite` role;
- give every cabinet its own car or machine number.

The link role and number can also be set in the game's Service Menu or, with
`NVRAM Settings` enabled, through the game's `Link Mode` and `Car Number`
options, which are applied at every start.

Every peer must use the same ROM set, core version and `Linked Cabinets` value.
Two- and three-cabinet sessions have been tested; larger sessions are
experimental.

## Performance

For demanding titles (e.g. Sega Rally 2, Daytona USA 2, Le Mans 24) use a
release build. The core uses synchronous audio, so if the host cannot sustain
full speed the sound will stutter. On weaker hardware these options help
most, roughly in order:

- `PowerPC CPU Frequency`: underclocking is the largest single saving, at the
  cost of cycle accuracy;
- `Emulation Threading`: keep the default `Multi-threaded + GPU`;
- `JIT Recompiler (ARM64)`: keep it enabled on ARM64;
- `Internal Resolution`: Half or Native;
- `Frame Skip`.

`Frame Timing Overlay` shows the per-frame PowerPC, render, GPU, sync, sound
and total times, colour-coded against the frame budget, plus 61-frame averages
of the engine, audio pacing, blit and present times, the total and worst
`retro_run` time, the actual frontend frame rate and the rates the engine
alone and the whole callback could sustain.

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

## Smoke test

`Scripts/libretro_smoke_test.py` boots every set in `Games.xml` (or the sets
given with `--game`) in a throwaway RetroArch profile, runs a fixed number of
frames and keeps the final frame, the RetroArch log, the generated `.srm` and
a CSV/JSON summary per set. It needs Python 3.9 or later, RetroArch in `PATH`
(or `--retroarch`) and a directory of `<set>.zip` ROMs. Sets whose ROM is
missing are listed as such; crashes, non-zero exits and wall-clock timeouts
fail the run.

```bash
python3 Scripts/libretro_smoke_test.py --rom-dir /path/to/roms --list   # sets and ROM availability
python3 Scripts/libretro_smoke_test.py --rom-dir /path/to/roms          # all sets, 60 emulated seconds each
python3 Scripts/libretro_smoke_test.py --rom-dir /path/to/roms --game scud --seconds 10 --output /tmp/smoke
```

It uses the repository's build and `Config/` files by default; `--core`,
`--base-config` (a RetroArch config to start from, e.g. for the video driver)
and `--no-fast-forward` (run in real time) change that. Results go to
`~/Documents/RetroArch/supermodel-smoke/<timestamp>` unless `--output` is
given.
