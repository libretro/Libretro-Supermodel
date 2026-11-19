# Two-Option Network Integration — 2026-10-17

## Scope And References

User-authorized implementation of Framework 0.1.30 F05/F14 and the current
single-sheet Network Review. Reuses the existing Supermodel adapter/runners,
SM2 native authority/handover and transport setup, and TGPulse lifecycle findings.
No commit, push or release. Existing unrelated working-tree changes are retained.
Prototype exclusions remain unchanged.

Authoritative workbook hashes:

- Network Review: `a41a07028027f1e84422cdee7c6e597ef89cc424b96432a45a646cd051de1a59`.
- Menu Review: `4ceb9be04d7ad32201c78ae53ac2b103987f8867df69675230996dab7c9c125a`.

## Delivered Convention

Exactly two controls are shown for the current game, including unsupported
placeholders. Network Board is removed. Linked Cabinets defaults to -1 Disabled,
accepts 0/1 aliases, and disconnects every currently reviewed optional Model 3
board. Totals are 2–17 Daytona, 2–8 Scud, 2–5 Dirt Devils/Le Mans/Rally/Virtual On,
and 2–4 Harley/Ski Champ/Spikeout. The review identifies these as theoretical;
maximum totals and every role combination are not gameplay-qualified.

Auto Network Role Settings defaults to 0 Single/Stand Alone/No Link. -1 leaves
native management/manual NVRAM authority; 1 selects Master/first identity,
subsequent numbers Slave/identity, and r the native Live/Relay/Satellite role,
with l/s aliases. Values have native contextual labels. Spikeout has only
Master/Slave and no configurable identity. Le Mans puts Satellite in Cabinet
Number; Sega Rally combines role and car, and Virtual On uses Seats A–D.

Topology never overrides the independently selected native role. ANRS operates
with NVRAM Settings disabled, hides only managed role/ID fields and synchronizes
their selectors after a successful native application. Disabling automation
hands back the last applied values, not stale manual choices. Unrelated fields,
backup RAM and integrity follow the existing native adapter.

Effective changes coalesce into one reset on Resume. Explicit Restart consumes
pending choices before native execution. Equivalent aliases and unchanged Resume
do not reset. Fresh valid NVRAM initialization is applied before the first native
frame; native layouts not yet initialized remain pending rather than being
rewritten with guessed bytes. The existing F05 consuming-read retention remains.
Frontend option setters are published outside transport locks and tolerate a
synchronous visibility callback. A missing saved selector is compared with its
registered default, so hidden state is synchronized correctly too.

## Transport And Upstream Boundary

Machine reset clears frame/handshake data while retaining the frontend lobby,
send/poll callbacks and connected clients. Offline lobbies accept clients before
board setup. Cabinet count changes do not stop/start Netpacket or unload content.
Native Master leads the board ring independently of frontend host/client ID.
Temporary count mismatches and absent/duplicate Master during staggered setup
remain pending and recover from subsequent matching hellos. Explicit request
and reply packets allow a ready peer to answer a restarted peer without a hello
echo loop. Protocol is now Supermodel Model 3 NetBoard v3; v2 peers must update.

The earlier small Libretro-only CModel3 Reset hook is retained. No additional
CPU/machine redesign or Vulkan/backend work is included. Save State format and
existing policy are unchanged; native linked-board Save States and synchronized
racing are not qualified by this integration.

## Scud Australia Campaign

The historical exclusion was incorrect: Games.xml patched the Test button into
a graphics debug menu. A controlled comparison restores the normal Test Menu
and Game Assignments by removing only that patch. It is now removed from the
project catalogue/embedded fallback; normal ROM identities and CRCs are unchanged.

The native role is configurable, not assigned automatically: Single runs attract;
Master and Slave issue their respective board requests, with retained identities.
The networking delivery initially exposed only the two verified native networking
fields. The 2026-10-18 follow-up below completes the already-supported parent
Difficulty and Cabinet Type scope for this clone.

The existing RetroArch campaign runner and Scud recipe were adapted to the missing
Country row. Inventory precedes supplemental variations: eight operator settings,
thirty distinct values and closed photographed cycles. Fifty-seven raw window
photographs include repeated cycles; the primary inventory selects forty, plus
a factory-default photograph. Native default is Master/Car 1, while the frontend
default is Single. The game-committed baseline was created from a fresh EEPROM,
a changed-and-returned native value, full menu exits and re-entry. Its seed is
used by Automatic Initial NVRAM, without replacing valid existing saves.

