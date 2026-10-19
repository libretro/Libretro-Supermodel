# Roadmap

Updated: 2026-10-19

Reference: [Libretro Core Framework 0.1.37](/Users/andrea/dev/libretro-core-framework/FRAMEWORK.md).

This is the sole current implementation roadmap. Completed means the stated
scope and evidence are complete; it does not imply all-platform gameplay or a
published release. The user authorized the accumulated U01–U05 source/documentation commit and push
on 2026-10-19. The project README is updated with this source publication.
No new tag or release was requested. Generated local evidence and
build artifacts remain outside the source publication.

Legend: 🟢 Completed · 🟡 Partial · ⚪ Not Started · 🔵 Deferred

| Status | Activity | Remaining Work |
| --- | --- | --- |
| 🟢 | F04 — Frontend Option Ordering | Implemented and installed locally. Profile/Rumble precede specialized Input groups; Star Wars controls are last. Approved Video follow-up groups Rendering, Image, then Timing/Diagnostics. Original whole-menu UI acceptance retained; its follow-up API 0/1/2 and four header configurations pass. The later Aspect Ratio selector is adjacent to Widescreen Mode. System/native order retained. |
| 🟢 | Widescreen And Aspect Ratio | User-approved Disabled / Expand 3D View Only / Expand 3D View + Stretch 2D Background labels and direct-16:9 help implemented and installed locally. WM values/default retained. Added Auto/4:3/16:9 Aspect Ratio, Auto default; 4:3 suppresses expansion, Disabled + 16:9 stretches. Both selectors now apply on Resume without reset/reload, reusing the existing resize path. API 0/1/2, build, CGL transitions and three RetroArch cases pass. Published in the first snapshot-based port release; see Current Local Delivery. |
| 🟢 | F04 — Final Registered Menu Wording Audit | The original 255 definitions and 36 applicable workbook rows pass; the later Aspect Ratio follow-up verifies 256 definitions with the original metadata preserved except revised WM help; full per-key metadata preserved. V2/V1/V0 order, 22 network pairs, exact help and four conditional header configurations checked. |
| 🟢 | F14 — Two-Option Networking | Native authority, handover and retained transport are implemented; initial real two-peer exchange is verified. User-approved fallback retains immediate effective-change application during non-pausing Netplay menus. See the 2026-10-18 feasibility follow-up for the explicit timing limit. |
| 🟢 | F05/F12 — Effective NVRAM Reset | Paused menus coalesce effective edits into one reset on Resume; unchanged/alias-equivalent settings do not reset. Explicit Restart consumes pending choices. During non-pausing Netplay, effective edits apply immediately; the user accepted retaining this behavior if reliable menu-exit deferral is unavailable. |
| 🟢 | Audio Recovery After Machine Reset | Startup and reset share the primed audio-queue initialization. Old/new PCM regression, real RetroArch recording, matching Multi-threaded + GPU playback before/after reset confirmed by the user, and existing Daytona 2 network lifecycle regression pass. Standard local core installed; no driver or user-configuration change. |
| 🟢 | F09 — Global Controller Profile Type | Full default and Normal mode implemented; paused metadata, both ports, remapping/None and legacy behavior checked. |
| 🟢 | F09 — Service/Test Order And VR1 | L3 Service / R3 Test retained. Single-View profiles bind D-Pad Up and Down to Change View where neither conflicts; native/keyboard bindings and VR4 distinctions retained. |
| 🟢 | F10 — Lightgun Standard And Reload | Combined-source arbitration and The Lost World reload behavior implemented; constant off-screen status alone does not take Lightgun ownership. Existing dedicated modes retained. |
| 🟢 | F21 — RetroArch Notifications | Extended notifications and fallback paths verified; actual Resume/reset OSD inspected. |
| 🟢 | Gamepad Rumble Lifecycle And Effect Mixing | Common adapter preserves independent effects, ports and axes; mute/re-enable retains current native state without reset or reload. Native board emulation is independent of gamepad output. Scud runtime regression and direct adapter checks documented in `GAMEPAD_RUMBLE_2026-10-18.md`. No synthetic cornering threshold is applied. |
| 🟢 | F04/F05/F07 — Reviewed Names And Help | Canonical wording applied with documented Sound Volume and Internal Resolution exceptions. Values/defaults and native operator labels retained. |
| 🟢 | F06 — NVRAM Master Visibility | Per-set reviewed-field gating and saved selections retained; managed network fields use ANRS authority while unrelated fields keep normal management. |
| 🟢 | F13 — Supplemental Release-Clone NVRAM Campaign | 478/478 acquired values across 35 release clones validate. Existing valid samples are retained; no regeneration required by menu ordering. |
| 🟢 | F13 — Scud Australia Correction And Acquisition | Eight settings and thirty distinct values inventoried. Seventeen native-committed/fresh-reloaded samples now cover Link ID, Car Number, Difficulty and Cabinet Type. Six supplemental samples close the previously omitted clone overrides; native defaults Normal/Twin and existing initial seed verified. |
| 🟢 | F13 — Common Campaign Workbook | Common four-sheet summary adopted, with Framework 0.1.33 core-specific verification columns: 605 native setting rows, 62 sets, all 179 implemented controls and 50 actual EEPROM seeds reconciled with source. Four obsolete Country exposure flags corrected. Historical records preserved in notes; five replaced summary files removed. |
| 🟢 | Selected Upstream Quad/Catalogue Integration | OpenGL 4.1/macOS Quad adaptation and corrected vs298 catalogue CRC integrated locally. Reviewed upstream snapshot and exclusions remain in the integration ledger; no fresh upstream review is implied. |
| 🟢 | Quad Rendering Performance Investigation | Closed by the user. The Mac geometry-stage/draw-path penalty was isolated; tested workarounds did not produce a safe production correction. |
| 🟢 | Existing Multi-Platform CI And Release Artifacts | Commit 4566c94 / release 0.3.0-beta already passed Linux x86_64, Windows x86_64 and both macOS jobs, including format/API/dependencies and artifact assembly. Reconfirmed from GitHub run 36183994220 on 2026-10-18. |
| 🟢 | F18/F19 — New Package Checks | Source `c12cd392` passed all four desktop CI jobs, including runtime identity, inventory, XML, notices, provenance, checksums and artifact upload. Linux host-GL and Windows Git prerequisites corrected. [CI evidence](https://github.com/Zer0one/Libretro-Supermodel/actions/runs/39134741470). |
| 🟢 | Libretro Upstream Submission | [PR #20](https://github.com/libretro/Libretro-Supermodel/pull/20) published from existing `main`, including the Scud Australia and rumble fixes in `36a105e2`. Desktop CI run 39173389437 started for that implementation commit. New standalone changes remain pending below; no new branch or native upstream import. |
| 🟢 | Source Publication And GitHub Page | Accumulated source/docs committed and pushed; README and repository description updated. Current release source is published on `main`; `supermodel-modern` remains the repository default and historical branches are retained. The new four-platform prerelease is published; see Current Local Delivery. |
| 🟢 | U01 — Native Wheel Drive-Board Corrections | Integrated locally from `beb421ed` and `175add57`: servo nibble order/gain/idle stop, final per-frame output and bounded command queue. Native register probes, Scud/Daytona 2/Sega Rally 2 ABI runs, reset and old/new Save State checks pass; Scud Multi-threaded + GPU checked separately. Libretro preflight accepts the optional servo block only when absent, retaining strict corruption checks. Physical-controller response remains unqualified. Included in the authorized 2026-10-19 source publication. |
| 🟢 | U02 — PowerPC Interpreter Correctness | Integrated `70b1b316` with the port's cached carry API retained. Actual interpreter dispatch: 138 edge checks, eight baseline failures reduced to zero. Daytona 2 interpreter startup/reset/state round-trip and separate Sega Rally 2 JIT smoke run pass. JIT FPSCR shortcuts remain distinct and are not claimed corrected by this interpreter patch. Included in the 2026-10-19 source publication; the tagged release is unchanged. |
| 🟢 | U03 — Quad Triangle Rendering Corrections | Integrated `5dbc97fa`, `3574db93` and `53ff7674`; preserved OpenGL 4.1 prefixes and renderer reattachment. Same-state Scud/Daytona 2 comparisons: triangle-renderer images identical; Quad differences limited to 191/173 pixels in the sampled frames. Twenty live WM/AR transitions pass without reset. Targeted evidence only, not all-scene or performance qualification. Included in the 2026-10-19 source publication; the tagged release is unchanged. |
| 🟢 | U04 — Shared Model 3 / Real3D Updates | Completed the selected Model 3 scope from `a8297d4c` and `4f5d686b`: polygon bounds, base rotation/fog/LOD tag, FIFO flags, viewport/list/point-ref/no-LOD traversal, decal ordering/depth bias and LOS after transparency passes. Retained local async LOS, conditional transparency path and renderer reattachment; bound skipped-entry lists to RAM. Native CGL tests pass 29 checks each with triangles/Quad, three saved-scene comparisons are pixel-identical, VF3/Lost World boot/reset/state and 20 live WM/AR transitions pass. Pro-1000 camera/hardware/SCSI and interface refactors are assessed and excluded; no full upstream synchronization or other-platform qualification is implied. Included in the 2026-10-19 source publication; the tagged release is unchanged. |
| 🟢 | U05 — Shared Force-Feedback Effect Count | Aligned `NUM_FF_EFFECTS` to four (`0a99174d`). Existing four-effect Libretro mixer and native drive-board regressions pass. The upstream overflow affected standalone DirectInput, not this adapter. Included in the 2026-10-19 source publication; the tagged release is unchanged. |
| 🟡 | F13 — Complete Visual Game Settings Catalogue | 120/130 pending parent cycles completed, plus Bass Fishing Event Game Time; 720 cycle photographs and two contexts archived. YAML/Excel/catalogue reconciled. Only ten vs298 rows remain blocked by the local ROM CRC mismatch. |

## Current Local Delivery

Published [v0.3a-20261012-git-ed608de-libretro.1](https://github.com/Zer0one/Libretro-Supermodel/releases/tag/v0.3a-20261012-git-ed608de-libretro.1)
from `57b0701611e4ed89cd627697956eebc01a45b97e` on `main`. The release
contains four architecture-specific packages and archive checksums.
[CI 39164917017](https://github.com/Zer0one/Libretro-Supermodel/actions/runs/39164917017)
passed Linux x86_64, Windows x86_64, macOS ARM64 and macOS x86_64. All packages
record identical clean source commit/fingerprint; their GitHub digests match
the verified local archives. The earlier Windows autocrlf provenance failure
was corrected before publication.

The published CI ARM release remains the release baseline. Local RetroArch now
uses the separately verified 2026-10-19 U01/U02/U03/U04/U05 development
build under the agreed standard name `supermodel_libretro.dylib`, SHA-256
`c0082f673f738b6f1c8f5f070590599019446d41ae62818de78e24806d72ffb5`. The matching .info, user configuration and external
System XML are unchanged. The source integrations are included in the authorized
2026-10-19 publication on `main`; they are not part of the tagged release. Evidence: `outputs/autonomous-upstream-20261019/` and `outputs/u04-real3d-20261019/`.

The preceding CI ARM release delivery used the matching .info.
Core SHA-256: `b150417731ae694844b7b694c38f8ede50797c54b5844b7e473009d00fc5208f`. Twenty CGL transitions
pass on that installed CI binary. Configuration and external System XML remain
unchanged. Evidence: `outputs/release-20261018/`; historical delivery details
remain below. No cross-platform gameplay qualification is inferred from CI.

See [the menu ordering/audit report](CORE_OPTIONS_ORDER_AUDIT_2026-10-18.md).
Its Video follow-up records eight changed Video positions with all 255 per-key
definitions preserved and the new build installed. The original audit records
28 changed positions, unchanged metadata for all 255 definitions and
177 native operator fields, System's exact 225-definition sequence, the full
workbook audit and actual RetroArch ARM 1.22.2 / Ozone page inspection. The shared
review workbook and per-core workbook Order IDs are unchanged. Earlier installed
hashes identify historical deliveries, not additional cores to deploy.

The implementation uses the existing definition sequence rather than a new
runtime ordering layer. Scoped source/rebuild rollback follows F20; binary
backups are not routine. Before future delivery, reread active RetroArch paths,
resolve symlinks and verify installed hashes. The active external System root
last resolved to `/Volumes/MacBook External Drive/Retro/_Media/_Multi (RetroArch)/system`.

## Current Networking Contract And Limits

The [two-option integration report](NETWORK_TWO_OPTION_2026-10-17.md) is the
current networking baseline. Earlier Network Board / ANS instructions are
superseded, not pending work.

- Exactly two controls are visible: Linked Cabinets and Auto Network Role
  Settings (ANRS); unsupported games show the Not Supported pair.
- Linked Cabinets defaults to -1 Disabled, with 0/1 aliases. All currently
  reviewed Model 3 boards are optional, so Disabled disconnects the board.
  Required-board local loopback is the conditional Framework rule, not a claim
  that these Model 3 games require fitted boards.
- ANRS independently defaults to the native Single / Stand Alone / No Link role.
  Disabled returns manual authority to the last applied native values. Active
  ANRS works with NVRAM Settings disabled and hides only its managed fields.
  Topology does not silently convert the selected role.
- Effective topology/role/identity changes reset once on the next emulation
  frame: on Resume for paused menus, immediately for non-pausing Netplay menus.
  Explicit Restart consumes pending choices too. Content and the frontend lobby
  stay loaded; connected clients/callback ownership survive and transient
  staggered setup can recover.
- Per-title totals are reviewed/theoretical: Daytona 2–17, Scud 2–8,
  Dirt Devils/Le Mans/Rally/Virtual On 2–5, Harley/Ski Champ/Spikeout 2–4.
  These totals are not multiplayer gameplay qualification.
- Protocol v3 peers must use matching updated cores. Source/native/ABI checks
  cover 22 sets and 159 presets. Actual two-instance Scud tests establish a
  2/2 roster and first data exchange, including frontend host/native Master on
  different peers. Synchronized racing, every role combination, maximum totals,
  linked-board Save States and other platforms remain unqualified.

The current activity does not open another networking implementation or imply
that those additional qualifications are complete.

## Remaining Visual Catalogue Work

Use the [Service Menu and NVRAM summary](Service_Menu_NVRAM_Summary.xlsx),
diagnostic YAML and reviewed recipes. This is the sole current campaign
summary: Core options, Campaign, Calibration and Automatic setup. It has
209 parent setting rows and 396 clone setting rows. Implementation flags,
selector defaults, raw keys/encodings and startup overrides are checked against
the current code, including all 171 native logical settings and six operational
calibration controls. The 50 EEPROM seeds are read from the actual template
header, separately from the fields applied by automatic initialization.
The original workbook rows, clone coverage and four replaced narrative reports
are preserved losslessly in cell notes, with source hashes and historical
status. Current table status supersedes older conclusions. Native defaults
remain bold in Observed Values and distinct from frontend defaults.

The summary records 121 newly verified cycles, including one newly discovered
conditional field; ten vs298 cycles remain pending. Thirteen other clone rows
still lack direct value cycles; 250 clone value lists explicitly cite parent
evidence. The supplemental binary
campaign is complete at 35 release clones / 478 acquired values; this does not
complete every photographic value cycle.

Campaign uses Model 3 evidence-specific columns: Sample Validation, Persistence
Verification and Integrity Verification. The 22 parent recipe campaigns retain
their accepted 355 samples and photographic reconciliation; a separate frontend
reload or per-sample integrity report is not inferred. The 35-clone results
distinguish native encoding/integrity checks from save-container persistence
simulation. Scud Australia additionally records seventeen fresh RetroArch reloads
and explicit CRC/mirror checks. Backup RAM families are not labelled as EEPROM
CRC tests. Fixed frame checkpoints are not imposed on unrelated methods. This
documentary adaptation opens no new acquisition campaign and changes neither
the core, existing valid samples nor runtime policy.

1. Resume only the ten blocked vs298 cycles after the required ROM is available.
   The other 120 previously pending parent cycles and Bass Fishing Event Game
   Time are photographed and reconciled; reuse their validated manifest records.
2. Reconcile photographs against the existing YAML/recipes and samples. Preserve
   valid artifacts; generate supplemental SRM samples only for currently exposed
   settings with missing, invalid or contradicted evidence.
3. Propagate verified native mapping/default/integrity corrections to NVRAM
   Settings and Automatic Initial NVRAM when applicable. Separate native factory
   defaults from deliberate frontend overrides; retain Export-before-USA policy.
4. Keep parent evidence and direct clone evidence explicit. Do not infer a
   clone's field presence, ordering or encodings solely from its parent.
5. Check ROM availability when resuming acquisition. The vs298 catalogue expects
   `epr-20917.17` CRC `969E4BDA`. On 2026-10-18 both the Model 3 working copy
   and original MAME archive still contained CRC `C3BBB270`. Existing historical
   photographs and valid binary samples were retained; no ROM bypass was used.

For Scud Australia, normal operator-menu routing is restored. Native default is
Master / Car 1, while frontend ANRS defaults to Single. Link ID, Car
Number, Difficulty and Cabinet Type are exposed as overrides. Difficulty defaults
to Normal and Cabinet Type to Twin, matching the existing initial seed. Country
is fixed by this release and has no selector. The four other inventoried fields
are excluded from clone overrides to match the parent's exposed scope, as
confirmed by the user; they are not pending integration. The old
debug-menu photographs are historical diagnostics.

## Scope Exclusions And Deferred Decisions

By user decision on 2026-10-19, standalone GUI/configuration work, Pro-1000
and host SCSI expansion, and the multi-core launcher are excluded from this
project's roadmap, rather than deferred activities. The launcher was completed
in another project, as confirmed by the user. Previously integrated shared
Model 3 corrections remain part of U04.

Prototype analysis, inventories, acquisition, tests, implementation and maintenance
are excluded unless explicitly requested. Existing prototype records/support are
retained; old srally2p/srally2pa failures are not active completeness blockers.
Prototype and other pre-release sets remain outside the established automatic
NVRAM/override campaign. `mgtrkbad` was removed from the catalogue by user request;
Royal Ascot II is non-working and outside operational qualification.

The native widescreen assessment is complete: no verified Model 3 native widescreen
setting supports an SM2-style automatic aspect policy. Widescreen Mode remains an
explicit rendering enhancement. New primary evidence can reopen this assessment.
Synthetic cornering rumble has no counterpart to adopt: Supermodel forwards its
native drive-board commands. Neither result authorizes inventing a replacement.

Any future Quad rendering correction needs a demonstrated cause, preserved native
interpolation and controlled image validation. The completed investigation does
not authorize a replacement renderer or Vulkan backend.

## Package, Upstream And Publication Boundaries

[Upstream integration and release identity](UPSTREAM_INTEGRATION.md) records the
2026-10-13 reviewed snapshot, integrated/adapted/excluded changes and future
version scheme. This roadmap refresh is not an upstream update check.
Standalone GUI/configuration changes are excluded by user decision; RetroArch
retains frontend ownership. Unmerged SCSP #349 and GLES/Wayland #332 remain
upstream watch notes, not scheduled integration tasks.

The existing package gate assembles core, .info, Games.xml, Music.xml, notices,
source provenance and checksums. The existing [GitHub run 36183994220](https://github.com/Zer0one/Libretro-Supermodel/actions/runs/36183994220),
on commit 4566c94, already passed all four platform jobs on 2026-09-25, including
format/API/dependency validation and artifact assembly/upload. This was reconfirmed
on 2026-10-18; the published 0.3.0-beta binaries are not an outstanding task.

The 2026-10-18 source delivery is now committed and pushed: `14a740d6` contains
the accumulated implementation and documentation, followed by `c12cd392` for
the package-probe Linux host dependency and Windows MSYS2 Git prerequisite.
[GitHub run 39134741470](https://github.com/Zer0one/Libretro-Supermodel/actions/runs/39134741470) passed all four platform jobs on `c12cd392`,
including build, format/API/dependencies, package checks and artifact upload.
The final documentation-only publication record does not change that tested
production source. It skips a redundant rebuild.

The public README and repository description are updated, and the default
branch is `supermodel-modern`; the old `master` branch remains intact. Local
ROMs, saves, build products and raw test outputs were excluded from the push.
Platform runtime validation, Libretro distribution and publication remain
distinct. A later authorized release must reconcile runtime version, .info,
tag/title and release notes; no new release was created by this source delivery.

## Framework Reporting And Historical Evidence

The user authorized meaningful progress/finding/delivery reports to Framework
chat `01a13152-dbfa-7e30-bc9d-0477abc2cccc`, host `local`. Report source/registration,
actual frontend and installed identity separately, with paths/hashes and limits.
This authorization does not permit work in other core chats or publication.

The [historical roadmap snapshot](ROADMAP_HISTORY_2026-10-18.md) retains the prior
requirements, old counts/hashes and detailed provenance without presenting them
as active instructions. Referenced delivery reports remain evidence at their
recorded dates; their scope is not silently expanded by this reconciliation.

## Active Netplay Menu Lifecycle Follow-Up — 2026-10-18

The required workflow is to start both instances as native Standalone in an
existing lobby, edit Linked Cabinets and ANRS while the local Quick Menu is open,
then commit only the final effective choices with one local machine reset on
menu exit. The lobby, callbacks and connected roster must survive. Continuing
emulation for transport does not justify applying each intermediate edit.

Actual Daytona 2 checks distinguish paused offline menus from active Netpacket
menus. The first passes; the latter currently resets immediately per effective
ANRS change. The same symptom is reproduced in installed TGPulse with Virtua
Racing. SM2 has the same next-retro_run application pattern by source review;
its runtime behavior in this scenario is not yet qualified here.

The existing ABI runner treats its next retro_run as Resume and therefore does
not establish real menu-exit timing in Netplay. No supported menu-exit signal was
identified in the current ABI; throttle state cannot distinguish a non-pausing
Netplay menu. The user explicitly approved retaining current behavior if reliable
deferral is unavailable. Accordingly, immediate effective-change application is
retained in that context; paused-session behavior and lobby preservation remain
unchanged. A frontend lifecycle extension is a possible future investigation,
not an approved frontend implementation task. Details and evidence are
in [the networking report](NETWORK_TWO_OPTION_2026-10-17.md#active-netplay-menu-lifecycle-follow-up--2026-10-18)
and `outputs/anrs-menu-20261018/`. No production-code change, new core build,
commit or push is claimed by this investigation.

## Audio Reset Recovery — 2026-10-18

Fixed a persistent silent-output state caused by clearing the Libretro ring
buffer to identical read/write positions during reset. Reuse the existing
startup initialization, including its two silent packets, so the asynchronous
producer and consumer can restart. No emulated sound-board, gain, DSP, transport
or frontend-driver change is involved.

The installed old core produced only zero PCM throughout eight ten-second
windows after two resets. The corrected core recovered nonzero PCM after both.
Actual RetroArch recordings passed two resets in Single and one in the user's
Multi-threaded + GPU mode; the user also confirmed correct live sound before
and after reset in Multi-threaded + GPU without recording. Initial distorted
live playback under the first Single/recording trial was not reproduced under
the aligned no-recording setup; the recordings themselves were confirmed
correct. Its exact output-path cause is not established and no driver fix is
claimed.

Build/export/dependency checks and the reused Daytona 2 lifecycle regression
(18 presets, native/reset/handover/client retention) pass. Standard installed
core matches the build: `5cca917dc93bcc9ea9b65377d5f87658533038c14c1bee20506b4e04442c7862`. The two user instances
were closed normally after authorization; all QA instances exited normally.
Evidence: `outputs/audio-reset-20261018/verification.json`. No commit or push.


## Widescreen Value-Label Follow-Up

The user approved these display names for the existing saved values:

| Saved Value | Display Label | Behavior Boundary |
| --- | --- | --- |
| `disabled` (Default) | Disabled | Original rendering; RetroArch must force 16:9 for full-image stretch. Core Provided retains 4:3. |
| `enabled` | Expand 3D View Only | Expand the 3D view while preserving native 2D proportions. |
| `wide_background` | Expand 3D View + Stretch 2D Background | Also stretch the lower 2D background; upper layers retain their proportions. |

Only display labels and the explanatory help changed. The option key, saved
values, default, category, order, restart policy and renderer remain unchanged.
The README reflects the same behavior. Supermodel's lower-background stretch
is not identical to TGPulse's stretch of both native 2D layers.

Build, the existing 25-export/dependency gate and actual API 2/1/0 registration
checks pass for all 255 definitions. Modern metadata changes only for the
three labels and help of this key; legacy API 0 retains its raw-value strings.
The normal installed core is hash-matched. Actual frontend UI/gameplay checks
were not repeated for this label-only change. Evidence:
`outputs/widescreen-labels-20261018/{verification,deployment}.json`. No commit
or push was requested for this follow-up.


The subsequent user clarification restores **Disabled** as the first label.
Both Expand modes directly publish 16:9, independent of frontend Aspect Ratio;
Disabled retains original rendering and reports 4:3. Forcing 16:9 in RetroArch
while Disabled still stretches the complete image. This supersedes the earlier
Stretch label, not the renderer behavior. The matching TGPulse adaptation keeps
native-widescreen titles unchanged and makes its expansion independent of its
separate Aspect Ratio selector. Supermodel stretches only the lower background;
TGPulse's optional 2D stretch still covers both native layers.

The revised Supermodel build, 25-export/dependency gate and all 255 API 0/1/2
registrations pass. Compared with the immediately preceding local delivery,
only the first value label and help changed. The normal installed core matches
`f7254edfd9e86924e4558636cdf457ba1082f4ed99a5e39763926e7a127ab9b9`.
Evidence: `outputs/widescreen-direct-20261018/`. No commit or push.


## Aspect Ratio Gate — 2026-10-18

The user's final rule supersedes unconditional artificial expansion: Widescreen
Mode acts only when the core's Aspect Ratio is Auto or 16:9. Explicit 4:3
suppresses expansion while retaining the saved WM choice. The user approved
adding the missing core selector; the global RetroArch aspect setting is not
readable through the Libretro ABI.

- Aspect Ratio: Auto/default, 4:3, 16:9; saved `auto`, `4_3`, `16_9`. It sits
  immediately before Widescreen Mode. Auto follows the effective WM choice,
  with native 4:3 fallback; no native Model 3 wide monitor is invented.
- Disabled + 16:9 presents the original 496x384 image stretched to 16:9.
  Disabled + Auto/4:3 remains 4:3.
- Expand + Auto/16:9 renders 683x384 and reports 16:9. Expand + 4:3 renders
  native 496x384 and reports 4:3, including the optional background mode.
- Aspect Ratio applies on Resume without resetting the machine. The existing
  resize/rebuild path parks workers before changing renderer flags. Aspect-only
  changes publish geometry even when pixel dimensions stay unchanged. WM changes
  now apply on Resume as well (see the live-selector follow-up below). RetroArch Core Provided honors the result;
  an explicit frontend override remains frontend-owned.

All 256 definitions pass API 0/1/2 checks. Exactly one definition was added;
previous keys/order/defaults/choices remain intact, with revised WM help. The
macOS build and 25-export/dependency gate pass. Reusing the CGL host verifies
12 live transitions, stable geometry notifications, no automatic machine resets,
and three explicit Restarts with one reset each in Multi-threaded + GPU mode.
Three isolated actual RetroArch glcore runs confirm 4:3 suppression, Auto
expansion and Disabled + 16:9 Stretch; screenshots were retained. These are
geometry/image-delivery checks, not new 3D/gameplay or other-platform acceptance.

Installed/build SHA-256:
`fcf6ea2d0049a8d8dcf5a1902e8e16313900965a358c98b5cc0990510a18fe53`.
Info/config/System files remain unchanged. Evidence:
`outputs/widescreen-aspect-gate-20261018/`. TGPulse and SM2 were instructed to
use the same Auto/16:9 gate with native widescreen unchanged; TGPulse's corrected
build is verified and installed. No commit or push.


## Widescreen Mode On Resume — 2026-10-18

The user requested the same live selector lifecycle verified in TGPulse. Reuse
the existing Aspect Ratio resize path, which parks/resumes workers around
renderer reinitialization; no machine reset or content reload. Remove the load-only WM cache and use the current selection read by the existing
Resume options block. Track effective widescreen
and background flags as well as dimensions: the two Expand modes have identical
geometry but different lower-background behavior. Background-only changes do
not publish redundant frontend geometry. Projection, shaders and composition
remain unchanged. Renderer/Supersampling restart rules remain unchanged.

Verification: macOS build and 25-export/dependency gate pass. All 256
registered definitions are preserved across API 0/1/2, except the WM help text.
Twenty actual CGL hardware-frame transitions verify paused callbacks are inert,
no automatic machine resets, explicit 4:3 suppression, Disabled + 16:9 stretch,
both Expand modes, same-size background renderer refresh and no redundant
geometry/reinitialization. One isolated RetroArch 1.22.2 macOS ARM glcore
Daytona 2 session verifies Disabled -> Expand -> Expand + Background -> Disabled
on Resume through actual 3D attract scenes, with no machine reset or reload.
Other platforms were not runtime-tested in this follow-up. Evidence:
`outputs/widescreen-live-selector-20261018/`. Public local core installed with
matching SHA-256 `112b326cd6770409619c6058b47c02c1cd398bc8a0e05112ad910c8d27fd8af5`; matching info, active configuration, System assets and TGPulse core preserved.
No commit or push.


## Live Presentation Texture Regression — 2026-10-18

Corrected the user-reported Daytona 2 texture corruption after WM/AR changes.
The previous green lifecycle result was insufficient for texture integrity.
Recreated renderers now receive texture RAM and persistent native rendering
registers through AttachRenderer, reusing LoadState's native operations. New
content loads also clear stale viewport margins. No reset/reload, shader or
projection change. Fifteen static/live pixel comparisons and twenty CGL
transitions pass, including the user's 2x/Multi-threaded + GPU settings.
Build/API/dependency checks pass; 256 options unchanged. macOS runtime only.
See [Renderer State After Live Presentation Changes](WIDESCREEN_RENDERER_STATE_2026-10-18.md)
for failed-probe distinctions and limits. Verified local deployment recorded
in `outputs/widescreen-renderer-state-20261018/deployment.json`. No commit/push.


## Snapshot Release Publication — 2026-10-18

User-authorized source commits `cea5eae2` (live presentation/state correction and
release identity) and `57b07016` (Windows checkout integrity) are published on
`main`. Tag `v0.3a-20261012-git-ed608de-libretro.1` targets `57b07016`, matching
the four CI package provenance files. The accompanying documentation-only
record does not change the tested release source and skips redundant CI.
Runtime version, .info display version, tag/title and release notes agree.
The repository default remains `supermodel-modern`; no historical branch was
rewritten or deleted.
