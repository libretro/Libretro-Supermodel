# Network Board And Linked Cabinets Integration

Date: 2026-10-14. Implemented and locally delivered; no commit or push.

Network Board controls attachment. Linked Cabinets Disabled keeps a present
board in local loopback; supported numeric totals retain their existing limits.
With Network Board disconnected, Linked Cabinets and ANS are hidden and their
saved selections are preserved. ANS follows the existing NVRAM Settings gate.
With NVRAM Settings enabled, offline topology applies reviewed Single / Stand
Alone / No Link fields regardless of the retained linked preset; that preset
applies only with a connected board and at least two linked cabinets. In linked
mode ANS Disabled retains manual control. No override occurs with NVRAM Settings
disabled. Published defaults and numeric choices are unchanged.

Effective board/topology changes apply automatically on Resume. Runtime
configuration is updated, changed native settings are committed, and one machine
reset refreshes the board attachment and mapped hardware. Network and NVRAM
changes are coalesced; unchanged Resume does not reset. This required a five-line
Libretro-only hook in Model3.cpp; standalone behavior is unchanged. The earlier
manual-restart notice was an integration error and has been removed.

Validation: macOS ARM build and the existing 25-symbol ABI/dependency gate pass.
Core Options APIs 0/1/2 preserve all 251 definitions, defaults, numeric choices,
order and native NVRAM metadata, adding Disabled only to 22 Linked Cabinets keys.
Eighty-four offline preset cases cover 21 reviewed sets and preserve unrelated
selection fields. Type 1/2 board probes cover local payload echo, detachment and
reattachment, and independence from an active frontend client ID (no sends,
polls or accepted external peers in local mode).

RetroArch ARM 1.22.2 / Ozone actual frontend proof: Scud and Le Mans reach attract
with board present, Disabled topology and retained ANS 2. System-menu inspection
confirms hide/restore and retained values. A separate Scud lifecycle run confirms
one reset each for disconnection, combined reconnection/two-cabinet configuration,
and return to Disabled; unchanged Resume adds zero resets. Native Slave / Car 2
was visible after applying total 2; returning to Disabled restores offline boot.
The modern OSD reset notification was observed. Owned instances exit normally.
Screenshots from boot runs are saved; interactive menu/reset screenshots were
inspected in the chat, not saved to disk. Held CUA keyboard input (250 ms) worked
when coordinate actions reported noWindowsAvailable and short taps were missed.

This is local macOS/device/frontend proof, not full-catalogue, physical-input,
complete-race or synchronized multiplayer acceptance. Existing per-title numeric
limits are retained; the full topology review remains separate. Tests reuse
dated probes/runners and do not add permanent Makefile checks.

Evidence: outputs/network-integration-20261014/, including registration.json,
ans-probe.log, board-probe.log, ui-auto-reset/reset-checks.json and deployment.json.
Standard installed-core SHA-256: `13d9362be2d944f22f09cd0b8755a57c8bb518d6d090aae2261ff00628acdc8c`.
Core/info backups are retained; configured external System files are unchanged.
The user authorized starting TGPulse feasibility in its existing chat; monitoring
continues there, not in this Supermodel chat.
