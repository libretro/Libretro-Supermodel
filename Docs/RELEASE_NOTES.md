# Supermodel Libretro v0.3a-20261012-git-ed608de-libretro.1

This port release supersedes 0.3.0-beta. The version identifies the reviewed
2026-10-12 upstream snapshot and the first port revision against that snapshot.
Upstream integration is selective; it is not a full merge of that snapshot or
a claim to include the latest standalone Supermodel.

## Changes

- Add Aspect Ratio Auto / 4:3 / 16:9. Widescreen Mode and Aspect Ratio apply on
  return from the menu, without resetting the machine or reloading content.
  Explicit 4:3 suppresses expansion; Disabled + 16:9 stretches the original image.
- Preserve 3D textures and rendering registers when live changes recreate the
  renderer; clear stale viewport margins between content loads.
- Include the previously delivered networking, NVRAM, controller/menu and
  audio-reset improvements.
- Adapt native Quad Rendering to OpenGL 4.1/macOS and include the corrected
  vs298 ROM catalogue entry. The corrected dump CRC is `969E4BDA`; old dump
  `C3BBB270` is rejected. Standalone GUI changes remain excluded.

## Packages

Linux x86_64, Windows x86_64, macOS ARM64 and macOS x86_64 packages contain the
core, matching .info, Games.xml, Music.xml, licenses, source provenance and
SHA-256 checksums. ROMs are not included. Copy support files into the configured
RetroArch System directory under `supermodel/`. macOS binaries are architecture
specific; see [installation instructions](README.md).

## Validation And Limits

Local macOS checks cover 256 registered options, live presentation transitions
and 3D pixel comparisons on Daytona 2 and Harley-Davidson, including Daytona 2
with 2x resolution and Multi-threaded + GPU. Platform packages are generated and
validated by the four-platform CI matrix. Build/package checks do not establish
gameplay acceptance on every operating system.

Quad Rendering remains optional, disabled by default, and may be slow on macOS.
No verified native Model 3 widescreen mode is claimed. Corrected-vs298-dump
gameplay remains unqualified. Upstream GUI changes and other exclusions are
recorded in [the integration ledger](UPSTREAM_INTEGRATION.md).
