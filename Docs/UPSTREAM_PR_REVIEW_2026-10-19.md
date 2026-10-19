# Upstream Review And Proposed Libretro Pull Request

Review Date: 2026-10-19

## Requested Outcome And Current Status

Check both upstreams for new work and propose this fork's integrations upstream.
The target is `libretro/Libretro-Supermodel:master`, the direct parent of the
fork. The user approved publication from the existing `main`, including the
project documentation and campaign archive already maintained in the fork.
[PR #20](https://github.com/libretro/Libretro-Supermodel/pull/20) is open from
`Zer0one:main` to `master`. The implementation commit is
`36a105e218c2245cad0fed75e4b13be516216c52`; the roadmap follow-up is documentation
only. No new branch or standalone upstream import was made.

The earlier contribution, [PR #13](https://github.com/libretro/Libretro-Supermodel/pull/13),
is merged. No other open PR by Zer0one was found before #20 was created.

## Fresh Upstream Check

Both remotes were fetched on the review date. Source comparisons use fetched Git
objects, and GitHub was checked for existing PRs and the published-head comparison.

| Repository | Fetched Tip | Result |
| --- | --- | --- |
| Libretro upstream | `84bc106b45b279bf868a53b9232897c8dc10ca17` | No commits missing from local `main`. The fork had seventeen additional published commits before this submission. |
| Standalone upstream | `ddbc3066e077e4c99581d2ab7d3452fd1a603723` | Twenty-five commits, including sixteen non-merge commits, after the previously reviewed `ed608de1`. Changes below are newly reviewed, not integrated. |
| Fork published head | `0d160a80971bc2b889a5e2ca44e51fa9f4c1333a` | Pre-submission baseline; superseded by `36a105e2`, which contains the Scud Australia and rumble corrections. |

The release identity remains based on the selectively integrated `ed608de`
snapshot. Fetching a newer tag does not change the built core's version or scope.

## Standalone Changes Relevant To This Proposal

| Change | Assessment |
| --- | --- |
| [`0a99174d`](https://github.com/trzy/Supermodel/commit/0a99174dff22256678301997c90c8a7857a79309): four force-feedback effects | Fixes the native DirectInput effect-array size from three to four. The Libretro adapter uses its own four-effect storage and does not build DirectInput. This is distinct from the adapter's overlapping-effect bug. |
| [`beb421ed`](https://github.com/trzy/Supermodel/commit/beb421ed040a2f1f654ddc9703f5dc9ac8dd5941): wheel servo registers | Relevant native Scud/Daytona/Rally corrections: nibble order, motor gain, idle stop and final per-frame force publication. Our output mixing/mute fix is complementary and does not replace these native changes. Pending separate integration and runtime verification. |
| [`175add57`](https://github.com/trzy/Supermodel/commit/175add57): drive command queue | Preserves successive main-board commands for the Z80 across frames, with reset/load handling. Separate native emulation work; not included in this outbound frontend proposal. |
| [`70b1b316`](https://github.com/trzy/Supermodel/commit/70b1b316): PowerPC fixes | Arithmetic, shifts, carry/exception and instruction correctness changes in `ppc_ops.c`. No equivalent import is claimed by this proposal. |
| [`5dbc97fa`](https://github.com/trzy/Supermodel/commit/5dbc97fa), `3574db93`, `53ff7674`: Quad triangles | Fix slivers and refine exact-triangle detection. These are newer than our OpenGL 4.1 adaptation and require a separate renderer integration review. |
| `4f5d686b`, `a8297d4c`, `ddbc3066`: Pro-1000 / Real3D / SCSI | Large new hardware and shared-renderer changes. Importing them would expand this proposal substantially; not included. Shared interfaces must be reconciled when later integrating them. |
| ROM scanning, GUI calibration/DPI and related helpers | Standalone frontend work; does not supersede RetroArch-owned UI. Keep optional assessment separate from core fixes. |

The [open SCSP proposal #349](https://github.com/trzy/Supermodel/pull/349) and
[OpenGL ES/Wayland proposal #332](https://github.com/trzy/Supermodel/pull/332)
are not merged into the fetched standalone tip. They are not represented as
delivered updates.

## Proposed PR Content

Proposed Title: **Update Core Options, Native Networking And NVRAM Management**

The proposal covers the post-#13 runtime integration, including current local
fixes. It does not resubmit #13 or label native changes as Libretro inventions.

| Area | Concrete Behavior |
| --- | --- |
| Controller profiles and guns | Per-title labels and secondary actions; mouse/analog preservation; optional reload shortcut. |
| Native networking | Per-title cabinet totals, transport/native-role separation and established reset lifecycle. No maximum-cabinet or all-game racing qualification is claimed. |
| NVRAM | Opt-in overrides and validated per-set seeds; existing-save precedence; clone mappings, including Scud Australia Normal/Twin controls. |
| Presentation | Core Aspect Ratio selector, live Widescreen Mode application, texture/register restoration after renderer recreation. |
| Rumble | Independent effects, ports and axes; output mute independent of the emulated board; current-effect restoration without reset/reload. No five-percent threshold. |
| Audio and frontend lifecycle | Existing reset/timing corrections and effective option changes. |
| Build and packages | Existing desktop matrix, runtime metadata checks, support-file inventory, checksums and clean-source provenance. |

The published PR contains the accumulated difference from Libretro master,
including production source, catalogue, Makefile, CI/package code and supporting
project documentation, campaign scripts/YAML and the summary workbook. Raw ROMs,
SRM samples, photographs, personal configurations and local evidence outputs are
not committed. The earlier local 31-file subset is historical preparation only;
it is not the published PR scope.

## Validation And Submission Boundaries

- Published source: four desktop build/package jobs passed CI run 39164917017.
- Later local source: macOS ARM build/ABI, NVRAM validation and Scud rumble
  lifecycle probes pass. The old CI run must not be attributed to this newer tree.
- Rumble: held-effect mute/restore verified through Libretro callbacks; no physical
  controller response claim. Fixed-frame comparisons preserve the emulated scene.
- NVRAM: 478 acquired values across 35 release clones validate; Scud Australia has
  seventeen native-committed samples with fresh RetroArch reloads.
- No new upstream gameplay qualification, full standalone merge, or current-tip
  synchronization is implied.

The four-platform CI for implementation commit `36a105e2` is
[run 39173389437](https://github.com/Zer0one/Libretro-Supermodel/actions/runs/39173389437).
It was in progress when the PR was opened; do not substitute the earlier release
run for its result. Existing fork-specific release links and provenance identify
the actual contributing source, not an upstream-issued release.

The newly reviewed standalone updates are explicitly pending in roadmap items
U01-U05. No fetched snapshot is represented as incorporated by this PR.

## Standalone Outbound Candidate

The renderer-reattachment restoration remains a separate possible contribution
to `trzy/Supermodel`. The new upstream `AttachRenderer` implementation still does
not contain our texture upload/register restoration sequence, but its interfaces
changed with Pro-1000. Our fix must be adapted and verified on pristine current
standalone source before it can be presented as a ready standalone PR. The
Libretro rumble adapter is not a standalone patch.

## Published Review Materials

- [Pull Request #20](https://github.com/libretro/Libretro-Supermodel/pull/20).
- The GitHub Files Changed tab is authoritative for the current published scope.
- Public-facing PR body: `outputs/upstream-pr-20261019/pull-request.md`.
- Local pre-submission subset and manifest in that output directory are retained
  as historical preparation and are superseded by the published comparison.
