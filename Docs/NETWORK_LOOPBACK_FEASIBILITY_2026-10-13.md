# Network Board And Local Loopback Feasibility

Implementation follow-up: [the integrated local version](NETWORK_INTEGRATION_2026-10-14.md) supersedes the pending-delivery statements in this historical study.

Date: 2026-10-13. Scope: the user's approved Framework contract and isolated
Supermodel feasibility trials. No production implementation, deployment,
commit or push is included. The original Master-mode frontend trials below
are historical non-contract trials; corrected offline trials supersede their
game-compatibility conclusion.

## Approved Contract

Libretro Core Framework edition 0.1.13 introduced the target convention;
editions 0.1.14/0.1.15 added manual-role and visibility rules; edition 0.1.16
corrects the ANS precedence described below. Edition 0.1.17 records the
corrected Scud/Le Mans startup evidence:

| Network Board | Linked Cabinets | Target Behavior |
| --- | --- | --- |
| Disabled | Inactive | Communication board absent/disconnected from the emulated machine. |
| Enabled | Disabled | Board present with a local one-node loopback; no external peers. |
| Enabled | Supported Total | Board present with the specified linked topology. |

`Disabled` in Linked Cabinets means local loopback, not absent hardware.
This is a target contract with per-core adaptations; it does not claim that
all cores already implement these states. Totals must follow verified native
game topologies, with playable cabinets and supported Relay/Satellite roles
counted explicitly. ANS/native NVRAM roles remain separate from topology.
Published defaults and saved selections are not changed by this study.

The user additionally requires Network Board Disabled to hide both Linked
Cabinets and ANS, retaining their saved values. Enabled restores applicable
per-title controls; ANS still requires NVRAM Settings. Hiding a saved active ANS
preset does not disable its responsibility to prepare offline native settings
for absent hardware. This visibility rule is approved but not implemented in
the production core or the feasibility prototype; their existing gates depend
on the loaded title/NVRAM Settings rather than board presence.


Framework: `/Users/andrea/dev/libretro-core-framework/FRAMEWORK.md`, section
6, F14 Board Presence And Local Loopback. The workbook is unchanged.

## Approved Menu Help Follow-Up — 2026-10-14

Apply the following common help together with the approved networking behavior;
these are target texts, not proof of delivery in the installed core.

- **Automatic Network Settings:** Automatically maintain native network-role and cabinet-identity settings through reviewed NVRAM presets. When Linked Cabinets is Disabled, the selected preset is ignored and the game uses Standalone mode. The selection is retained and applies when Linked Cabinets is set to two or more cabinets.
- **Linked Cabinets:** Set the total number of cabinets participating in the emulated network link, including a relay where supported. Disabled connects this cabinet in local loopback, without linking to other cabinets.

The ANS choice is preserved while inactive in local mode; returning to supported
linked totals restores its applicability. No saved token/default is changed by
this wording update. The current production core lacks Disabled/local-loopback,
so its menu help is not changed ahead of implementation.

## Existing Supermodel Boundary

The Libretro board is already a simulation. Network Board determines
`IsAttached`; linked totals currently start at two. `configured_network_cabinets`,
`LibretroWrapper::SetNetworkCabinets` and board initialization enforce this
minimum. The handshake/exchange then depends on an active Netpacket session.
Normal standalone gameplay works using native offline settings; it is distinct
from a present board completing a one-node ring as Master.

The standalone upstream simulator already explicitly permits a zero-peer ring
when incoming/outgoing ports match and AddressOut is 127.0.0.1
(`Src/Network/SimNetBoard.cpp`, Type 1 handshake). The feasibility prototype
reuses the Libretro board's status/header and bank-swap logic; no standalone
sockets, board firmware, CPU or renderer changes are introduced.

## Isolated Prototype

A copy of the current working sources and existing build objects lives in
`/private/tmp/supermodel-loopback-20261013/`. The prototype adds a Disabled
choice to per-set Linked Cabinets, maps it internally to one node, relaxes
only the adapter's minimum-total clamps, completes the existing handshake
locally, and copies the local segment back before the normal bank swap.
Network Board continues to control attachment. Numeric totals and default 2
are retained in this prototype. A later isolated correction makes offline
topology take precedence over the saved ANS role when applying reviewed native
network fields; linked totals of at least two retain the existing ANS role path.

Only three adapter files differ in the copy: libretro.cpp, LibretroWrapper.cpp
and LibretroNetBoard.cpp. Patches, original source hashes and executable hashes
are retained under `outputs/network-loopback-20261013/`. The production source
files and installed standard core are unchanged. The installed core SHA-256
remains `2aaf9f5887e037448e98dd448d36f312fbc9a72d11098ddc1a8f7bc63b054c54`.
The normal macOS ARM build and existing 25-symbol ABI/dependency gate pass for
the prototype.

## Device And Frontend Evidence

