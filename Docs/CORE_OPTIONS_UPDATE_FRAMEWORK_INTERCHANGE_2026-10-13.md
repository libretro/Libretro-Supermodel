# Core Options Update Signal — Framework Interchange

Date: 2026-10-13  
Source: Supermodel Libretro, local changes over `4566c9470ab89ec8e2738bfb43c18281df580971`  
Contracts: F05 (Lifecycle), F09 (Paused Profile Refresh), F12 (NVRAM Application), F21 (Notifications)  
Framework Feedback: Edition 0.1.6

## Symptom And Cause

The real RetroArch ARM 1.22.2 (`9be7ec92`) windowed Scud probe could change
Controller Profile Type and Controls metadata while paused. However, enabling
NVRAM Settings and selecting Difficulty Hard did not apply the change or show
the reset notification after Resume.

The new profile poll called `GET_VARIABLE` before `GET_VARIABLE_UPDATE` in
`retro_run`. The paused display callback also read variables. RetroArch 1.22.2
clears its global Core Options update flag when servicing `GET_VARIABLE`, not
when servicing `GET_VARIABLE_UPDATE`. Reading even an unrelated profile option
can therefore consume the update signal needed by other runtime settings.
An ABI mock which clears the flag only on GET_VARIABLE_UPDATE misses this bug.

Primary source: [RetroArch 1.22.2 runloop.c](https://github.com/libretro/RetroArch/blob/v1.22.2/runloop.c#L1281-L1330).

## Delivered Correction

Source: [libretro.cpp](../Src/OSD/libretro/libretro.cpp).

1. Query the update flag before polling the profile in the frame path. SM2's
   existing `retro_run` update-before-read ordering is the reference.
2. Query and retain the flag before variable reads in the paused display
   callback, using one pending boolean. Merge that pending indication with
   the frame-time flag when gameplay resumes.
3. Consume the pending indication for the effective update pass, read the
   latest option values, and retain existing change detection. Multiple menu
   edits coalesce; unchanged/inactive NVRAM does not cause repeated resets.
4. Reset the pending state when establishing a new environment. No option
   key, raw value, saved remap, native setting encoding or public default is
   changed by this fix.

The mechanism belongs to the Libretro adapter. It is not Model 3-specific and
does not require a shared library, generic event bus or per-frame notification.
Frontend variable strings still obey their existing lifetime rules; the latch
stores only the pending signal, not borrowed variable pointers.

## Verification And Boundaries

- Local macOS ARM build and existing binary-format, dependencies and 25-symbol
  Libretro gates pass.
- Focused ABI checks reproduce RetroArch's actual flag-clearing semantics and
  cover Core Options registration versions 0/1/2, a display callback before
  Resume, effective/unchanged/inactive NVRAM changes, state reapplication,
  profiles, visibility, extended messages and legacy/rejected-message fallback.
- Real windowed RetroArch ARM/Ozone on Scud shows Controller Profile Type first
  in Input with Normal + Test/Service as default. While paused, Normal clears
  the operator assignments on both ports; full restores Service A/B on L3 and
  Test A/B on R3, with immediate Device Type updates.
- On the corrected build, enabling NVRAM Settings and choosing Difficulty Hard
  then resuming logs exactly one Model 3 reset. The OSD visibly shows
  `Applied NVRAM Settings; machine reset.` and the attract sequence resumes.
- The corrected core is installed, backup retained and hash verified:
  `17cdde5e287691f8fae90981bdda1b5ecebccab77ec0f1b5137bbcd1af3fe74c`.
  The isolated frontend exits normally (0); no RetroArch instance remains.

Detailed local delivery record: [Roadmap](ROADMAP.md).
After the frontend checks, the user restored the standard local name
`supermodel_libretro.dylib` and matching info, replacing the older standard
binary and removing the dev-named core/info. The verified binary hash above
is unchanged. Current naming and configured asset paths are recorded in
[Local Deployment Check](LOCAL_DEPLOYMENT_CHECK_2026-10-13.md).
Temporary diagnostic logs/results are under
`/private/tmp/supermodel-lcf-visual-retry/` and
`/private/tmp/supermodel-lcf-ra-semantics-api{0,1,2}.log`; these are local
ephemeral evidence, not packaged assets. CUA screenshots were observed in the
chat and were not saved as local screenshot files.

This does not qualify physical controllers, audio, Linux/Windows gameplay,
legacy frontend UI or notification stacking in every frontend. It does not
establish a defect in SM2-Emu, TGPulse or another destination merely because
they implement the same interface.

## Destination Review Checklist

Before adapting to another core, inspect all runtime option reads and paused
display callbacks. Preserve the update signal before any read that can consume
it, or demonstrate that the destination's existing application path does not
depend on that signal. Reuse its existing lifecycle/change-detection machinery.

Validate an effective edit, an unrelated profile read, several edits while
paused, inactive/unchanged settings and the frontend-without-display-callback
Resume path. Keep ABI registration/fallback checks distinct from real menu,
OSD and gameplay evidence. A frontend mock must model the consuming read; do
not add a permanent Makefile check solely to retain this temporary probe.

Record the destination's applicability, adoption decision and result in its
own roadmap/chat. This feedback updates the Framework contract and evidence;
it does not modify another core or authorize publication.
