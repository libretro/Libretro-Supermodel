#ifndef SUPERMODEL_CORE_OPTIONS_TYPES_H
#define SUPERMODEL_CORE_OPTIONS_TYPES_H

// Pad layout for driving games. L/R always shift sequentially and L2/R2 are the pedals.
// The optional layouts only decide whether the right stick also acts as a shifter.
enum class DrivingLayout {
   Default,            // right stick free
   TriggersGate,       // gears 1-4 on the right stick
   TriggersSequential, // sequential shift on the right stick
   TriggersHGate       // gears 1-4 on the right stick diagonals (H-pattern)
};

// Renderer capabilities behind the 3D Renderer and CRT Colour options.
// HAVE_LEGACY3D comes from the Makefile (Legacy3D sources in the build). The CRT
// colour pass is only validated on desktop OpenGL.
#if !defined(ANDROID) && !defined(CORE_GLES) && !defined(HAVE_OPENGLES)
#define HAVE_CRT_COLOURS
#endif

enum class Renderer3D {
   New3D,
   Legacy3D
};

// Source of the per-player virtual gun cursors (Supermodel MOUSE5/MOUSE6).
enum class GunInput {
   Hybrid,        // RetroLightgun, RetroMouse or left stick; the last one moved wins
   Lightgun,      // RetroLightgun, absolute screen coordinates
   MouseAnalog,   // RetroMouse or left stick (Hybrid without RetroLightgun)
   Mouse,         // RetroMouse, relative movement
   AnalogSticks   // RetroPad left stick moving a relative cursor
};

// Source of the Star Wars Trilogy yoke (Supermodel MOUSE7).
enum class StarWarsInput {
   Hybrid,        // mouse or absolute RetroPad stick, the last one moved wins
   Mouse,         // RetroMouse relative movement
   AnalogSticks   // RetroPad left stick as an absolute arcade yoke
};

// Supermodel MultiThreaded / GPUMultiThreaded (read when the content starts).
enum class EmulationThreading {
   SingleThread,
   MultiThreaded,      // sound and drive boards on worker threads
   MultiThreadedGPU    // also overlaps PowerPC emulation with rendering
};

struct CoreOptions {
   bool initial_nvram_setup;
   float resolution_multiplier;
   bool widescreen;
   bool wide_background;     // stretch the lower 2D layer too (needs widescreen)
   int upscale_mode;         // Supermodel UpscaleMode: 0=Nearest 1=Biquintic 2=Bilinear 3=Bicubic
   unsigned crosshairs;      // native Supermodel mask: 0=off, 1=P1, 2=P2, 3=both
   bool force_feedback;
   bool steering_progressive;             // driving: quadratic fine-center steering curve
   int steering_output_range;             // driving: percent, 50..150
   int accelerator_output_range_per_mille;
   int brake_output_range_per_mille;
   int sound_volume;
   int music_volume;
   bool legacy_sound_dsp;    // SCSP DSP: false = MAME (default), true = Legacy ElSemi
   bool service_on_sticks;
   int ppc_frequency;
   int frameskip;
   EmulationThreading emulation_threading;
   bool sound_enable;
   bool jit_enable;
   bool timing_overlay;      // draw the ImGui frame-timing overlay (costs a draw pass every frame)
   DrivingLayout driving_layout;
   GunInput gun_input;
   StarWarsInput star_wars_input;
   Renderer3D renderer_3d;
   int crt_colors;           // Supermodel CRTcolors: 0 = none, 1-5 = monitor profiles
};

extern CoreOptions g_options;

#endif
