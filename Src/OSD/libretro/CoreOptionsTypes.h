#ifndef SUPERMODEL_CORE_OPTIONS_TYPES_H
#define SUPERMODEL_CORE_OPTIONS_TYPES_H

// Pad layout for driving games. L/R always shift sequentially and L2/R2 are the pedals.
// The optional layouts only decide whether the right stick also acts as a shifter.
enum class DrivingLayout {
   Default,            // right stick free
   TriggersGate,       // gears 1-4 on the right stick
   TriggersSequential  // sequential shift on the right stick
};

struct CoreOptions {
   bool initial_nvram_setup;
   float resolution_multiplier;
   bool widescreen;
   bool wide_background;     // stretch the lower 2D layer too (needs widescreen)
   int upscale_mode;         // Supermodel UpscaleMode: 0=Nearest 1=Biquintic 2=Bilinear 3=Bicubic
   unsigned crosshairs;      // native Supermodel mask: 0=off, 1=P1, 2=P2, 3=both
   bool force_feedback;
   int analog_sensitivity;
   int sound_volume;
   int music_volume;
   bool legacy_sound_dsp;    // SCSP DSP: false = MAME (default), true = Legacy ElSemi
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
