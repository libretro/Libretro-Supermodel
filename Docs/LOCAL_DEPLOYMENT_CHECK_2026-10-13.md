# Configured Local Deployment Check

Date: 2026-10-13  
Framework Contract: F20 — Installation And Delivery Evidence

## Current Naming Policy — User Follow-Up

The user restored `supermodel_libretro.dylib` and `supermodel_libretro.info` as
the only current local deployment target. The verified build replaces the
previous standard core; the dev-named binary/info are removed, old targets are
backed up, and references to the removed filename are migrated. Do not recreate
the dev copy without a new explicit request. The earlier deployment below is
historical; the current catalogue follow-up is recorded at the end.

The active RetroArch configuration is
`~/Library/Application Support/RetroArch/config/retroarch.cfg`.
Its cores/info paths resolve to `~/Library/Application Support/RetroArch/cores`
and `~/Library/Application Support/RetroArch/info`. The currently authorized
development target is `supermodel_dev_libretro.dylib`, with its matching .info;
the standard `supermodel_libretro.dylib` was not updated.

The installed development binary matches the final checked macOS ARM build:
`17cdde5e287691f8fae90981bdda1b5ecebccab77ec0f1b5137bbcd1af3fe74c`.

## Support Asset Correction

The initial support-file update used `~/Documents/RetroArch/system/supermodel`,
but the active configuration uses
`~/Retro/_Media/_Multi (RetroArch)/system/supermodel`, resolving to the external
drive. The isolated frontend probes copied their own assets, so they did not
establish that the normal frontend's support assets had been updated.

The configured external Games.xml is now corrected to CRC `969E4BDA` for
`epr-20917.17`, with a backup. All ROM names/CRCs match the source catalogue;
the existing informational copyright and Fighting Vipers 2 Japan metadata
are retained. This is semantic ROM-catalogue agreement, not byte-identical XML.
Music.xml already matches the source and required no write.

Verified configured Games.xml SHA-256:
`53499adc7463d59f24bcf9ad185c91196aa6dcdfb193c43365a6e922a6c9d7c6`.
Music.xml SHA-256:
`cf7555ca86e8640ee5bbb8e0c0897b073fc86647a93eda10b05065227610478c`.

## Reusable Lesson

Before declaring local deployment complete, read the active frontend's actual
cores/info/system paths and resolve symlinks. Check the binary and configured
support files separately. A successful isolated run or a matching internal
copy is not proof of deployment to the normal configured frontend. Preserve
user metadata/overrides and back up corrections; do not infer that a standard
core should be replaced from another project's deployment policy.

## Catalogue Follow-Up — 2026-10-13

After user approval, `mgtrkbad` was removed from source Games.xml, the generated
DAT, the embedded fallback and the active external System Games.xml. The
remaining 62 sets match semantically; configured copyright/title metadata is
preserved. The standard local core/info are updated after successful macOS
build and ABI/dependency checks, with backups in
`outputs/catalogue-update-20261013/backups/`.

Current installed core SHA-256:
`983e03c1de796f4a9bcad6d0b1cb8c870f86db151acbd387915e38d3806e307e`.
Current configured Games.xml SHA-256:
`ef2a05d9bebbd3bd10084ceddac15dc36e3b720aaaf09002faed65e8ba53bd1d`.

Active System directory:
`/Volumes/MacBook External Drive/Retro/_Media/_Multi (RetroArch)/system`.
The original ROM download directory is separately
`/Volumes/MacBook External Drive/Retro/_Media/_Multi (MAME)/roms`.
Both it and the Model 3 working copy still contain the old vs298 component;
39/40 ROMs match the updated catalogue. The ROM archives are unchanged.

## VR1 Up/Down Follow-Up — 2026-10-13

The user-approved single-View convention now binds D-Pad Up and Down to the
same native View action across the fifteen applicable sets, including srally2.
RetroArch descriptors expose VR1 for both directions; VR4 and existing remaps
remain unchanged. The standard macOS core/info are updated with backups after
the normal build and 25-symbol ABI/dependency gate pass. Current installed
binary SHA-256: `809ff044c57a86dbbba97154c64e19b3c0b7944ef27914cc0e88ce14fe9ab527`.
The external System catalogue is unchanged. Evidence and backups are under
`outputs/vr1-up-down-20261013/`. Framework F09 is corrected in edition 0.1.10
with provenance M15. No new physical-controller/gameplay check, commit or push
is claimed; an already-loaded frontend must reload the core to use this build.

## Framework Menu Wording Follow-Up — 2026-10-13

The revised menu wording is locally delivered under the standard core/info
target after build, ABI and Core Options API 0/1/2 checks. The current installed
core SHA-256 is `cdd54d18e411aa8532d190935762489914cecc2ccf10b244dd82a756e2f1ffda`.
Backups and verification are in `outputs/menu-wording-20261013/`.
The actual Video/Input/Audio menus were inspected in a separate owned QA
instance, which exited normally. The external System catalogue is unchanged.
Scope and the original factual text exceptions are recorded in ROADMAP.md under
User-Revised Framework Menu Wording. An already-loaded core needs reloading to
show the new wording. No commit or push.

## Common Gamepad Rumble Comment Follow-Up — 2026-10-13

The approved Menu Review!F98 comment now reads exactly:
"Enable gamepad vibration using the game's force-feedback or rumble effects."
The previous Rumble exception is superseded; Sound Volume and Internal
Resolution exceptions remain. The incremental macOS build, 25-symbol ABI gate
and API 0/1/2 registration comparison pass. Only one source help string changed;
all option choices, labels, defaults and behavior are preserved. No additional
frontend/gameplay qualification is claimed. The standard installed core matches
the build with SHA-256
`2aaf9f5887e037448e98dd448d36f312fbc9a72d11098ddc1a8f7bc63b054c54`.
Evidence and prior-core backup: `outputs/menu-wording-20261013/rumble-followup/`.
Reload the core to see the new comment. No commit or push.