Supplemental acquisition is limited to the implemented fields: three Link ID
values and eight Car Numbers. Each starts from the same native-committed baseline,
exits both menus and saves normally. All eleven samples pass fresh RetroArch re-import, photographed menu verification, native mirrors and CRC. The curated archive contains 52 primary photographs (40 value-cycle images, one factory baseline and eleven reloads). The former debug-menu dataset is retained under historical provenance. Other valid parent/clone samples are not regenerated.
The diagnostic YAML, recipe, workbook, catalogue, clone runner and validator
are reconciled; the previous 34-clone/461-value campaign remains provenance.

## Verification

Local evidence root: `outputs/network-two-option-20261017/`.

| Scope | Evidence / Result |
| --- | --- |
| Build / ABI | macOS ARM build and existing 25-export/dependency gate pass. Only the pre-existing linker alignment warning remains. |
| Final Registration | 255 definitions across APIs 0/1/2. All 22 supported pairs, complete reviewed help, labels, values, aliases, defaults, adjacency and deterministic order pass. All 207 pre-existing non-network definitions retain values/defaults/order. The 21 existing native Link Mode helps remove obsolete host/Master coupling; the other 186 definitions are identical. Two Scud AU native fields are added. |
| Native Presets | 22 sets / 159 presets pass native integrity, masked unrelated-field and backup-RAM preservation, supported encodings and repeat application. |
| ABI Lifecycle | All 22 sets pass consuming reads, repeated paused callbacks, synchronous re-entry, no-display-callback Resume, NVRAM-off ANRS, hidden synchronization, handover, combined/unchanged/alias reset counts, explicit Restart, invalid fallback and fresh Save RAM reload. |
| Transport | Public device/backing-memory probe passes board fitting/removal, 1/2/1/3 totals, retained callbacks/full connected-client roster, transient count/role recovery and native Master independent of host. The theoretical 17-node roster is a transport probe, not gameplay. The additional role-recovery case fails before the correction and passes after it. |
| Actual Linked Frontend | Two isolated RetroArch ARM 1.22.2 instances form Scud 2/2 native rosters and complete the first data-frame exchange. The reversed test uses a frontend host with native Slave/Car 2 and a client with native Master/Car 1, on the final core. Both exit 0. Host peer-frame timeout occurs after the deliberately closed client disconnects during teardown. No race/input qualification. |
| Actual Native Campaign | Scud AU normal menus, complete value cycles and native committed samples acquired through RetroArch, with isolated configuration/save paths. All eleven samples pass native integrity and fresh frontend re-import, with 52 curated primary photographs. |
| Installed Frontend | Standard installed core and active external System assets. System shows exactly the two controls; defaults are Disabled / Single. With NVRAM Settings off, Master and Single each reset once on Resume; unchanged pause/Resume adds no reset (two total). Single reaches attract. Only the owned QA processes are closed. Audio is disabled in this isolated UI check. |
| Existing Clone Campaign | The existing validator passes 472/472 acquired values across 35 release clones, including the eleven new Scud AU samples. Source files were read unchanged into a hash-recorded temporary mirror because direct validator access to Documents stalled; no native samples were regenerated. |
| Workbook Scope | Eight clone rows added and one coverage row corrected. All other values and all three green native table styles retained; table/filter range extends to A1:G397. |

ROM-free registration, offscreen core ABI/native execution, transport simulation,
actual frontend behavior, installed identity and synchronized gameplay are distinct
claims. No maximum-total, other-platform, physical-controller or new racing
qualification is implied. Historical preliminary probes are retained with their
errors/limits; they are not substitutes for the final campaign and audits.

## Local Delivery

Final core SHA-256:
`05e67dc406f7f8f3b5ade96d04a4544c65d14ae6f122a333417b42766ccec082`.

The configured standard core/info and external System path are checked before
installation. Framework F19 uses retained scoped source and rebuild rollback;
no additional binary backup is created. Installed hashes, support-file scope,
archive curation and final frontend checks are recorded in the deployment/final
verification manifests. No commit or push.

## Active Netplay Menu Lifecycle Follow-Up — 2026-10-18

### Required Behavior

