# Upstream Integration And Release Identity

Review Date: 2026-10-13

## Sources And Coverage

The port is based on `libretro/Libretro-Supermodel`, retaining the frontend
lineage of `sgiannop/Libretro-Supermodel`. Standalone `trzy/Supermodel` supplies
machine and renderer reference changes; a fetch is not an integration.

Port baseline before this delivery: `4566c9470ab89ec8e2738bfb43c18281df580971`.
Current reviewed standalone tip: `ed608de1c5f0f61dcad957cffe0ff8234cbc8e6c`.
The changes below are included in the 2026-10-18 source delivery on
`supermodel-modern`; this delivery does not assign a new release tag.
This ledger records the reviewed delta, not a claim that every older upstream
commit or future platform has been reconciled.

| Source Change | Disposition | Evidence / Boundary |
| --- | --- | --- |
| Libretro master `84bc106`; dev `f2e1e7d` — MinGW dependency/default-target fix | Integrated in `4566c94` | Existing Makefile has the default goal and generated-header prerequisites. No new import required. |
| Standalone `7d3b6f4`, PR #339 — Quad on OpenGL 4.1 | Adapted in this source delivery | Native shader body matches the same-revision upstream after removing its inline version directives. The port's central GLSL prefix is retained and also applied to geometry shaders. macOS now compiles the existing option; published default remains Disabled. |
| Standalone `b5125ca`, PR #327 — vs298 corrected dump | Integrated in this source delivery | CRC `969E4BDA` replaces `C3BBB270` in XML/DAT and the embedded catalogue. Local old-dump rejection is verified; corrected-dump gameplay is unavailable. |
| Standalone PR #336 — GUI preservation of per-game INI sections | Excluded from project scope by user decision (2026-10-19) | The adapter does not write the user INI. No standalone configuration writer is introduced. |
| Standalone `44404c2`, PR #340 — GUI labels/custom values and helpers | Excluded from project scope by user decision (2026-10-19) | RetroArch owns the option UI. GUI helpers and mechanical cleanups are not planned integrations or imported emulator fixes. |

The LCF changes are port-specific frontend adaptations. They do not establish
additional standalone upstream coverage; their status is recorded only in
`ROADMAP.md`.

## Snapshot-Based Port Version Scheme

Supermodel's upstream identifies snapshots as
`v0.3a-YYYYMMDD-git-SHA`, rather than a sequence such as SM2's `0.9.44`.
For a future snapshot-based port release, use the reviewed upstream snapshot
identity plus `-libretro.N`, where N is the port revision. Advance N for
port-only releases; restart it when the reconciled snapshot advances. The
release notes must state the exact integrated/adapted/excluded scope and any
applicable pending changes before assigning that identity.

This review assigns no new public tag and rewrites no historical release.
The existing public `0.3.0-beta` tag and runtime `v0.3a-libretro` remain
historical/current identities respectively. A later authorized release must
align runtime version, .info, tag/title and release notes in one scoped change.
Do not present the latest fetched upstream tag as the built port's version.

## Package Gate

Reuse the normal Makefile and existing binary format/export/dependency gate.
The CI then calls `.github/scripts/package-libretro-core.py CORE OUTPUT`;
`--check` verifies an already assembled directory. The package includes the
core, .info, Games.xml, Music.xml, license notices, source provenance and
SHA-256 checksums. Source provenance distinguishes a clean committed source
from a modified tracked working tree and records its fingerprint.

There is no additional runtime database/manifest or packaged default INI.
Embedded fallback and non-overwriting user overrides retain their existing
runtime ownership. The package gate compares actual runtime library identity
with metadata and checks every shipped file's hash and exact inventory.

The standalone desktop Linux identity probe preloads the system OpenGL library,
matching the frontend's host dependency while retaining eager symbol resolution.
The Windows MSYS2 environment explicitly installs Git for build identity and
source provenance; the checkout action's separate Git is not on that shell's PATH.

