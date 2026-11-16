#ifndef SUPERMODEL_CORE_OPTIONS_TYPES_H
#define SUPERMODEL_CORE_OPTIONS_TYPES_H

enum class GunInput {
   Hybrid,        // RetroLightgun, RetroMouse and RetroPad share one cursor
   Lightgun,      // RetroLightgun, absolute screen coordinates
   Mouse,         // RetroMouse, relative movement
   MouseAnalog,   // Hybrid variant that excludes RetroLightgun input
   AnalogSticks   // RetroPad left stick controlling a relative cursor
};

enum class StarWarsInput {
   Hybrid,        // RetroLightgun, RetroMouse or absolute RetroPad stick
   Lightgun,      // RetroLightgun absolute screen coordinates
   Mouse,         // RetroMouse relative movement
   MouseAnalog,   // Hybrid variant that excludes RetroLightgun input
   AnalogSticks   // RetroPad left stick as an absolute arcade yoke
};

enum class WidescreenMode {
   Disabled,
   Widescreen,
   WidescreenWideBackground
};

enum class AVTimingMode {
   Default60Hz,
   Native57524Hz
};

enum class Renderer3D {
   New3D,
   Legacy3D
};

enum class EmulationThreading {
   SingleThread,
   MultiThreaded,
   MultiThreadedGPU
};

enum class FourSpeedShifter {
   Standard,  // each right-stick half-axis selects one gear directly
   HGate      // right-stick diagonals reproduce a four-position H-pattern
};

struct CoreOptions {
   bool initial_nvram_setup;
   bool network_board;
   unsigned network_cabinets;
   bool nvram_settings_enabled;
   float resolution_multiplier;
   Renderer3D renderer_3d;
   bool legacy_multi_texture;
   bool quad_rendering;
   int crt_colors;
   int supersampling;
   int upscale_mode;
   WidescreenMode widescreen_mode;
   bool no_white_flash;
   AVTimingMode av_timing_mode;
   unsigned crosshairs;      // native Supermodel mask: 0=off, 1=P1, 2=P2, 3=both
   bool force_feedback;
   bool steering_progressive;             // driving: quadratic fine-center steering curve
   int steering_output_range;             // driving: percent, 50..150
   int accelerator_output_range_per_mille;
   int brake_output_range_per_mille;
   int sound_volume;
   int music_volume;
   bool legacy_sound_dsp;
   int ppc_frequency;
   int frameskip;
   EmulationThreading emulation_threading;
   bool sound_enable;
   bool jit_enable;
   bool timing_overlay;      // draw the ImGui frame-timing overlay (costs a draw pass every frame)
   GunInput gun_input;
   StarWarsInput star_wars_input;
   bool star_wars_upright_x_inversion;
   FourSpeedShifter four_speed_shifter;
};

extern CoreOptions g_options;

#endif
