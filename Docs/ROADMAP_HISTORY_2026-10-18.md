# Historical Roadmap Snapshot — Before 2026-10-18 Reconciliation

This file preserves the previous roadmap verbatim below. It includes superseded
counts, prototype acquisition requirements, deployment conventions and old
Network Board/ANS semantics. It is historical evidence, not current instructions
or a second implementation roadmap. Use [the current roadmap](ROADMAP.md).

---

# Roadmap

## Two-Option Network Convention — 2026-10-17

Status: Implemented And Delivered Locally — 2026-10-17. No Commit Or Push.

The user-authorized integration is complete. See [the delivery report](NETWORK_TWO_OPTION_2026-10-17.md) for implementation, evidence and qualification limits. The requirements below are retained as the acceptance record; historical Network Board/ANS sections are superseded.

Authoritative contract: [Framework 0.1.30, F05/F14](/Users/andrea/dev/libretro-core-framework/FRAMEWORK.md), the latest edition reviewed on 2026-10-17. It includes the 0.1.23 lifecycle requirements and subsequent final-registration, functional-ordering and delivery guidance. Adoption uses the existing Supermodel adapter/runners and the reviewed TGPulse/SM2 solutions.
Per-game review: [Network Review](/Users/andrea/dev/libretro-core-framework/outputs/01a13152-dbfa-7e30-bc9d-0477abc2cccc/Framework_Network_Review.xlsx), single sheet. This revision supersedes prior Network Board / Automatic Network Settings semantics and older network comments in Menu Review; the final two-option implementation is the current local baseline.

- Exactly two always-visible controls: Linked Cabinets and Auto Network Role Settings (ANRS); both display Not Supported for unsupported games. Remove the separate Network Board option.
- Linked Cabinets: -1 Disabled/default, with 0/1 aliases; board disconnected unless required, then local loopback. 2..n per-game totals; theoretical role count permitted where unknown, identified as theoretical.
- ANRS: -1 Disabled/manual; 0 native Standalone/No Link default; 1 Master; 2..n Slave/ID; r special native role with l/s aliases. Use native labels and contextual ID descriptions. When Standalone is not separately defined, Master means Standalone in loopback and Master in a network, not a converted role.
- Keep topology and role independent. ANRS overrides NVRAM Settings and works when it is disabled. Hide only the corresponding native role/ID controls while automation is active; preserve unrelated fields and integrity.
- Retain program-implied roles/identities; dedicated Relay programs have one choice, and non-Relay Virtual On must not offer Relay. Respect the reviewed per-game defaults.
- No compatibility with the previous networking model. Invalid values fall back to default with notification. Coalesce effective changes into one automatic reset on Resume; unchanged Resume does not reset.
- Acceptance: all applicable catalogue rows mapped; modern/legacy registration, labels/values/defaults/aliases, unsupported-game visibility, NVRAM priority and last-applied authority handover, native persistence/integrity, local and representative linked behavior checked separately. When delivery is authorized, follow current F19: verify installed artifact identity and use scoped source/rebuild rollback; retain a binary backup only for a concrete need or explicit request. Do not present theoretical totals as multiplayer qualification.
- Prototypes are excluded from analysis/acquisition/testing/implementation unless the user explicitly requests them. Royal Ascot II is not working and excluded from operational qualification.

### TGPulse And SM2-Emu Findings — Applied Adoption Checks

Apply the [cross-core adoption review](/Users/andrea/dev/libretro-core-framework/TGPULSE_NETWORK_ADOPTION_REVIEW_2026-10-17.md) as the integration interchange alongside current F05/F14. Reuse the documented [TGPulse integration](/Users/andrea/dev/tgpulse-next-libretro/docs/NETWORK_TWO_OPTION_2026-10-17.md) and [SM2-Emu integration](/Users/andrea/dev/sm2-emu-libretro/Docs/NETWORK_TWO_OPTION_2026-10-17.md) as implementation/evidence references, adapting the existing Supermodel runner and adapter rather than starting another framework. The review's original SM2 consuming-read exposure was subsequently corrected in that destination; its reported correction is a reference, not a defect to assume in Supermodel. The checks below were applied; destination-specific evidence is recorded in the delivery report.

- **Existing F05 mechanism — preserve and verify:** `g_core_options_update_pending` already retains updates before paused visibility reads in `update_core_option_visibility`, merges them in `retro_run` on Resume and is cleared for a new environment. Do not replace this with an unguarded GET_VARIABLE path or claim the mechanism is missing. Regression checks must model consuming GET_VARIABLE reads, repeated paused callbacks and application without display callbacks; verify real frontend reset/OSD behavior. Retain a failing-before/passing-after case if an additional consuming-read defect is found.
- **Supermodel correction — remove the superseded override:** replace the `!linked` branch of `LibretroNvramSettings::ApplyAutomaticNetwork`, which currently forces native role/ID defaults from topology, with independent topology and role selection. ANRS must apply with NVRAM Settings disabled. Keep managed role/ID selectors synchronized with effective native values while hidden; disabling ANRS reveals the last applied values under NVRAM Settings when enabled, otherwise native game management. Never restore stale manual choices, and do not let manual role overrides compete with active ANRS. Unrelated fields retain their normal authority.
- Preserve frontend lobby ownership, registered send/poll callbacks and already-connected clients across Resume and explicit Restart, including cabinet-total changes. Do not unload content or invoke Netpacket stop/start for machine reconfiguration. Clear stale machine/frame data without tearing down the lobby; verify already-open lobbies, existing/new clients, staggered setup/recovery, board fitting/removal and effective topology/role/ID before native frame one. Retain independently selected roles when the board is disconnected.
- Coalesce effective topology and NVRAM changes into one reset, including fresh-machine construction. Verify no reset for unchanged Resume or aliases with unchanged effect.
- Resolve native roles/IDs from reviewed Service Menu mappings rather than numeric positions or table indexes; do not transfer another title's colors/encodings. Verify existing-save and clean-start behavior, unrelated-field preservation, native checksums/mirrors and fresh reload. Publish frontend setters outside locks that their callbacks can re-enter; demonstrate synchronous re-entry safety without copying a language-specific locking design.
- Diagnose actual topology and native role before attributing communication errors to reset or lobby loss; independent selections do not guarantee every pair works. Apply the new reviewed per-game defaults rather than preserving superseded networking defaults, and do not silently convert roles.
- Reuse destination runners and the final-registration audit below. Compare complete registered text and deterministic V2/legacy ordering, including supported-pair adjacency and unsupported placeholders; counts/common prefixes alone are insufficient. Do not infer the same wording/sort defect from another core's report.
- Distinguish source/ABI/transport checks, actual frontend lobby/reset behavior, synchronized gameplay and installed-artifact identity. TGPulse/SM2 evidence does not qualify Supermodel; theoretical totals and roster/COMM readiness do not establish synchronized racing. Keep Save State restrictions tied to Supermodel's actual serialization support rather than importing the Model 1 policy. Retain prototype exclusions and existing Framework progress reporting.