The user clarified that both instances may join the lobby in native Standalone,
then change Linked Cabinets and ANRS without leaving that lobby. Stage edits
while the local Quick Menu is open; apply the final effective combination and
reset the local machine once on menu exit. No effective change means no reset.
Keep the transport session, callbacks and client roster throughout. The need to
continue Netpacket communication does not require resetting on each menu edit.

### Actual Frontend Reproduction

Evidence: `outputs/anrs-menu-20261018/`. Runs reuse the existing isolated smoke
environment with copied configuration, separate System/save/options/log paths,
and uniquely owned QA processes. Audio is disabled. Public announcements, MITM
and NAT traversal are disabled for the local host trials. The frontend is
RetroArch 1.22.2, revision `9be7ec92`; the ARM Mach-O UUID matches the configured
user frontend (`A455EF9F-C973-34C1-8FF5-69D8B3DB7F10`).

| Trial | Observed Result |
| --- | --- |
| Daytona 2, no lobby, menu pause enabled | Single → Car 1 → Car 2: zero resets while the System menu remained open; one reset on Resume; another unchanged pause/Resume added zero resets. Counts come from native `Model 3 reset.` log records. |
| Daytona 2, Netpacket host, frontend Netplay pausing disabled | The same two effective ANRS edits each reset immediately with the System menu visibly open; reset count 1 then 2 before any Resume. |
| Installed TGPulse, Virtua Racing, same Netpacket host conditions | No Link → Red (Master) reset immediately with the System menu still open. The reset OSD and native CANCELLED screen were observed; one reset notification was logged. |
| SM2 current source | Its pending application/reset path also executes in retro_run. No equivalent real-menu runtime acceptance is claimed here. |

The Supermodel build and configured installed standard core both remain SHA-256
`ca0360e3f0b64bff689157f64f7e7a1a97bee8c2ee75fc8c985d4d0389eef235`.
No production source or user saves/settings were modified. Owned frontend runs
exited normally. TGPulse's first UI attempt could not complete value selection;
only the second trial supplies the ANRS reproduction.

### Cause And Existing Evidence Boundary

Supermodel's display callback retains the update signal without patching NVRAM.
Its next retro_run reads the final current options, patches effective native
values and resets if required. SM2 and TGPulse use the same next-run assumption.
When RetroArch pauses the core in its menu, that next call is Resume. In active
Netplay it can instead occur while the menu is still open.

