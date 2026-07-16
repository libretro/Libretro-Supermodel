#ifndef SUPERMODEL_CORE_OPTIONS_TYPES_H
#define SUPERMODEL_CORE_OPTIONS_TYPES_H

// Pad layout for driving games. The two analog layouts differ only in what the right stick
// does: a gate shifter (4 gears) or a sequential lever (up/down). It cannot be both.
enum class DrivingLayout {
   Default,            // pedals on the d-pad, gears 1-4 on L/R/L2/R2
   TriggersGate,       // pedals on the triggers, gears 1-4 on the right stick
   TriggersSequential  // pedals on the triggers, sequential shift on the right stick
};

struct CoreOptions {
   float resolution_multiplier;
   bool widescreen;
   bool vsync;
   bool crosshairs;
   bool force_feedback;
   int analog_sensitivity;
   int sound_volume;
   int music_volume;
   bool service_on_sticks;
   int ppc_frequency;
   int frameskip;
   bool sound_enable;
   bool jit_enable;
   bool timing_overlay;      // draw the ImGui frame-timing overlay (costs a draw pass every frame)
   DrivingLayout driving_layout;
};

extern CoreOptions g_options;

#endif