### Progress Reporting To The Framework Chat

The user explicitly authorized this project's chat to send meaningful integration updates to Framework thread `01a13152-dbfa-7e30-bc9d-0477abc2cccc`, host `local`, using send_message_to_thread. Report implementation progress, blockers, any proposed contract changes, validation evidence/limits and local delivery with paths and hashes. Update this roadmap as work progresses. Report facts without treating findings as authorization to alter the shared contract. This reporting authorization does not itself authorize implementation or commit/push.

Model 3 result: the independent two-control model, ANRS precedence, native authority handover and retained lobby ownership are implemented. Final API 0/1/2 registration passes for 255 definitions and 22 supported pairs; native/ABI checks cover 159 presets. The installed macOS ARM core passes a real menu/default/Resume check; two actual RetroArch peers exchange native Scud frames with frontend host and native Master reversed. Maximum totals and synchronized racing remain unqualified. srally2p and srally2pa remain excluded.

Scud Australia: corrected the catalogue patch that redirected Test to a graphics debug menu. The complete operator inventory now records eight settings and thirty distinct values, with 52 primary photographs and eleven native-committed role/Car Number samples verified by fresh RetroArch reload. Link ID, Car Number, Automatic Initial NVRAM, diagnostic YAML, recipe, workbook and curated archive are reconciled. Existing valid samples were retained. The complete clone validator passes 472/472 acquired values across 35 release clones.

## User-Revised Framework Menu Wording — 2026-10-13

### Frontend Option Ordering — 2026-10-17

Status: Pending — Destination Adoption Review. User-approved directive: [Framework F04](/Users/andrea/dev/libretro-core-framework/FRAMEWORK.md) and [Framework roadmap](/Users/andrea/dev/libretro-core-framework/ROADMAP.md), Frontend Option Ordering.

- Within menus/submenus other than System, preferably place global options at the top. Group options displayed as `<Game Name>: ...` at the bottom and sort lexicographically by displayed label while preserving reviewed functional groups. Functional grouping takes priority: keep Horizontal Range immediately followed by Vertical Range at the Horizontal group position. Sort around groups rather than splitting them; review actual related parameters before adding further groups (Framework 0.1.30 F04).
- Prefer broad independent controls before specialized functional blocks, while preserving explicit category priorities and meaningful internal block order. Adapt source-backed task groups from the SM2 interchange; do not move a specialized controller ahead of an independent general control merely because its name sorts earlier.
- System is generally exempt and retains its functional grouping/order. Preserve explicit exceptions such as NVRAM Settings and native operator-field ordering. Do not move options between categories to satisfy this rule.
- Review final modern and legacy registration order within the grouping supported by each API, preserving keys, values, defaults, visibility and established exceptions. Keep workbook sorting and stable Order identifiers unchanged.
- Review the complete registered catalogue for real dependency/axis/numbered-source/native blocks using implementation and reviewed references. Preserve meaningful internal order before sorting blocks, with controlling modes before dependent settings where justified. Names, adjacency and categories alone do not establish a group; do not group independent controls or override native operator order. TGPulse delivered MVD Input/H/V/Gravity/Drift/Diagnostics and LCD Display/Position/Opacity as source-backed examples, not Model 3 requirements. Inspect equivalence before adapting.
- Extend existing V2/legacy audits with independently specified expected group sequences and baseline metadata comparison; keep keys/text/values/defaults/visibility unchanged when only ordering is intended. Keep registration and actual menu acceptance distinct.
- Integrate this check with the existing final-registration audit; document justified exceptions and report adoption evidence to Framework. This roadmap scheduling does not start implementation, builds or deployment.


### Final Registered Menu Wording Audit — SM2 Feedback, 2026-10-17

Status: Pending — Review And Acceptance Follow-Up. Reference: [SM2 wording interchange](/Users/andrea/dev/sm2-emu-libretro/Docs/FRAMEWORK_MENU_WORDING_HANDOFF_2026-10-17.md), current Framework F04/F14 and the approved Menu Review. SM2 corrected post-review suffix generation; this does not establish the same defect here.

- Inspect final V2 registrations for every applicable game, including dynamically constructed definitions. Compare complete names/help, raw values, displayed labels, order and defaults against the reviewed contract; counts, templates and common prefixes are insufficient.
- Require one identical full Linked Cabinets help across supported variants within this core, with no per-game suffixes or raw ROM identifiers. Keep topology detail in values/project documentation, distinguish unsupported placeholders and document genuine hardware differences. Compare equivalent cross-core text without copying incompatible hardware claims.
- Scan registered strings for ROM identifiers and stale wording. Review the Crosshair Style lightun typo only if applicable; do not introduce the Model 2-specific Manx TT item into this core. Shared workbook reconciliation is complete: Orders 034/035 and 077/084 updated, obsolete Order 222 physically removed. Use the final 202-row workbook (SHA-256 4ceb9be04d7ad32201c78ae53ac2b103987f8867df69675230996dab7c9c125a), effective Name (New) and Comments; retain Order IDs, not old worksheet row numbers. Do not restore removed definitions from Source Options.
- Apply the [Final Menu Registration Follow-Up](/Users/andrea/dev/libretro-core-framework/TGPULSE_NETWORK_ADOPTION_REVIEW_2026-10-17.md): compare final V2/legacy registration order, supported-pair adjacency and unsupported placeholders. TGPulse fixed equal sort ranks causing two positions to swap; inspect this implementation before assuming the same defect. Require deterministic ordering without unintended changes to modern order, values or defaults. Reuse the existing destination audit, with TGPulse tools/test_libretro_menu_wording.py as a reference; record the final workbook and tested core hashes.
- Verify legacy labels/values where help is unavailable. Reuse existing audits and repeat after changes to option construction. Assertions must encode the reviewed contract rather than preserve generated deviations as expected output.
- Report registration evidence and any required correction to Framework; distinguish it from frontend/gameplay acceptance. This roadmap update does not start implementation, builds, installation or publication.