The focused board probe drives public hardware registers and shared RAM,
without a frontend Netpacket session. Scud (Type 1), Le Mans (Type 2), and Dirt
Devils (Type 2) each complete a Master one-node handshake and 100 payload
loopback exchanges. Disabling the board leaves it unattached in all three.
One-node Slave requests are explicitly rejected for Scud and Le Mans; no
silent native-role rewrite is performed. The unmodified board stays waiting
when asked for one node, because it clamps the total to two.

Real ROM trials use RetroArch ARM 1.22.2 (9be7ec92), Apple M4, OpenGL 4.1,
windowed operation and the existing isolated smoke-runner environment setup.
Each owns configuration, global core options, save/state paths, System copies,
logs, screenshots and its process. NVRAM Settings and ANS 1 explicitly select
Master; no external Netplay host/client is started. Quad Rendering is disabled.

| Historical Master-Mode Trial (Not The Approved ANS Convention) | Result At 1,200 Frontend Frames |
| --- | --- |
| Scud, production board enabled, total 2, Master without peers | Remains at NETWORK CHECKING / THIS IS MASTER CONTROLLER. |
| Scud, prototype board enabled, Disabled/local loopback, Master | Logs cabinet 1 of 1 and the first completed local exchange; reaches the 3D attract demo / INSERT COINS. |
| Le Mans, prototype board enabled, Disabled/local loopback, Master | Logs cabinet 1 of 1 and first local exchange; diagnostic screen reports COMM NODE 0001 / NODE ID 0001. This alone is not successful game startup. |
| Scud, prototype board disabled, Master | Reports CANCELLED / NETWORK BOARD NOT PRESENT; confirms the distinction from board-present loopback. |

The extended Le Mans prototype run reaches 3,600 frontend frames (~62 seconds)
with no crash and a completed board handshake, but the final screenshot still
shows CHECKING NETWORK. A separate production Master run stays on the COMM
diagnostic page at 1,200 frames. These Master trials used the wrong ANS/native mode for the approved Disabled
convention and do not establish a limitation of that convention. The generic
board echo remains a separate technical capability test.
Both extended/comparison runs exit normally (0).

All four corrected trials exit normally (0), save screenshots and leave their
owned processes stopped. CUA inspected the owned window; final PNGs were also
read from disk. Initial four attempts are retained but excluded: RetroArch's
per-core option mode did not read the prepared global option file. The corrected
runner explicitly sets global_core_options and an isolated rgui_config_directory.

## Corrected ANS Precedence And Frontend Results

The user corrected the erroneous interpretation: with Linked Cabinets Disabled,
ANS prepares the title's native Standalone/Single/No Link settings regardless of
the retained linked role. That selected role becomes applicable only with a
supported linked total of at least two. Linked Cabinets controls topology;
Network Board still controls attachment. The original Master trial was not the
convention requested by the user.

The isolated correction uses the existing reviewed network defaults, changing
only Link Mode/Link ID while preserving the retained ANS selection, all other
native fields and the existing NVRAM Settings gate. It introduces no country,
calibration or unrelated settings rewrite.

| Corrected Trial | Result At 1,200 Frontend Frames |
| --- | --- |
| Le Mans, board present, Linked Cabinets Disabled, retained ANS 2 (Slave) | Native No Link takes precedence; reaches the rendered attract sequence rather than CHECKING NETWORK. |
| Scud, board present, Linked Cabinets Disabled, retained ANS 2 (Slave) | Native No Link takes precedence; reaches the 3D attract demo / INSERT COINS. |
| Le Mans, board present, Linked Cabinets Disabled, ANS 0 and initial No Link defaults | Additional offline startup control; reaches the attract sequence. This seeded control does not qualify preservation of an explicitly different manual NVRAM setting. |

All three corrected-ANS trials exit normally (0). Saved option files retain
the selected values. Screenshots were saved and inspected. Their logs verify
board attachment, one-node configuration and application of the offline native
preset. Native No Link may avoid starting the game's link exchange; successful
offline boot is not itself an active-echo claim. The independent device probe
establishes the local echo mechanism.

## Limits And Remaining Integration

Feasibility is positive for the local board mechanism and offline boot in Scud
and Le Mans under the agreed ANS precedence. The earlier claim that Le Mans
required special adaptation based on its Master one-node trial is withdrawn.
This is not full-catalogue support or a completed race. Dirt Devils is only
device-probed here.

Full delivery still requires per-title reviewed offline/linked mappings,
production visibility and lifecycle integration, local-mode independence from
an active frontend Netpacket session, native integrity and reset/reload checks,
and supported linked totals. Manual controls and inactive NVRAM Settings require
separate preservation checks. Do not silently rewrite settings outside the
approved automation or generalize one game's results to all titles. No
multiplayer, physical input, complete race, reset/save-state or Windows/Linux
proof is claimed. Production source/core remain unchanged; this is a prototype.

Reproduction: prepare.py records the isolated source copy and changes;
board-probe.cpp exercises the device; run-frontend-r2.py performs the corrected
four frontend trials; run-frontend-r3.py retains the non-contract Master comparison;
run-frontend-corrected-ans.py uses the corrected offline native settings;
verify.py checks source/core preservation and retained logs. Source patches and
all raw logs/results are dated study artifacts, not permanent Makefile checks.