## Network Integration Follow-Up — 2026-10-14

The standard installed core now implements local loopback, offline ANS precedence,
board-dependent menu visibility and automatic hardware reconfiguration/reset on
Resume. Build/ABI/registration and actual frontend lifecycle checks pass. SHA-256:
`13d9362be2d944f22f09cd0b8755a57c8bb518d6d090aae2261ff00628acdc8c`. Evidence and backups: `outputs/network-integration-20261014/`.
See NETWORK_INTEGRATION_2026-10-14.md for scope and limits. No commit or push.

## Audio Reset Recovery Follow-Up — 2026-10-18

The standard installed core now shares the startup and reset audio-queue
priming. Old/new actual PCM comparison, real frontend recordings and user
confirmation of correct Multi-threaded + GPU live playback before/after reset
pass. The existing Daytona 2 native/reset/lobby ABI regression also passes.

Installed/build SHA-256: `5cca917dc93bcc9ea9b65377d5f87658533038c14c1bee20506b4e04442c7862`.
Matching info, active RetroArch configuration and external System assets were
verified unchanged. Both original user sessions were closed normally with user
authorization, and QA instances exited 0; the next launch uses the corrected
core. No frontend-driver or configuration change, commit or push. Evidence:
`outputs/audio-reset-20261018/{verification,deployment}.json`.


## Widescreen Value Labels — 2026-10-18

The standard installed core now shows Stretch 3D and 2D View, Expand 3D View
Only and Expand 3D View + Stretch 2D Background. The help explains the frontend
16:9 requirement for full-image stretch and the lower-background-only expansion.
Saved values, default and renderer behavior are unchanged. Build, 25-export/
dependency checks and API 0/1/2 comparisons pass; this is registration evidence,
not a repeated frontend/gameplay check.

Installed/build SHA-256:
`48f9a27935eda40ab0ae42addfd897ad87ca2a3c4a5c5989137830932d9d874a`.
Active paths were reread/resolved and the .info, frontend configuration and
external System files verified unchanged. No binary backup was retained; scoped
header/rebuild rollback applies. Reload the core to display the new labels.
Evidence: `outputs/widescreen-labels-20261018/`. No commit or push.


## Direct Widescreen Convention — User Clarification

The first value is again labelled Disabled. Expand modes directly report 16:9;
Disabled reports 4:3 and still permits frontend-forced full-image stretching.
This supersedes the prior Stretch label. The new help documents that behavior
and preserves the lower-background versus upper-layer distinction.

Build, 25-export/dependency gate and API 0/1/2 scope checks pass. Installed/build
SHA-256: `f7254edfd9e86924e4558636cdf457ba1082f4ed99a5e39763926e7a127ab9b9`.
Active configured paths were resolved; matching info, config and System files
remain unchanged. Evidence: `outputs/widescreen-direct-20261018/`. No commit/push.


## Core Aspect Ratio Selector And Gate — 2026-10-18

The user approved Auto/4:3/16:9, Auto default. Explicit 4:3 now suppresses
expansion; Auto/16:9 permit the selected Expand mode. Disabled + 16:9 stretches
the complete image. Aspect-only geometry changes and live renderer resizing
reuse the existing paused-worker path without a machine reset.

Build, 25-export/dependencies, 256 API 0/1/2 definitions, 12 live CGL transitions
and three explicit Restarts pass. Three actual RetroArch glcore image-delivery
cases confirm the combinations. Installed/build SHA-256:
`fcf6ea2d0049a8d8dcf5a1902e8e16313900965a358c98b5cc0990510a18fe53`.
Matching info, active config and System assets were preserved. Evidence:
`outputs/widescreen-aspect-gate-20261018/`. No commit/push.


## Widescreen Mode On Resume — 2026-10-18

Installed the verified public `supermodel_libretro.dylib` after the live-selector
follow-up. Build/install SHA-256: `112b326cd6770409619c6058b47c02c1cd398bc8a0e05112ad910c8d27fd8af5`. Matching info, active
configuration/System files and TGPulse core preserved. Reused the existing
atomic deployment helper; no routine binary backup, commit or push. Source,
20 CGL transitions and actual Daytona 2 RetroArch UI evidence are under
`outputs/widescreen-live-selector-20261018/`.


## Live Presentation Renderer-State Fix — 2026-10-18

Installed the verified public core after the reproduced WM/AR texture loss
correction. Build/install SHA-256: `1c333eb0d3ab0bd5b584e2069484147cceb4dcef99574e77d5428d37104707f1`. Matching info, active
configuration, System XML assets and TGPulse core preserved. No routine binary
backup or commit/push. Evidence: `outputs/widescreen-renderer-state-20261018/`.
Already-running instances retain their previously loaded binary until the
content/core is reopened once; subsequent WM/AR changes remain live on Resume.


## Published CI Artifact Deployment — 2026-10-18

Local RetroArch core and .info were replaced atomically from the exact macOS
ARM artifact of CI 39164917017 / source `57b07016`, published as
`v0.3a-20261012-git-ed608de-libretro.1`. Installed/core artifact SHA-256:
`b150417731ae694844b7b694c38f8ede50797c54b5844b7e473009d00fc5208f`. Twenty CGL transitions pass on the
installed artifact. Active frontend config and external System XML hashes are
preserved. Evidence: `outputs/release-20261018/deployment.json` and
`geometry-abi.json`. No routine binary backup.