Model 3 acceptance gate for the pending two-option networking integration: repeat the final-definition audit after replacing old network definitions and offline semantics. The earlier completed menu review does not qualify newly constructed definitions.


| Status | Activity | Scope / Acceptance |
| --- | --- | --- |
| 🟢 | Apply The User-Revised Menu Workbook | Implemented locally for all 37 applicable review rows and 78 source keys, with the two factual exceptions below. Modern and legacy registration checks pass; native per-game NVRAM fields are unchanged. No commit, push or new gameplay qualification. |

Reference mapping: Approved LCF Harmonization canonical-text work and the shared Framework wording review. This is a new wording revision, not evidence
that previous canonical-text work already implements the user's latest edits.
The user has combined formerly separate entries and changed core membership;
preserve those decisions. The workbook's `Source Options` sheet is historical
registration evidence: do not use its original split rows or Order links to
undo the reviewed table. Exclude individual per-game NVRAM operator fields
from harmonization.

`Details` distinguishes global applicability (blank), a group of games or a
specific game. For non-game-specific, non-NVRAM rows, a retained qualifier
after `—` has been moved into the comment as an applicability sentence; the
category remains in Details. Game-specific names already identify their game.
Use the reviewed text, including those applicability sentences, across the
relevant registration paths. Preserve raw option keys, saved values, defaults,
visibility and behavior; a shared label does not authorize changing emulation
or replacing native feedback with synthetic feedback.

Approved consolidation follow-up (2026-10-13): the workbook originally had 203
review entries (202 after the 2026-10-17 Network Board removal). Use Timing / FPS Overlay (Order 012) for all three cores,
including the former Frame Timing Overlay; use Gamepad Rumble (Order 071)
for all three cores, including the former Force Feedback; use one Off-Screen
Reload Shortcut (Order 088) for Model 2 and Model 3. Former Orders 200, 212
and 218 are superseded by these rows. Core flag union is authoritative; do
not recreate the removed rows or alter raw keys/behavior because labels merge.

Single-game core option labels must start with `<Game Name>: `, using exactly
one colon and one following space, then the option description in Title Case.
Apply this to the effective target label: Name (New) when populated, otherwise
Name. Group/global options do not receive a single-game prefix. Per-game NVRAM
operator parameters remain outside cross-core harmonization. Validate this
prefix and the merged labels as part of wording acceptance below.

Acceptance: reconcile every applicable reviewed row against current option
keys, validate modern/legacy labels and help, and inspect the resulting menu.
Record any unresolved mapping without inventing a missing feature.

### Implementation And Acceptance

The implementation initially used 203 populated review rows. The original workbook SHA-256 was
`72b96114c437313fef061c3517a2812daa7563be0e9b15b1d270e14a375e374a`.
All 37 Supermodel/non-NVRAM rows map to 78 current source keys, including the
per-set Linked Cabinets and Automatic Network Settings families. The macOS
build registers 76 of these keys; 3D Renderer and Legacy3D Multi-Texture are
source-covered build-conditional options absent from this build. Historical
Order links are not used to restore the three superseded split rows.

Timing / FPS Overlay, Gamepad Rumble and the single Off-Screen Reload Shortcut
label follow the approved consolidation. Both Star Wars Trilogy controls use
the reviewed game prefix and colon. Existing registration order, categories,
visibility logic, raw keys, choice labels, stored values and defaults remain
unchanged. All 175 registered native per-game NVRAM definitions are identical
to the pre-edit snapshot.

Two factual exceptions preserve the existing implementation:

| Review Order | Option | Applied Exception And Evidence |
| --- | --- | --- |
| 205 | Sound Volume | Help describes SCSP output rather than an overall mixed-output gain. `SoundBoard.cpp` scales SCSP channels before DSB mixing; Music Volume independently controls the MPEG board. |
| 195 | Internal Resolution | Retain this name without Restart Required and explain application on return to the game. `retro_run` updates geometry and calls `LibretroWrapper::UpdateScreenSize`, which reinitializes renderers without restarting content. |

The normal macOS ARM build and the existing 25-symbol format/dependency gate
pass. ROM-free registration checks for Core Options APIs 0/1/2 verify coverage,
labels/help, identical choices/defaults and unchanged native NVRAM fields.
After normalizing only the edited label/help fields, both modified source
files equal their pre-edit snapshots; no emulation or mapping logic changed.

Real RetroArch ARM 1.22.2 / Ozone on Scud visibly shows the updated labels and
help in Video, Input and Audio, including both Star Wars prefixes and the
original three local exceptions (the Rumble exception is superseded below). The dedicated QA app used isolated configuration/save
directories and PID 22906, then exited normally (0). Other chats' frontends
were not controlled or terminated. Screenshots were inspected in the chat,
not saved as local image files. This is menu acceptance, not audio, physical
input or gameplay qualification.

The standard local core/info are updated with backups and matching hashes.
Installed core SHA-256:
`cdd54d18e411aa8532d190935762489914cecc2ccf10b244dd82a756e2f1ffda`.
The external System catalogue, original workbook, shared Framework documents,
personal configuration/remaps and save files are unchanged by this activity.
Evidence: `outputs/menu-wording-20261013/`. No commit or push.

### Gamepad Rumble Comment Follow-Up — 2026-10-13

