# Renderer State After Live Presentation Changes

The user reported corrupted Daytona 2 3D after changing Widescreen Mode or
Aspect Ratio, while the HUD remained intact. Previous lifecycle checks proved
mode adoption and no resets but did not prove persistent texture integrity.

## Cause And Correction

The shared resize path creates a new 3D renderer. `CReal3D::AttachRenderer`
attached emulated memory and stepping, but did not populate the replacement
GPU texture banks or restore persistent rendering registers. Existing texture
RAM was therefore present in emulation but missing from the new GPU resources.

Restore the complete texture sheet, sun clamp and block-culling state when
attaching a renderer, using the same existing native operations as LoadState.
This is a small native attachment correction; no shader, projection or texture
format changes and no Save State round trip/reset in the runtime resize path.
A separate reused-content case retained the old wide viewport margin: reset
the wrapper offsets when a new content load establishes its native dimensions.

## Verification

A static Expand control and live Disabled-to-Expand case were rendered from
identical saved 3D machine state. Before the fix, the live frame lost vehicle
and scenery textures. Afterwards their pixels match exactly. Fifteen static/live
comparisons pass across Daytona 2 and Harley-Davidson, plus Daytona 2 with the
user's current Quad Off, Supersampling 1, 2x resolution, Multi-threaded + GPU.
The comparisons cover WM expansion, Aspect Ratio expansion, WM return to
Disabled, explicit 4:3, and background-only switching, with no option reset.

Single-thread controls compare the first frame. Multi-GPU controls compare
three frames after Save State restoration to avoid first-restored-frame cache
history; those artifacts and the earlier viewport-control failure are retained
and distinguished in the evidence. Twenty existing CGL transitions and the
macOS build/API/dependency checks pass; all 256 option definitions are unchanged.
No other-platform runtime or frontend CRT-shader qualification is claimed.

Evidence: `outputs/widescreen-renderer-state-20261018/`.
Build SHA-256: `1c333eb0d3ab0bd5b584e2069484147cceb4dcef99574e77d5428d37104707f1`. No commit or push.