Historical multi-platform binary validation is complete: [GitHub run
36183994220](https://github.com/Zer0one/Libretro-Supermodel/actions/runs/36183994220),
commit `4566c9470ab89ec8e2738bfb43c18281df580971`, passed Linux x86_64, Windows
x86_64, macOS ARM64 and macOS x86_64 on 2026-09-25. Each job passed build,
format/API/dependency validation, artifact assembly and upload. The corresponding
0.3.0-beta artifacts were packaged and published; job results were reconfirmed
on 2026-10-18. They are not pending verification.

The 2026-10-18 source delivery (`14a740d6`, followed by package prerequisites
in `c12cd392`) passed [GitHub run 39134741470](https://github.com/Zer0one/Libretro-Supermodel/actions/runs/39134741470) on Linux x86_64, Windows
x86_64, macOS ARM64 and macOS x86_64. All four jobs passed binary validation,
package inventory/runtime identity/provenance/checksums and artifact upload.
The initial run exposed missing host OpenGL for the isolated Linux identity
probe and missing Git inside Windows MSYS2; both were corrected and the full
matrix rerun successfully. The final documentation-only record retains this
tested production source and skips a redundant rebuild.

This supersedes the previous pending status of the new checker; it does not
expand native gameplay qualification or update the existing release. CI success,
native gameplay, Libretro-hosted distribution and publication remain different
claims.


## Local Renderer Reattachment Candidate — 2026-10-18

`CReal3D::AttachRenderer` now repopulates GPU texture banks and restores sun
clamp/block-culling registers from native state, using LoadState's existing
operations. This fixes replacement-renderer texture loss in Libretro WM/AR
changes. It is a small candidate for upstream review, not an upstream submission
or standalone runtime qualification. Keep it when merging an upstream update
unless an equivalent reattachment synchronization is present. Evidence and
limits: [Renderer State](WIDESCREEN_RENDERER_STATE_2026-10-18.md).


## First Snapshot-Based Port Release

The user authorized commit, push and a new release on 2026-10-18.
`v0.3a-20261012-git-ed608de-libretro.1` aligns runtime identity, .info display version and release
tag/title. It uses the reviewed `ed608de` snapshot identity and port revision 1,
with the selective coverage above; excluded standalone GUI changes remain
excluded. Publication now uses `main`; historical branches are retained.
The earlier references to 0.3.0-beta and v0.3a-libretro describe the preceding
delivery. See [current release notes](RELEASE_NOTES.md).


## Release Checkout Integrity

Windows Git autocrlf conversion changes mixed-ending upstream source files and
can make a clean CI checkout appear modified. The Windows CI job now disables
autocrlf and restores tracked files from the index in its fresh checkout before
building, preserving committed bytes. Artifact assembly rejects tracked working
tree changes on every platform. This does not normalize upstream files or alter
the user's working copy. Release archives must record the same clean source
commit and tracked-source fingerprint across all platforms.


## U01 — Native Drive-Board Integration — 2026-10-19

Selectively integrate standalone `beb421ed040a2f1f654ddc9703f5dc9ac8dd5941`
and `175add57` without importing other new upstream work. Vibration nibbles are
assembled low-first, torque/vibration honor servo gain, motor idle clears pending
forces, and wheel output uses the final values of each frame. A bounded native
command queue retains distinct main-board writes for complete Z80 frames. Reset
and load clear the queue as upstream specifies. The Libretro output mixer remains
unchanged; no synthetic force or five-percent threshold is introduced.

Adapt the patch to the port's pointer/size BlockFile API, existing corrected method
names, null-safe output route and CTC load order. Keep the optional upstream
`WheelBoardServo` gain block and full-gain fallback for older states. Libretro's
strict state preflight additionally accepts layouts without that optional block;
a malformed present block or another layout mismatch still fails before mutation.

Evidence: `outputs/u01-driveboard-20261019/`. Native register probes cover fifteen
vibration levels, gain/idle, command FIFO/dedup/overflow/reset and gain persistence.
Before/after isolated CGL/Libretro runs cover Scud, Daytona 2 and Sega Rally 2,
including old/new state loads and reset; a separate Scud run uses Multi-threaded
+ GPU. Build/API checks and the existing Libretro mixer regression pass. These
checks do not establish physical gamepad response or other-platform gameplay.
Local source/build/deployment only: no commit, PR update or release is implied.


## U02–U05 — Autonomous Local Integration — 2026-10-19

Evidence: `outputs/autonomous-upstream-20261019/`. These are selective local
changes on top of U01, not a full synchronization with standalone `ddbc3066`,
not a PR update and not a release. No commit or push was performed.

- **U02 / `70b1b316`:** arithmetic alias/carry fixes, defined unsigned overflow
  and rotation, aligned RFI target, FPSCR field masks/comparison flags and safe
  floating-point-to-integer clamping. Preserve `ppc.xer_ca` and the port's carry
  setters rather than replacing them with direct XER bit writes. Actual production
  interpreter dispatch passes 138 checks; the old binary fails eight. Existing
  upstream TODOs remain TODOs. The ARM JIT has its own FPSCR/rounding shortcuts;
  this interpreter import does not remove or qualify those shortcuts.
- **U03 / `5dbc97fa`, `3574db93`, `53ff7674`:** duplicate the final vertex for
  triangles, rotate degenerate quads consistently and use the explicit triangle
  shader path. Retain the Libretro GLSL prefix and GLSL 4.10-compatible flat arrays.
  Quad must be selected before content startup; the comparison starts separate
  old/new core sessions with Quad enabled rather than assuming live switching.
- **U04 / selected `a8297d4c` and `4f5d686b` hunks:** constrain polygon headers
  and vertex payloads to Model 3 RAM/VROM, stop unterminated models at the last
  valid polygon, restore the base coordinate-system rotation on reset_matrix,
  accept the native disabled-fog sentinel, use the LOD child tag and self-clear
  SCSI FIFO reset flags. Keep existing fixed Model 3 memory sizes/interfaces and
  renderer reattachment. The local CTC already expands raw zero to 256 without
  changing serialized layout; do not import the upstream UINT16 field blindly.
- **U05 / `0a99174d`:** `NUM_FF_EFFECTS` is four. The common Libretro adapter
  already has four independent effect slots; no new mixing or threshold rule.

At the end of the initial subset, U04 was **partial**. Viewport/direct-model and pointer-list traversal, point-ref/
no-LOD handling, LOS ordering and layered/decal changes still need a separate
Model 3 relevance assessment and targeted qualification. The Pro-1000 camera
matrix path, host SCSI, IRQ/JTAG abstraction and stepping/enumeration refactoring
are not prerequisites for the selected corrections and were not imported. No
claim is made that all shared changes or the complete standalone commits landed.

Validation uses the actual ARM64 core in an isolated CGL 4.1 ABI host, not the
user's running RetroArch sessions. Four same-state scene comparisons cover Scud
and Daytona 2: triangle-renderer pixels are identical before/after; Quad changes
191/173 pixels in those sampled frames, with both new images inspected. This
is bounded visual regression evidence, not proof for every triangle, depth or
texture case. The host retains the same pre-existing GL error 1282 observation
in both baselines; these runs do not claim a clean GL error stream. Twenty
Multi-threaded + GPU live WM/AR transitions pass without reset or redundant
refresh. Separate Daytona 2 interpreter and Sega Rally 2 JIT boot/reset/state
smoke runs distinguish execution engines. Native bounds/SCSI register probes,
U01 drive-board checks, the existing effect mixer regression and ARM64 API gate
pass. Physical rumble, other-platform builds/gameplay and JIT numerical parity
are not established by this local campaign. Temporary probes stay in `outputs/`;
no additional permanent Makefile checks are introduced.

Installed locally as `supermodel_libretro.dylib`, SHA-256 `c515e60a45c61f04db87e600f3d91cbe9b444695f1f754f94167bcaf8784ed83`.
The installer reread the active RetroArch System path, resolved its external
volume and verified configuration, core options, .info and both XML hashes
unchanged. No `_dev` filename or support-file migration was introduced.


## U04 — Remaining Model 3 Renderer Scope Completed — 2026-10-19

This follow-up supersedes the partial U04 status above. Integrate the remaining
shared renderer behavior from `a8297d4c`: tagged viewport model/list children,
skipping inactive list entries until the actual terminator, point-reference
siblings while retaining the ordinary colour-table sibling restriction, and
index 127 as the no-LOD sentinel. Bound pointer-list iteration to the active
Model 3 culling RAM region: skipping inactive entries must not walk past RAM
when a list lacks a terminator. Keep the earlier polygon memory bounds.

Draw non-layered geometry before decals within each viewport and use upstream's
reverse-depth multiplier `1 + 1/512` only for decals, in both triangle and Quad
shaders. Preserve unit depth for ordinary geometry and GLSL version handling.
The port already had the correct depth-fail stencil operation; retain it rather
than treating that overlapping hunk as a new correction.

There is a semantic overlap with local rendering optimizations, not an
unresolvable incompatibility: standalone moves LOS sampling after transparency
rendering, while this port conditionally skips transparency passes, uses a
separate trans2 framebuffer sharing the copied depth/stencil attachment, and
collects LOS asynchronously. Put the sample after the conditional transparency
block, outside it: no-transparency frames read colour depth; transparency frames
read the final trans2 attachment. Keep the PBO queue/collection and existing
frame latency, conditional pass optimization and renderer state reattachment.

### Evidence And Limits

Evidence is in `outputs/u04-real3d-20261019/`. Temporary probes call actual native
renderer functions and production shaders in a CGL 4.1 context; they are not new
permanent Makefile tests. The prior installed core fails eight targeted cases;
the updated candidate passes 29 checks with triangles and 29 with Quad. The
candidate also checks the two new RAM-boundary guards and all three states of
the active depth-bias uniform, hence the counts differ from the old candidate.
Tests include actual decal/surface pixel output in both input orders, shader
linking, active colour/trans2 depth-stencil readback through the asynchronous
LOS decoder, direct viewport/list paths, point-ref versus normal siblings,
no-LOD alpha, null terminators and unterminated list boundaries.

Three same-state comparisons (Scud and Sega Rally 2 with Quad, Daytona 2 with
triangles) are pixel-identical before/after this follow-up. VF3 and The Lost World
complete 1200 boot frames, state round-trip and reset in the isolated core host;
boot images were inspected. Twenty Multi-threaded + GPU live WM/AR transitions
still pass without reset/redundant refresh. PowerPC, polygon/SCSI, effect-mixer,
U01 drive-board and ARM64 API regressions pass. Historical host GL error 1282
remains a baseline observation in the game-scene runner, not a new U04 failure;
the targeted decal draws read the expected output. These tests do not establish
all-game gameplay, real lightgun targeting, physical rumble, cross-platform
runtime behavior or performance improvement.

### Assessed Exclusions

The nonzero environment-camera matrix path is explicitly described by the
upstream commit as unused by Model 3. Do not import it or Pro-1000 layered-memory,
ROM, new host-SCSI/target-mode transport and hardware support. The `4f5d686b` /
`ddbc3066` IRQ/JTAG/stepping-enum and emulator initialization changes primarily
support the new platform; importing the hierarchy is not required by these
Model 3 renderer corrections. Model 3 keeps its existing DMA SCSI path, fixed
memory layout, IRQ interfaces, save layout and local renderer reattachment.
The local CTC zero-to-256 correction is already equivalent without widening its
serialized field. Completion here means this selected Model 3 review/integration
scope, not that the complete standalone commits were merged.

The final U04 ARM64 core is installed as the agreed standard
`supermodel_libretro.dylib`, SHA-256 `c0082f673f738b6f1c8f5f070590599019446d41ae62818de78e24806d72ffb5`.
Installed bytes match the tested build. Active RetroArch configuration, core
options, .info, Games.xml and Music.xml are unchanged. No user RetroArch process
was used or closed; all test processes completed. No commit, push or release.


## Source Publication — 2026-10-19

The user authorized commit and push of the accumulated U01–U05 integrations,
including the completed selected Model 3 U04 scope and roadmap exclusions.
Publish on existing `main` to `origin/main`; the existing Libretro PR #20 follows
that branch. Update the project README to describe these development changes
separately from the current tagged release. Historical local-only/no-commit
statements above describe the earlier validation stages, not this publication.
No new release, tag or repository default-branch change is included. Temporary
probes, local build artifacts and desktop metadata are excluded from the commit.
The installed ARM64 core remains the previously verified local build; source
publication does not imply new cross-platform CI/gameplay qualification.