The user-approved common comment in Menu Review!F98 (Order 071) is now applied
exactly: "Enable gamepad vibration using the game's force-feedback or rumble effects."
This supersedes the previous Rumble wording exception. Choices, defaults and
native feedback behavior are unchanged; the Sound Volume and Internal Resolution
exceptions remain valid. The incremental build, ABI gate and API 0/1/2
registration comparison pass; only the comment differs from the previous build.
The standard local core is updated (SHA-256
`2aaf9f5887e037448e98dd448d36f312fbc9a72d11098ddc1a8f7bc63b054c54`).
No additional frontend/gameplay qualification is claimed.
Verification and delivery evidence are retained in
`outputs/menu-wording-20261013/rumble-followup/`. No commit or push.

## Approved LCF Harmonization — 2026-10-13

Reference: [Libretro Core Framework](/Users/andrea/dev/libretro-core-framework/FRAMEWORK.md),
edition 0.1.4. The user approved the applicable items from the Supermodel
review and requested their registration here. This is the sole implementation
roadmap; the sections below retain detailed provenance for existing items.
Approval and implementation/verification status are distinct.

Legend: 🟢 Completed · 🟡 Partial · ⚪ Not Started · 🔵 Deferred

| Status | Activity | Remaining Work |
| --- | --- | --- |
| 🟡 | F13 — Service Menu Catalogue And Workbook | Complete the existing visual campaign and reconcile photographs, recipes, catalogue and Excel. Retain valid parent/clone samples; keep native labels as evidence and use English/Title Case for presentation. Supplemental binary validation is already complete. See the acquisition section below. |
| 🟡 | F18/F19 — Package Verification And Upstream Versioning | Package assembly/checker, notices, source provenance, checksums and upstream/version ledger implemented. Local macOS assembly passed; Linux/Windows CI and publication remain separate delivery gates. See UPSTREAM_INTEGRATION.md. |
| 🟢 | F05/F12 — Automatic Reset After Effective NVRAM Changes | Implemented: validated changes reset once on Resume; inactive/unchanged settings do not reset. Source/ABI checks confirm persistence and modern/legacy behavior; final frontend delivery is tracked below. |
| 🟢 | F09 — Global Controller Profile Type | Implemented and visually verified in RetroArch ARM: first Input option, full default, global Normal/full changes refresh both port labels and operator assignments while paused. Remapping, None, legacy IDs and operator polling pass focused checks. |
| 🟢 | F09 — Shared Service/Test Order | Align descriptors and native bindings on both ports to L3 Service / R3 Test. Preserve keyboard bindings, native polarity, special commands and saved remaps; document the changed default layout. |
| 🟢 | F10 — Lightgun Standard Arbitration | Implement the already-approved correction described below: constant off-screen status alone must not claim Lightgun ownership. Retain dedicated modes, source scales and The Lost World reload behavior. |
| 🟢 | F14 — Automatic Network Settings | Implement the approved per-set presets and canonical tokens described below, Disabled default and reviewed native roles, coordinated with effective NVRAM application/reset. |
| 🟢 | F21 — Modern RetroArch Notifications | Implemented and verified: extended notifications, absent-interface fallback and rejected-message fallback. Real RetroArch OSD displays the effective NVRAM/reset notification after Resume. |
| 🟢 | F04/F05/F07 — Equivalent Option Names And Help | Reuse canonical text for equivalent initial NVRAM, operator overrides and steering response/output ranges. Adopt the common A/V Timing label while retaining the existing 60 Hz default, raw tokens and Model 3 cadence/lifecycle. Correct overlay help to its actual metrics; retain distinct Sound/Music Volume and native Force Feedback semantics. Check modern and legacy registration. |
| 🟢 | F06 — NVRAM Settings Master Visibility | Implemented and verified through content load/unload callbacks: show only for reviewed implemented fields, preserving per-set gating and saved values. |
| 🔵 | F15 — Multi-Core Launcher | Development belongs to the separate multi-core launcher project. The ANS section below retains the initial handoff as provenance, not authorization to build a second launcher in this core repository. |

### Local Verification And Remaining Delivery

The approved execution order is: effective NVRAM reset; global profile;
Service/Test order; Lightgun arbitration; ANS; notifications; canonical text;
NVRAM master visibility; catalogue/workbook; package/versioning. Table order
above preserves the original framework review grouping.

macOS ARM builds and focused input/native NVRAM checks pass. ANS checks cover
21 reviewed sets and 123 native presets, including integrity, repeat application,
disabled/invalid presets and preservation of unrelated settings. Registration
and notification negotiation were checked with modern and legacy interfaces.
These callback checks do not establish real menu/OSD appearance or gameplay.
RetroArch probes use isolated configuration/save files and windowed mode.
Initial native UI binding encountered timeouts and selected a different open
RetroArch instance. After the user authorized closing all instances, the
windowed isolated session was selected successfully. Visual profile/menu and
notification acceptance are now complete as described below. No commit, push
or release is implied by local status.

### Real Frontend Acceptance And Update-Flag Correction

RetroArch ARM 1.22.2 (`9be7ec92`), Scud, New3D OpenGL 4.1, installed development
core, isolated configuration/save directories, windowed Ozone menu:

- Controller Profile Type is the first Input option; default is Normal +
  Test/Service. Selecting Normal while paused immediately updates the Device
  Type on both ports and clears the L3/R3 operator assignments. Restoring full
  restores Service A/B on L3 and Test A/B on R3 without Resume.
