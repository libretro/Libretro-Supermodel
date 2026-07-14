#ifndef SUPERMODEL_CORE_OPTIONS_TYPES_H
#define SUPERMODEL_CORE_OPTIONS_TYPES_H

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
   bool transparency_fast;   // New3D: true = 1-layer transparency (faster), false = 2-layer (accurate)
};

extern CoreOptions g_options;

#endif