The matching official frontend source makes menu pausing conditional on
[ALLOW_PAUSE](https://github.com/libretro/RetroArch/blob/9be7ec92/menu/menu_driver.c#L6497-L6502).
[Netpacket with active connections disallows that pause](https://github.com/libretro/RetroArch/blob/9be7ec92/network/netplay/netplay_frontend.c#L9624-L9628);
the tested host without peers also disallows it when netplay_allow_pausing is
false. This explains continued execution, not a requirement to apply ANRS.

The existing ABI test's edit(paused=True) invokes the display callback and then
retro_run, assuming that call is Resume. Its passing reset counts and transport
retention do not qualify this real-menu timing. The earlier Installed Frontend
row remains evidence for a paused single session only.

### Feasibility Outcome And User-Approved Fallback

The current Libretro ABI has no menu-close notification identified in this
review. GET_THROTTLE_STATE exposes pause/throttling and uses the same
[menu-pause eligibility](https://github.com/libretro/RetroArch/blob/9be7ec92/runloop.c#L3246-L3264);
it cannot identify an open, non-pausing Netplay menu. Treating silence in player
input, a timeout, or the next frame as menu exit would be an unreliable substitute.

The user made menu-exit deferral conditional on feasibility and explicitly
approved retaining the current behavior otherwise. No reliable portable core-only
solution was identified with this frontend. Current runtime behavior is therefore
retained: paused sessions coalesce on Resume; active non-pausing Netplay sessions
apply each effective edit on the next retro_run. The lobby is not deliberately
closed by the reset path. This follow-up did not repeat a two-connected-peer
runtime test; earlier retained-transport evidence remains scoped as above.

An explicit supported frontend-to-core menu lifecycle signal would be needed
before implementing guaranteed menu-exit deferral while frames continue. Such a
frontend change is a future design question, not an approved implementation here.
If later pursued, reuse existing effective-change and retained-lobby reset handling
and accept it with two connected frontend instances initially Standalone: several
topology/role edits, no in-menu resets, one commit/reset on local menu exit,
unchanged Resume and retained connections. No heuristic, production-code change,
new core build, deployment, commit or push was introduced.

Framework follow-up must qualify the shared Resume wording against actual
non-pausing Netplay behavior and preserve this destination's explicit fallback;
it must not claim that another core's paused-session acceptance proves otherwise.

## Audio Recovery During Machine Reset — 2026-10-18

The previous ResetLibretroAudio cleared the queue with writePos == playPos == 0.
With the asynchronous sound producer, under-run recovery could wrap the write
position onto playPos: GetAvailableAudioLen then reported empty while OutputAudio
reported full, leaving the consumer producing silence indefinitely. This was
reproduced without RetroArch or launcher involvement by collecting actual PCM
from the installed core before and after two explicit resets.

Reset now uses the same two-packet silent priming as OpenAudio, and OpenAudio
reuses that initialization function. The producer is parked during machine
reset. The patch preserves callbacks, enabled state, gains, sound hardware and
network ownership; it does not redesign the existing ring buffer.

Evidence root: `outputs/audio-reset-20261018/`. Old core: eight consecutive
ten-second windows after two resets had peak/RMS/nonzero count zero. Candidate:
all corresponding windows recovered nonzero PCM. Real RetroArch 1.22.2 with
CoreAudio3 and an isolated host lobby produced recorded audio after two resets
in Single mode and after one reset in matching Multi-threaded + GPU. The user
confirmed the recorded excerpts correct, then correct live Multi-threaded + GPU
playback without recording both before and after a further reset.

The first live Single/recording trial was reported distorted despite correct
recorded PCM; it is not hardware-audibility acceptance. That symptom was not
reproduced in the aligned no-recording trial. No definitive driver cause, driver
fix or user configuration change is claimed. The relevant waveform recordings,
frontend configs, reset events and user-acceptance summary are retained.

The existing Daytona 2 ABI/network matrix passes 18 native presets, effective
reset counts, authority handover, explicit Restart and retained callbacks/client
roster. This is not a new two-connected-frontend racing/audio qualification.
The two original user sessions were closed normally only after explicit user
authorization; owned QA trials exited 0. Build and 25-export/dependency gates
pass. Standard local core SHA-256: `5cca917dc93bcc9ea9b65377d5f87658533038c14c1bee20506b4e04442c7862`. Info,
active configuration and external System assets remain unchanged. Scoped source
rollback/rebuild is retained; no extra binary backup, commit or push.

## Scud Australia Override Completion — 2026-10-18

The native inventory was complete, but an obsolete per-set exclusion and missing
selector defaults still blocked Difficulty and Cabinet Type. The exclusion now
applies only to Country, which has no native selector in this Australian release.
The existing Scud field patcher is reused: Difficulty is the low byte of words
16/45 (Easy=0, Normal=1, Hard=2, Hardest=3); Cabinet Type is the high byte
(Deluxe=0, Twin=1). Defaults are Normal and Twin.

Six supplemental values were acquired through the existing RetroArch campaign
runner from the previously validated game-committed baseline. All six pass fresh
RetroArch reloads with photographed native values, EEPROM CRC and mirrors.
The previous eleven networking samples remain valid and unchanged. The existing
clone validator now passes 478 acquired values across 35 release clones.
Evidence: `outputs/scudau-settings-20261018/validation.json` and
`outputs/scudau-settings-20261018/clone-validation.tsv`.

The dedicated initial EEPROM seed already contains Normal/Twin and is preserved;
Automatic Initial NVRAM still explicitly applies Single without overwriting valid
user saves. The common workbook, diagnostic YAML, standalone recipe and clone
runner now reflect all four managed fields. Advertise Sound, Game Mode, Motor
Power and Multi Display remain inventoried and pending selection in the roadmap,
consistent with the current parent implementation.

macOS ARM build and 25-export ABI validation pass. Registration adds exactly two
definitions (258 total), preserving all 256 previous definitions. A real RetroArch
Service Menu probe also confirms that enabled NVRAM overrides apply Hardest and
Deluxe together. This does not add linked gameplay or other-platform qualification.

A supplemental runner failure exposed a timing race in `wait_while_alive`: a
second monotonic-clock read could cross the deadline and pass a negative sleep.
Clamp the remaining delay at zero. Existing successful samples were retained;
only the interrupted sample was retried.