- The first NVRAM change exposed a real update-order bug: GET_VARIABLE clears
  RetroArch's update flag. Polling the profile before GET_VARIABLE_UPDATE, or
  reading options in the paused display callback, lost that signal. The run
  path now checks the flag first, and the display callback retains a pending
  update for Resume. This follows the update-before-read order of SM2-Emu.
  Reference: [RetroArch 1.22.2 runloop.c](https://github.com/libretro/RetroArch/blob/v1.22.2/runloop.c#L1281-L1330).
- Focused checks now reproduce RetroArch's flag-clearing semantics, including
  a paused display callback before Resume, and pass for API versions 0/1/2.
- On the corrected build, enabling NVRAM Settings and changing Difficulty
  from Normal to Hard produces one logged Model 3 reset. The real OSD visibly
  displays `Applied NVRAM Settings; machine reset.`; the attract sequence then
  resumes normally. Audio and physical controller behavior were not tested.

The corrected binary is installed under the user-restored standard target
`supermodel_libretro.dylib`, with matching info, backup and verified hash.
The dev-named core/info are removed; the current policy is in README.md.
Verified binary hash:
`17cdde5e287691f8fae90981bdda1b5ecebccab77ec0f1b5137bbcd1af3fe74c`.
The isolated frontend exits normally (status 0). These visual checks close the
previous F09/F21 acceptance gaps; the photographic catalogue and cross-platform
package delivery remain partial.

The final build also passes save-state reapplication of configured NVRAM
overrides and a single effective reset, across Core Options API versions 0/1/2.
Two bounded real RetroArch ARM runs (300 frames each, Scud) exit successfully
with Quad disabled and enabled; logs confirm New3D OpenGL 4.1 and the selected
Quad mode. Audio was intentionally disabled; these are graphics/startup checks,
not audio or gameplay proof. The final development core is installed as
`supermodel_libretro.dylib`, with matching standard .info and verified configured
support assets (see LOCAL_DEPLOYMENT_CHECK_2026-10-13.md). The former standard
core was replaced after backup, and the dev-named copy was removed.

The final eight-file macOS package passes identity, inventory, XML, notices,
provenance and checksum gates. A deliberately modified shipped file is rejected
by the checksum check. Existing format/dependency and 25-symbol Libretro gates
also pass. Cross-platform CI has not been run or published.

### Catalogue Follow-Up — 2026-10-13

The user-authorized removal of `mgtrkbad` is applied to Games.xml, its generated
DAT, the embedded catalogue and the active external RetroArch System copy.
`magtruck` remains supported; the current inventory is 62 sets. macOS build,
25-symbol ABI/dependency gates and exact XML/embedded-byte agreement pass.
The standard local core/info are deployed with backups and verified hashes;
the current binary is `983e03c1de796f4a9bcad6d0b1cb8c870f86db151acbd387915e38d3806e307e`.
Earlier acceptance and performance hashes remain historical snapshots.

Both the original MAME download and Model 3 working `vs298.zip` contain 39/40
expected files with matching CRCs. `epr-20917.17` still has `C3BBB270`, so the
post-0.289 redump `969E4BDA` remains required. Neither archive was modified.
Evidence: `outputs/catalogue-update-20261013/`. No commit or push.

### Quad Rendering Performance Review

Status: Completed. The user closed the analysis and workaround-test activity
on 2026-10-13. No production performance correction was identified.

The [2026-10-13 performance review](QUAD_RENDERING_PERFORMANCE_2026-10-13.md)
completes the requested Mac analysis and isolated Batocera cross-check. With
Scud at native resolution, Apple M4 Render rises from 1.1 to 26.5 ms with Quad;
Vega 11/Mesa remains at 1.8 ms in both modes. Native repeats and 2x runs retain
the difference. CPU sampling and private arithmetic-reduction probes point to
the Apple geometry-shader draw/driver path, with its private cause unresolved.
This is attract-sequence telemetry, not image-fidelity or full-library proof.
The [instruction/driver follow-up](QUAD_SHADER_INSTRUCTION_REVIEW_2026-10-13.md)
adds matched GPU probes on both hosts and a pristine standalone upstream run.
Targeted `fma`, precision, division/root and indexing substitutions do not explain
the large Mac penalty. The core and pristine upstream both average about 78
microseconds per Quad draw. Splitting draws increases rendering from 27 to 45 ms
with identical submitted vertices; cheaper shaders retain the call cost.
The subsequent Quad/Triangle controls validate 576 samples per host, plus 264
focused depth samples. A pass-through geometry stage preserves triangle pixels
but adds about 9.5 ms CPU-visible time over 128 draws on M4; an empty stage
without `EmitVertex` still costs about 8.8 ms. Neither effect appears on Vega.
Native depth/interpolation dependencies have a secondary Mac-specific scoped
cost, but removing the depth store or replacing its final division does not
explain it. Individual arithmetic changes do not remove the dominant draw cost.
The next isolated workaround checks add 672 CGL worker-mode samples, 384
multi-draw samples and eight fixed-state Scud comparisons. CGL ON returns from
draw calls sooner but moves the wait to the overlay or the remaining renderer;
without the overlay, callback medians remain 28.9/28.3 ms OFF/ON. One multi-draw
call retains the 128-range completion cost (12.26/12.16 ms). Corresponding
synthetic captures match; game-image accuracy is not qualified. Neither test
delivers a practical correction or changes production defaults.
The production core/default and installed artifacts are unchanged. No safe
rendering correction is delivered or replacement renderer approved. Any future
correction needs a demonstrated causal link, equivalent native interpolation
and controlled game-image validation. Framework edition 0.1.9 F17 and M14
record the reusable stage/dependency controls and the later CGL/multi-draw
measurement lessons. The M14 report fingerprint is verified; M11/M12/M13
fingerprints remain historical. This guidance does not automatically change
another core or qualify a production correction.

The workbook retains 208 parent setting rows and 388 clone rows. There are
130 parent rows across 21 titles whose full selectable-value cycle remains
unphotographed; all are explicitly marked `Value Cycle Pending`. This is a
catalogue gap, separate from the completed 461/461 supplemental release-clone
binary campaign. Existing valid samples were retained. The complete clone validator passes 472/472 acquired values across 35 release clones. Prototype acquisition
for `srally2p` and `srally2pa` still does not produce readable Game Assignments.
The current local `vs298` archive contains the old ROM CRC and cannot be used
for fresh acquisition against the corrected catalogue.

Already covered: single-View VR1 on D-Pad Up/Down, driving response/ranges and the
60 Hz timing default. No synthetic cornering rumble or NVRAM-driven native
widescreen mode is introduced. An aspect-ratio policy following the optional
widescreen enhancement would be a separate proposal.

## Upstream Integration Review — 2026-10-13

Reviewed upstream: `trzy/Supermodel` master `ed608de` (2026-10-12).
The Libretro master branch has no new pending change; the MinGW build fix
also published on its dev branch as `f2e1e7d` is already integrated.

- Completed locally: adapt the native Quad shader from `7d3b6f4` (PR #339) to
  OpenGL/GLSL 4.1 and request that context in the Libretro adapter. Preserve
  the published Quad Rendering default and Legacy3D/GLES behavior. Validate
  the build and real frontend shader initialization separately from gameplay.
- Implemented: adopt the corrected `vs298` ROM CRC from `b5125ca` (PR #327)
  in Games.xml and its DAT, including the regenerated embedded catalogue.
- Optional, deferred: GUI/configuration changes from PR #336 (`90be175`,
  `4154cdb`, `9175f97`) and PR #340 (`44404c2`). Review preservation of
  per-game INI sections, readable value labels and custom-value retention
  only if a Supermodel-owned GUI/configuration writer needs them. The
  Libretro core currently reads user INI overrides without rewriting them.
  Shared configuration helpers and mechanical cleanups remain part of this
  deferred review; do not import GUI policy into RetroArch Core Options.

This review records selective coverage and does not claim that the full
standalone upstream revision has been integrated.

Validation: clean macOS ARM64 build; 25 Libretro ABI exports and dependency
checks; shader body compared with pristine `7d3b6f4`, retaining the port's
central GLSL prefix; XML/DAT CRC and embedded-catalogue integrity checks.
RetroArch 1.22.2 on Apple M4 ran Scud Race for 300 frames with Quad disabled
and enabled, requesting and receiving OpenGL 4.1, with clean exit and no
shader compilation/link errors. The native Quad option is now also compiled
on macOS. This is startup/shader evidence, not a full visual/gameplay or audio
qualification; audio output was excluded from the targeted probe.

The local vs298 ZIP still contains CRC `C3BBB270`. The updated loader correctly
reports the missing `969E4BDA` ROM, so corrected-dump gameplay remains
unverified. Existing ROMs were not modified.

The existing RetroArch Development core/info and local Games.xml were updated
with backups and verified binary SHA-256. Retained private build/frontend
evidence: `/private/tmp/supermodel-upstream-20261013-final/` and
`/private/tmp/supermodel-upstream-20261013-build.log`. No commit, push or release
was performed.

## Authoritative Game Settings acquisition and NVRAM audit

Status: visual catalog in progress; supplemental NVRAM validation complete.
All 22 parent sets have been acquired and matched against their existing
recipes; 38/40 clones have direct structural evidence. The two Sega Rally 2
prototypes fail before reaching the Service Menu. Current coverage and findings are tracked in
the catalogue and audit source notes in the
[current campaign workbook](Service_Menu_NVRAM_Summary.xlsx).
The Model 3 workbook
originally contained separate `Cloni` and `Copertura cloni` worksheets derived
only after direct clone menu acquisition and complete NVRAM value validation.
Their records are now preserved in the [common summary](Service_Menu_NVRAM_Summary.xlsx)
under Core options and Campaign; this section retains the historical status.

Prototype and location-test sets are part of the campaign whenever they can be
run. Their status is never an exclusion criterion. The known bad dump `mgtrkbad`
was removed from the core catalogue by user request; `srally2p` and `srally2pa` remain
open acquisition failures rather than exclusions.

This applies to the authoritative visual catalog. Prototype, location-test and
other pre-release sets are deliberately excluded from Libretro NVRAM
management, from automatic initialization and from the supplemental binary
campaign.

Run a Service Menu acquisition campaign for every runnable ROM set in
`Games.xml` and catalog every setting available for each title. The original
inventory had 63 sets including the excluded bad dump `mgtrkbad`; its removal
leaves the same 62 campaign sets: 22 parents and 40 clones. The catalog is not
limited to settings already exposed by the core.

The narrower initial scope applies only to supplemental samples and binary
validation. Reuse the tested parent recipes and their existing `.srm` samples
as the starting point for the settings currently exposed as Libretro `NVRAM
Settings`; generate a new sample only when an exposed photographed value has no
corresponding recipe/sample, when the existing artifact is not verifiable, or
when the comparison finds a discrepancy. That implementation currently covers
56 sets: 22 parents and 34 clones. Settings found in the Service Menu but not
yet exposed must still be recorded in the catalog and identified as
implementation gaps; they do not require supplemental `.srm` samples during
this first campaign.

The supplemental clone campaign is complete for all 34 released clones managed
by the core: 461/461 values were acquired from clone-specific baselines and
matched uniquely with valid structure, integrity, and persistence. Direct
evidence removed the inherited Asia value from `vf3a` and changed `dayto2pe`
Cabinet Type from Twin/Deluxe to Twin/Special with its clone-specific Backup
RAM route. Prototype, location-test, and other pre-release sets remain outside
this binary NVRAM scope.

The campaign must produce two repository documents modelled after the SM2-Emu
reference work:

- an authoritative catalogue, now preserved in the [workbook source notes](Service_Menu_NVRAM_Summary.xlsx), derived from Service Menu
  screenshots and recording every available setting's visible label, relative
  order, native default, and every observed selectable value for each set;
- an audit, now preserved in the [workbook source notes](Service_Menu_NVRAM_Summary.xlsx), comparing that catalog with
  `LibretroNvramSettings.h`, including option presence, order, labels, values,
  defaults, encoded values, integrity updates, template routing, and clone
  routing. Settings present in the catalog but absent from the implementation
  must be listed explicitly without expanding the initial sample scope.

### Acquisition rules

1. Build the full catalog inventory from every set in `Games.xml`. Build a
   separate supplemental-sample inventory from the NVRAM options registered by
   the core at the start of the campaign. Exclude known bad dumps explicitly
   rather than treating them as missing evidence. Record the exact core revision and
   `Games.xml` revision used for the evidence.
2. Acquire a clean native baseline for every supported set with `Automatic
   Initial NVRAM Setup` and `NVRAM Settings` disabled. Each option cycle must
   restart from that immutable baseline rather than from the preceding sample.
3. Capture every Game Settings page and enumerate every distinct selectable
   value for every setting. Screenshots are authoritative for visible presence,
   order, labels, values, and native defaults; implementation tables, TOML
   recipes, and parent metadata are secondary evidence.
4. Keep ROMs, generated Save RAM, raw screenshots, logs, and temporary frontend
   profiles outside Git. Commit the reproducible campaign definitions, the
   indexed catalog, the audit, and validation tooling.
5. For settings in the supplemental-sample inventory, first match every
   photographed value to the current recipe suffix and existing `.srm` sample.
   Reuse that sample to confirm byte encoding, checksum or mirror update, clean
   import, and persistence after a second launch. Generate a replacement or
   additional sample only for a demonstrated gap. Catalog-only settings do not
   require generated samples in this first campaign.

### Parent and clone criteria

- Acquire every clone directly. A `parent` relationship in `Games.xml` does not
  prove that labels, order, values, defaults, offsets, checksums, or storage
  layout are compatible.
- A clone may share a parent catalog only after its own evidence confirms the
  same complete visible menu. For settings in the supplemental-sample
  inventory, every exposed value must also be written and persisted with the
  parent encoding rules.
- The 38 clone `menu-structure.srm` files are native baselines used to document
  menu structure and defaults. They are not per-value samples and do not, by
  themselves, complete the supplemental clone campaign.
- A byte-identical native baseline alone is insufficient to authorize catalog
  inheritance. Conversely, a different baseline does not require a different
  catalog when direct menu acquisition and complete write validation prove the
  same option semantics.
- Any clone difference produces a clone-specific catalog or routing rule and is
  recorded in a parent/clone comparison matrix. No unacquired value is inferred
  from the parent.
- Derive clone diagnostic definitions and sampling recipes from the parent only
  after the clone's own Test Menu and Game Settings pages have established its
  visible rows, order, and defaults. Use the tested parent recipe as navigation
  scaffolding, not as evidence of clone equivalence.

### Default and override policy

Align Supermodel with the SM2-Emu distinction between native defaults and
deliberate policy overrides:

- the native default is the value visible in the clean Service Menu baseline;
- ordinary Core Option defaults must match that native value unless the setting
  is listed as a reviewed policy override;
- policy overrides are limited to region selection (Export when available,
  otherwise USA), a safe offline or stand-alone network state, and any
  Supermodel-specific cabinet or I/O choice that is explicitly approved and
  documented;
- the current Star Wars Trilogy Upright initialization and every other existing
  non-native default must be reclassified from evidence as either a reviewed
  override or a defect; undocumented exceptions are not retained;
- overrides apply only through the documented new-save initialization and
  NVRAM Settings paths. Existing saves are never rewritten implicitly;
- automated checks must compare ordinary defaults with the native catalog and
  test the reviewed override list separately, including its rationale and
  affected sets.

### Completion criteria

- 100% of the 62 runnable sets and every available setting have indexed Service Menu
  evidence, including every clone;
- every currently exposed NVRAM option is accounted for, and no unreviewed
  option or value remains exposed;
- every catalog setting absent from the implementation is listed explicitly;
- for the currently exposed subset, catalog and implementation agree on
  presence, order, labels, accepted values, native defaults, encodings, and
  integrity handling;
- every clone route is supported by clone-specific evidence;
- native defaults and deliberate overrides are reported separately and covered
  by automated checks;
- clean initialization, option application, save import, and persistence pass
  for every set and setting in the supplemental-sample inventory before the
  audit is marked complete.

## Network Board And Linked Cabinets Loopback Convention

Status: implemented and locally delivered on 2026-10-14. Disabled now provides
local loopback with a present board; board disconnection hides Linked Cabinets
and ANS while preserving selections. Offline native settings take precedence
with NVRAM Settings enabled; the retained linked preset applies at totals >= 2.
Published defaults and numeric per-title choices remain unchanged.

Changes apply automatically on Resume with one reset for effective board,
topology and/or NVRAM changes; unchanged Resume does not reset. Build, ABI,
modern/legacy registration, 21-set offline mapping, device echo and real macOS
frontend checks pass. Scud/Le Mans attract and hide/restore are verified. Full
catalogue/topology and synchronized multiplayer qualification remain separate.
See [integration and evidence](NETWORK_INTEGRATION_2026-10-14.md) and the retained
[feasibility study](NETWORK_LOOPBACK_FEASIBILITY_2026-10-13.md). No commit or push.

## Automatic Network Settings alignment

Status: implemented; native preset and ABI checks complete. Supermodel already
registers per-set `Linked Cabinets` transport options and exposes reviewed
native Link Mode / Link ID fields through `NVRAM Settings`, and now provides the shared `Automatic Network Settings` selector. This is a new
frontend feature, not a rename of an existing option. This entry is reconciled
with Libretro Core Framework edition 0.1.1 contracts F14 and F15; the retained
implementation provenance is
`/Users/andrea/dev/sm2-emu-libretro/Docs/ANS_SUPERMODEL_INTERCHANGE_2026-10-09.md`.

Use the exact label `Automatic Network Settings`, with per-set keys named
`supermodel_automatic_network_<ROM set>`, defaulting to raw value `0`. The
shared cross-core convention is:

| Raw value | Meaning |
| --- | --- |
| `0` | Disabled; preserve and apply the existing manual NVRAM settings |
| `1` | Master / First Cabinet |
| `2` | First Slave / Second Cabinet |
| `3`... | Subsequent Slaves / sequential cabinet identities |
| `relay` | Relay / Live Monitor, only where the native mapping is supported and reviewed |

Value labels use Title Case. Register only roles established for that ROM set,
including decimal cabinet identities such as `10` and `16` where supported.
`relay` is the only special-role token: do not introduce `none`, `100`, `live`
or a second raw-value convention. Do not equate Satellite with Relay without
specific Model 3 evidence, and do not derive the native cabinet role from the
frontend's host/client status.

Active ANS presets must map explicitly to each game's reviewed native network
fields and apply through the existing enabled `NVRAM Settings` lifecycle.
Disabled must leave manual choices effective. Existing save/template
precedence, unrelated NVRAM fields, restart/apply behavior, published manual
keys and their values remain unchanged. Because this is a new option, no legacy
ANS alias or migration is required.

The corresponding multi-instance launcher contract is:

```sh
--game gamename
--game gamename1,gamename2
--game gamename:1,gamename:2
--game gamename:1,gamename:2,gamename:relay
```

A single name repeats the ROM for every instance; a comma-separated list
selects each instance's ROM. A `:ANS` suffix selects that instance's preset and
never changes the ROM name. Explicit suffixes override saved values. Each
instance receives an isolated controller, configuration and save environment,
using the project's established process lifecycle. The launcher performs no
game compatibility, native program combination or cabinet-limit validation,
does not add a separate role argument, and does not encode role semantics in
frontend host/client selection.

Implementation acceptance requires registration and mapping checks across the
linked-game catalog, including two-digit cabinet IDs, disabled/manual behavior,
reviewed Relay support and unchanged unrelated NVRAM. Add one necessary bounded
frontend probe with the existing runner and report it separately from gameplay
proof. After validation, update the user documentation and deploy the normal
development core; commit and push remain separately authorized steps.

## Cross-core VR1 supplemental binding review

Status: implemented locally after the user's 2026-10-13 clarification. The
earlier review checked only Up and incorrectly treated that as full alignment.
The implemented SM2 reference and the user-approved convention require both
Up and Down on the same View action where neither has an independent function.
The retained provenance is
`/Users/andrea/dev/sm2-emu-libretro/Docs/VR1_VIEW_BINDING_INTERCHANGE_2026-10-11.md`.

Supermodel now maps the single View/VR1 action to both D-Pad Up and Down, with
matching VR1 descriptors in RetroArch Controls, while preserving
its existing keyboard binding. This covers `dirtdvls`, `dirtdvlsu`,
`dirtdvlsau`, `dirtdvlsj`, `dirtdvlsg`, `eca`, `ecap`, `ecau`, `ecaj`,
`harley`, `harleya`, `srally2`, `srally2p`, `srally2pa`, and `srally2dx`.
No current single-View profile assigns either direction to a separate native
game action. Existing keyboard bindings and saved frontend remaps are retained.

The four-button View profiles remain unchanged: D-Pad Up/Down/Left/Right map
to VR1/VR2/VR3/VR4 respectively for `daytona2`, `dayto2pe`, `lemans24`,
`scud`, `scudau`, `scuddx`, `scuddxo`, `scudplus`, and `scudplusa`. Future
single-View profiles must reuse the existing View action and both directional
aliases after reviewing independent Up/Down game functions; this convention
does not authorize changes to VR2 or VR4 mappings.

## Cross-core Lightgun Standard source arbitration

Status: implemented; focused input checks complete. This entry is
reconciled with Libretro Core Framework edition 0.1.1 contract F10. The
retained provenance is
`/Users/andrea/dev/sm2-emu-libretro/Docs/LIGHTGUN_STANDARD_INTERCHANGE_2026-10-11.md`.

Supermodel's `Gun Input Mode` already defaults to `Standard` and combines
Lightgun, Mouse and Analog Stick through one virtual cursor. Its current source
arbitration nevertheless treats a reported `IS_OFFSCREEN` state by itself as
Lightgun activity. A frontend that reports this state continuously can
therefore keep selecting the Lightgun path without valid absolute coordinates
and suppress otherwise valid Mouse or stick movement. This gap applies to the
Gun profiles used by `lamachin`, `lostwsga`, `lostwsgp`, `oceanhun`, and
`oceanhuna`.

Align `Standard` with the shared contract: select Lightgun aim from valid
coordinate movement or an actual Trigger, Reload, or Aux command, then apply
its off-screen status when that source owns the cursor. A constant off-screen
report alone must not claim source ownership. Preserve the existing absolute
coordinate scale, relative Mouse cursor, stick response, per-player routing,
secondary-shot rules, The Lost World physical and shortcut reload paths, and
the independent optional Mouse-edge reload behavior. `Lightgun Only` remains
restricted to Lightgun callbacks and receives no fallback.

Star Wars Trilogy's analogous `Standard` source selection already ignores
off-screen state when choosing Lightgun ownership; its analog-yoke protocol is
outside this integration and remains unchanged. Acceptance requires bounded
source-arbitration checks for constant off-screen plus moving Mouse and stick,
valid Lightgun movement, Lightgun commands, and The Lost World off-screen
reload, without copying Model 2 hardware mappings or treating frontend status
as physical-device detection.

## Cross-core synthetic cornering rumble review

Status: not applicable. This result is reconciled with Libretro Core Framework
edition 0.1.1 contract F11. The retained provenance is
`/Users/andrea/dev/sm2-emu-libretro/Docs/RUMBLE_FRONTEND_DEADZONE_INTERCHANGE_2026-10-11.md`.

Supermodel does not synthesize cornering vibration from the frontend steering
axis. Its Libretro adapter translates the emulated drive board's native
Constant Force, Self Center, Friction and Vibrate commands to the frontend
rumble motors. Although the steering input passes through Supermodel's normal
axis deadzone and scaling before the game reads it, that processed value is not
used to calculate an additional cornering-rumble curve or centre threshold.

Do not add the SM2 synthetic effect to Supermodel. Preserve the native drive
board commands, their force values and thresholds, the existing strong/weak
motor mapping, the Force Feedback option, and lifecycle stop behavior. If a
future Supermodel-specific synthetic steering effect is proposed, this
frontend-deadzone convention must be reviewed before implementation; it does
not authorize a new effect.

## Cross-core widescreen assessment

Status: complete. No Model 3 title is known to have a native widescreen
presentation.

The Model 3 video path has a common 496x384 visible area. Supermodel identifies
496/384 as the Model 3 aspect ratio, defaults `WideScreen` to disabled, and
describes the optional implementation as a wide-screen hack. MAME likewise
configures the Model 3 driver with a 496x384 visible area and has no per-game
aspect-ratio override. The documented Twin and Deluxe cabinets use larger CRT
or projection displays, but no discovered game or cabinet setting selects a
native 16:9 presentation.

SM2-Emu's `Aspect Ratio` therefore has no Model 3 counterpart to adopt. That
option keeps the emulated 496x384 framebuffer unchanged and, in `Auto`, reports
4:3 or 16:9 from the active Indy 500/STCC cabinet setting in NVRAM.
Supermodel's `Widescreen Mode` instead expands the rendered 3D field of view,
changes the framebuffer width, and can optionally extend the lower 2D
background. It remains a separate, explicit opt-in emulator enhancement and is
never enabled automatically from a game or cabinet label.

The Game Settings acquisition campaign must still record any Cabinet, Display
Type, or equivalent setting it encounters. New primary evidence of a native
Model 3 widescreen mode would reopen this assessment before any automatic or
NVRAM-driven aspect policy is added.
