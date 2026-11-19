# Gamepad Rumble Corrections

## Scope And Cause

The common Libretro output adapter previously wrote each force-feedback command
directly to a motor. Constant Force and Self Center shared the strong motor;
Friction and Vibration shared the weak motor. Stopping one effect therefore
cancelled another still-active effect. Joystick X/Y commands also shared state.

Gamepad Rumble also controlled the emulated drive board's startup configuration.
Starting with the option disabled disabled the board itself. Enabling output
later changed only an input-system flag and could not restart native effects.
The old initialization workaround applied that flag after board initialization
because the earlier attempt ran before the input system existed.

## Implementation

Keep the emulated drive board active independently of the gamepad output option.
Acquire the frontend rumble interface and apply the selected output state to the
new input system before the initial machine reset. No forced off/on workaround
is needed. Runtime changes mute or restore output without machine reset/reload.

Maintain native effect strengths per controller port and axis, including while
muted. Publish the maximum active contribution to each motor, preserving the
existing strong/weak assignments. An effect's zero or Stop affects only its own
state; machine reset and teardown explicitly clear all states and stop motors.
A mutex serializes drive-thread commands and frontend option changes.

Reuse SM2's separation between native simulation and frontend output, and its
maximum-contribution mixing principle. Do not port its synthetic cornering
effect, steering onset threshold, amplification or pulse duration. Supermodel
retains native force magnitudes with no additional five-percent threshold.

## Verification

Evidence: `outputs/scud-rumble-20261018/`.

Direct probes cover overlapping effects, two ports, independent joystick axes,
muted state updates, output restoration without a new native command, scoped
Stop, reset/teardown and unchanged native magnitudes. macOS ARM build and the
existing Libretro ABI gate pass.

Scud is the runtime regression case for this shared adapter. Isolated CGL/Libretro
runs exercise startup enabled and startup disabled followed by enable, disable
and re-enable during one game session. Motor callbacks and fixed-frame pictures
are compared with the pre-fix baseline. These are core API/output checks, not
physical-controller or other-platform gameplay qualification. No native emulator
drive-board code or persistent user settings are changed.

Final results: 232 nonzero callbacks with output enabled at startup; 233 with
output enabled after startup and a later mute/unmute. During the held Scud effect,
frame 4140 stops the motor and frame 4170 restores its current strength of 53199
without a new native command. All six fixed-frame pictures match the pre-fix
baseline exactly. Teardown stops both motors on both ports. See
`validation-fixed.json`, `check-fixed.log` and `deployment-fixed.json` in the
evidence directory. The verified macOS ARM core is installed locally; the matching
info, frontend configuration, saved option selections and System XML are retained.
