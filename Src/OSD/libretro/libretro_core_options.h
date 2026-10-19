#include "libretro.h"
#include "libretro_cbs.h"
#include <cstdlib>
#include <cstring>

// --- Core Options ---
static struct retro_core_option_v2_category option_cats[] = {
   {
      "system",
      "System",
      "Configure machine initialization, persistence, and networking."
   },
   {
      "video",
      "Video",
      "Configure graphics and rendering options."
   },
   {
      "audio", 
      "Audio",
      "Configure audio settings."
   },
   {
      "input",
      "Input",
      "Configure input and control settings."
   },
   {
      "cpu",
      "CPU",
      "Configure CPU performance and emulation settings."
   },
   { NULL, NULL, NULL },
};

static struct retro_core_option_v2_definition option_defs[] = {
   // F04: one definition sequence feeds modern and legacy registration.
   // System: preserve initialization, networking and native operator order.
   {
      "supermodel_initial_nvram_setup",
      "Automatic Initial NVRAM Setup (Restart Required)",
      NULL,
      "Initialize missing save data from a validated NVRAM template and apply approved initial settings while preserving existing valid saves.",
      NULL,
      "system",
      {
         { "enabled",  NULL },
         { "disabled", NULL },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "supermodel_nvram_settings",
      "NVRAM Settings",
      NULL,
      "Apply reviewed operator overrides to saved NVRAM while preserving unrelated fields and calibration.",
      NULL,
      "system",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   // Video: rendering, image presentation, then timing and diagnostics.
   // Rendering engine and its specific controls.
#if defined(HAVE_LEGACY3D) && !defined(USE_LEGACY3D)
   {
      "supermodel_renderer_3d",
      "3D Renderer",
      NULL,
      "Select the New3D or Legacy3D graphics engine.",
      NULL,
      "video",
      {
         { "new3d",    "New3D (Default)" },
         { "legacy3d", "Legacy3D (Experimental)" },
         { NULL, NULL },
      },
      "new3d"
   },
#endif
#ifdef HAVE_QUAD_RENDERING
   {
      "supermodel_quad_rendering",
      "Quad Rendering",
      NULL,
      "Render native quadrilaterals directly instead of splitting them into triangles. Applies only to the New3D renderer.",
      NULL,
      "video",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
#endif
#ifdef HAVE_LEGACY3D
   {
      "supermodel_legacy_multi_texture",
      "Legacy3D Multi-Texture",
      NULL,
      "Select single-texture or eight-texture-map decoding for the legacy renderer. Applies only to the Legacy3D renderer.",
      NULL,
      "video",
      {
         { "disabled", "Single Texture (Default)" },
         { "enabled",  "Eight Texture Maps" },
         { NULL, NULL },
      },
      "disabled"
   },
#endif
   // Image quality and presentation.
   {
      "supermodel_resolution",
      "Internal Resolution",
      NULL,
      "Select the internal rendering resolution to balance image quality and GPU load. Changes apply when returning to the game.",
      NULL,
      "video",
      {
         { "half",   "Half (248x192) - Low-End Mobile" },
         { "native", "Native (496x384) - Recommended" },
         { "2x",     "2x (992x768)" },
         { "3x",     "3x (1488x1152)" },
         { "4x",     "4x (1984x1536)" },
         { NULL, NULL },
      },
      "native"
   },
#ifdef HAVE_SUPERSAMPLING
   {
      "supermodel_supersampling",
      "Supersampling (Restart Required)",
      NULL,
      "Render additional internal samples per output pixel to improve image quality. Applies only to Vulkan / OpenGL / GLES renderers.",
      NULL,
      "video",
      {
         { "1", "1x (Disabled, Default)" },
         { "2", "2x (4 Samples)" },
         { "3", "3x (9 Samples)" },
         { "4", "4x (16 Samples)" },
         { "5", "5x (25 Samples)" },
         { "6", "6x (36 Samples)" },
         { "7", "7x (49 Samples)" },
         { "8", "8x (64 Samples)" },
         { NULL, NULL },
      },
      "1"
   },
#endif
   {
      "supermodel_upscale_mode",
      "2D Layer Upscaling Filter",
      NULL,
      "Select the upscaling filter applied to 2D tile layers before compositing with 3D.",
      NULL,
      "video",
      {
         { "0", "Nearest" },
         { "1", "Biquintic" },
         { "2", "Bilinear (Default)" },
         { "3", "Bicubic" },
         { NULL, NULL },
      },
      "2"
   },
   {
      "supermodel_aspect_ratio",
      "Aspect Ratio",
      NULL,
      "Select the displayed aspect ratio. Auto follows Widescreen Mode. Explicit 4:3 suppresses expansion; 16:9 with Widescreen Mode Disabled stretches the original image. Changes apply when returning to the game.",
      NULL,
      "video",
      {
         { "auto", "Auto" },
         { "4_3", "4:3" },
         { "16_9", "16:9" },
         { NULL, NULL },
      },
      "auto"
   },
   {
      "supermodel_wide_screen",
      "Widescreen Mode",
      NULL,
      "Expand the 3D field of view with Aspect Ratio set to Auto or 16:9, optionally stretching the lower 2D background. Explicit 4:3 suppresses expansion. Upper 2D layers keep their proportions. Disabled retains original rendering; Aspect Ratio 16:9 then stretches the complete image. Changes apply when returning to the game without a reset or content reload.",
      NULL,
      "video",
      {
         { "disabled", "Disabled" },
         { "enabled", "Expand 3D View Only" },
         { "wide_background", "Expand 3D View + Stretch 2D Background" },
         { NULL, NULL },
      },
      "disabled"
   },
#ifdef HAVE_CRT_COLOURS
   {
      "supermodel_crt_colors",
      "CRT Colour",
      NULL,
      "Apply the renderer's native colour and gamma correction. Applies only to the New3D renderer.",
      NULL,
      "video",
      {
         { "0", "None (Default)" },
         { "1", "ARI/D93 (Japan)" },
         { "2", "PVM-20M2U/D93" },
         { "3", "BT.601 525/D93" },
         { "4", "BT.601 525/D65 (USA)" },
         { "5", "BT.601 625/D65 (Europe/Australia)" },
         { NULL, NULL },
      },
      "0"
   },
#endif
   {
      "supermodel_no_white_flash",
      "Disable White Flash",
      NULL,
      "Suppress the white flash produced when 3D rendering is disabled",
      NULL,
      "video",
      {
         { "disabled", "OFF (Default)" },
         { "enabled",  "ON" },
         { NULL, NULL },
      },
      "disabled"
   },
   // Cadence and diagnostics.
   {
      "supermodel_av_timing",
      "A/V Timing (Restart Required)",
      NULL,
      "Select the video cadence and corresponding audio timing.",
      NULL,
      "video",
      {
         { "60hz",   "60 Hz (Default)" },
         { "native", "57.524160 Hz (Native Model 3)" },
         { NULL, NULL },
      },
      "60hz"
   },
   {
      "supermodel_timing_overlay",
      "Timing / FPS Overlay",
      NULL,
      "Display timing and frame-rate measurements to inspect emulation and rendering performance.",
      NULL,
      "video",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   // Audio: sound enable, output levels, then the SCSP processing choice.
   {
      "supermodel_sound_enable",
      "Sound Enable",
      NULL,
      "Enable or disable sound emulation.",
      NULL,
      "audio",
      {
         { "enabled",  NULL },
         { "disabled", NULL },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "supermodel_sound_volume",
      "Sound Volume",
      NULL,
      "Adjust the SCSP sound output level. The separate MPEG music board is controlled by Music Volume.",
      NULL,
      "audio",
      {
         { "0",   "0%" },
         { "10",  "10%" },
         { "20",  "20%" },
         { "30",  "30%" },
         { "40",  "40%" },
         { "50",  "50%" },
         { "60",  "60%" },
         { "70",  "70%" },
         { "80",  "80%" },
         { "90",  "90%" },
         { "100", "100%" },
         { "110", "110%" },
         { "120", "120%" },
         { "130", "130%" },
         { "140", "140%" },
         { "150", "150%" },
         { "160", "160%" },
         { "170", "170%" },
         { "180", "180%" },
         { "190", "190%" },
         { "200", "200%" },
         { NULL, NULL },
      },
      "100"
   },
   {
      "supermodel_music_volume",
      "Music Volume",
      NULL,
      "Adjust the output level of the separate MPEG music board.",
      NULL,
      "audio",
      {
         { "0",   "0%" },
         { "10",  "10%" },
         { "20",  "20%" },
         { "30",  "30%" },
         { "40",  "40%" },
         { "50",  "50%" },
         { "60",  "60%" },
         { "70",  "70%" },
         { "80",  "80%" },
         { "90",  "90%" },
         { "100", "100%" },
         { "110", "110%" },
         { "120", "120%" },
         { "130", "130%" },
         { "140", "140%" },
         { "150", "150%" },
         { "160", "160%" },
         { "170", "170%" },
         { "180", "180%" },
         { "190", "190%" },
         { "200", "200%" },
         { NULL, NULL },
      },
      "100"
   },
   {
      "supermodel_scsp_dsp",
      "SCSP DSP Engine",
      NULL,
      "Select the SCSP DSP implementation used for audio processing.",
      NULL,
      "audio",
      {
         { "new",    "New (MAME, Default)" },
         { "legacy", "Legacy (ElSemi)" },
         { NULL, NULL },
      },
      "new"
   },
   // Input: profile and rumble, Lightgun, Gear, Driving, then game-specific controls.
   {
      "supermodel_controller_profile_type",
      "Controller Profile Type",
      NULL,
      "Select whether controller profiles include Test/Service inputs while retaining gameplay controls and remapping.",
      NULL,
      "input",
      {
         { "normal", "Normal" },
         { "test_service", "Normal + Test/Service" },
         { NULL, NULL },
      },
      "test_service"
   },
   {
      "supermodel_force_feedback",
      "Gamepad Rumble",
      NULL,
      "Enable gamepad vibration using the game's force-feedback or rumble effects.",
      NULL,
      "input",
      {
         { "enabled",  "ON (Default)" },
         { "disabled", "Disable" },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "supermodel_crosshairs",
      "Show Crosshair",
      NULL,
      "Select external crosshair visibility for each player, or use the automatic native-reticle behavior.",
      NULL,
      "input",
      {
         { "auto", "Automatic" },
         { "0", "Disabled" },
         { "1", "Player 1 Only" },
         { "2", "Player 2 Only" },
         { "3", "Players 1 & 2" },
         { NULL, NULL },
      },
      "auto"
   },
   {
      "supermodel_gun_input",
      "Gun Input Mode",
      NULL,
      "Select which input devices control the aiming cursor, including combined and dedicated input modes.",
      NULL,
      "input",
      {
         { "hybrid",   "Standard" },
         { "lightgun", "Lightgun Only" },
         { "mouse_analog", "Mouse + Analog Stick" },
         { "mouse",    "Mouse Only" },
         { "analog",   "Analog Stick Only" },
         { NULL, NULL },
      },
      "hybrid"
   },
   {
      "supermodel_offscreen_reload_shortcut",
      "Off-Screen Reload Shortcut",
      NULL,
      "Enable dedicated inputs that force an off-screen reload while retaining native lightgun off-screen firing.",
      NULL,
      "input",
      {
         { "enabled",  "Enabled" },
         { "disabled", "Disabled" },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "supermodel_mouse_edge_offscreen_reload",
      "Mouse Edge Off-Screen Reload",
      NULL,
      "Allow a mouse shot at the screen edge to trigger off-screen reload.",
      NULL,
      "input",
      {
         { "disabled", "Disabled" },
         { "enabled",  "Enabled" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "supermodel_four_speed_shifter",
      "4-Speed Shifter",
      NULL,
      "Select direct directional gear selection or diagonal analog-stick H-gate shifting. Applies only to 4-speed driving games.",
      NULL,
      "input",
      {
         { "h_gate",   "H-Gate Mode" },
         { "standard", "Standard" },
         { NULL, NULL },
      },
      "h_gate"
   },
   {
      "supermodel_steering_response",
      "Driving Steering Response",
      NULL,
      "Select the steering response curve to adjust sensitivity around the center while retaining full travel.",
      NULL,
      "input",
      {
         { "linear",      "Linear" },
         { "progressive", "Progressive (Fine Center)" },
         { "fbneo",       "FBNeo Logarithmic (Fine Center)" },
         { NULL, NULL },
      },
      "linear"
   },
   {
      "supermodel_steering_output_range",
      "Driving Steering Output Range",
      NULL,
      "Scale steering output around its center to adjust the input travel needed for full lock.",
      NULL,
      "input",
      {
         { "50",  "50%" },
         { "60",  "60%" },
         { "63",  "63% (30-80-D0)" },
         { "70",  "70%" },
         { "80",  "80%" },
         { "90",  "90%" },
         { "100", "100%" },
         { "110", "110%" },
         { "120", "120%" },
         { "130", "130%" },
         { "140", "140%" },
         { "150", "150%" },
         { NULL, NULL },
      },
      "100"
   },
   {
      "supermodel_accelerator_output_range",
      "Driving Accelerator Output Range",
      NULL,
      "Scale accelerator output from its released position to adjust the input travel needed for full acceleration.",
      NULL,
      "input",
      {
         { "50",   "50%" },
         { "60",   "60%" },
         { "70",   "70%" },
         { "75.3", "75.3% (00-C0)" },
         { "80",   "80%" },
         { "90",   "90%" },
         { "100",  "100%" },
         { "110",  "110%" },
         { "120",  "120%" },
         { "130",  "130%" },
         { "140",  "140%" },
         { "150",  "150%" },
         { NULL, NULL },
      },
      "100"
   },
   {
      "supermodel_brake_output_range",
      "Driving Brake Output Range",
      NULL,
      "Scale brake output from its released position to adjust the input travel needed for full braking.",
      NULL,
      "input",
      {
         { "50",   "50%" },
         { "60",   "60%" },
         { "70",   "70%" },
         { "75.3", "75.3% (00-C0)" },
         { "80",   "80%" },
         { "90",   "90%" },
         { "100",  "100%" },
         { "110",  "110%" },
         { "120",  "120%" },
         { "130",  "130%" },
         { "140",  "140%" },
         { "150",  "150%" },
         { NULL, NULL },
      },
      "100"
   },
   {
      "supermodel_star_wars_input",
      "Star Wars Trilogy: Input Mode",
      NULL,
      "Select the input devices used to control the arcade analog joystick.",
      NULL,
      "input",
      {
         { "hybrid",       "Standard" },
         { "lightgun",     "Lightgun Only" },
         { "mouse_analog", "Mouse + Analog Stick" },
         { "mouse",        "Mouse Only" },
         { "analog",       "Analog Stick Only" },
         { NULL, NULL },
      },
      "hybrid"
   },
   {
      "supermodel_star_wars_upright_x_inversion",
      "Star Wars Trilogy: Upright Mode X-Axis Inversion Fix",
      NULL,
      "Invert horizontal analog input for the upright cabinet orientation.",
      NULL,
      "input",
      {
         { "enabled",  "ON" },
         { "disabled", "Disable" },
         { NULL, NULL },
      },
      "enabled"
   },
   // CPU: host threading and frame skipping, then PowerPC execution controls.
   {
      "supermodel_emulation_threading",
      "Emulation Threading",
      NULL,
      "Select how emulation and graphics work are distributed across host threads.",
      NULL,
      "cpu",
      {
         { "single",    "Single Thread" },
         { "multi",     "Multi-threaded" },
         { "multi_gpu", "Multi-threaded + GPU (Default)" },
         { NULL, NULL },
      },
      "multi_gpu"
   },
   {
      "supermodel_frameskip",
      "Frame Skip",
      NULL,
      "Skip selected rendered frames to reduce GPU workload.",
      NULL,
      "cpu",
      {
         { "0", "OFF" },
         { "1", "Skip 1 (render every 2nd frame)" },
         { "2", "Skip 2 (render every 3rd frame)" },
         { "3", "Skip 3 (render every 4th frame)" },
         { NULL, NULL },
      },
      "0"
   },
   {
      "supermodel_ppc_frequency",
      "PowerPC CPU Frequency",
      NULL,
      "Adjust the emulated PowerPC clock frequency to balance cycle accuracy and performance.",
      NULL,
      "cpu",
      {
         { "auto", "Auto (Default)" },
         { "25",   "25 MHz" },
         { "33",   "33 MHz (Half Speed - Aggressive)" },
         { "50",   "50 MHz (0.75x Speed)" },
         { "66",   "66 MHz (Step 1.0 Default)" },
         { "70",   "70 MHz (Fast - Underclocked)" },
         { "100",  "100 MHz (Step 1.5 Default)" },
         { "133",  "133 MHz (2.0x Base)" },
         { "166",  "166 MHz (Step 2.x Default)" },
         { "200",  "200 MHz (Max)" },
         { NULL, NULL },
      },
      "auto"
   },
#ifdef __aarch64__
   {
      "supermodel_jit_enable",
      "JIT Recompiler (ARM64)",
      NULL,
      "Enable ARM64 just-in-time compilation of the emulated PowerPC CPU. Applies only to ARM64 hosts.",
      NULL,
      "cpu",
      {
         { "enabled",  "Enabled" },
         { "disabled", "Disabled (Interpreter)" },
         { NULL, NULL },
      },
      "enabled"
   },
#endif
   { NULL, NULL, NULL, NULL, NULL, NULL, {{NULL, NULL}}, NULL },
};

// --- Helper: Read Core Option ---
static const char* option_get(const char* key, const char* default_value)
{
   struct retro_variable var = { key, NULL };
   if (environ_cb && environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
      return var.value;
   return default_value;
}

// Cache for parsed options
#include "CoreOptionsTypes.h"

// --- Update Core Options ---
void update_core_options(void)
{
   g_options.initial_nvram_setup =
      strcmp(option_get("supermodel_initial_nvram_setup", "enabled"),
             "enabled") == 0;

   g_options.nvram_settings_enabled =
      strcmp(option_get("supermodel_nvram_settings", "disabled"),
             "enabled") == 0;

   const char* resolution = option_get("supermodel_resolution", "native");
   if (strcmp(resolution, "half") == 0)
      g_options.resolution_multiplier = 0.5f;
   else if (strcmp(resolution, "2x") == 0)
      g_options.resolution_multiplier = 2.0f;
   else if (strcmp(resolution, "3x") == 0)
      g_options.resolution_multiplier = 3.0f;
   else if (strcmp(resolution, "4x") == 0)
      g_options.resolution_multiplier = 4.0f;
   else
      g_options.resolution_multiplier = 1.0f;  // native

#if defined(USE_LEGACY3D)
   g_options.renderer_3d = Renderer3D::Legacy3D;
#elif defined(HAVE_LEGACY3D)
   g_options.renderer_3d =
      strcmp(option_get("supermodel_renderer_3d", "new3d"),
             "legacy3d") == 0
         ? Renderer3D::Legacy3D : Renderer3D::New3D;
#else
   g_options.renderer_3d = Renderer3D::New3D;
#endif

#ifdef HAVE_LEGACY3D
   g_options.legacy_multi_texture =
      strcmp(option_get("supermodel_legacy_multi_texture", "disabled"),
             "enabled") == 0;
#else
   g_options.legacy_multi_texture = false;
#endif

#ifdef HAVE_QUAD_RENDERING
   g_options.quad_rendering =
      strcmp(option_get("supermodel_quad_rendering", "disabled"),
             "enabled") == 0;
#else
   g_options.quad_rendering = false;
#endif

#ifdef HAVE_CRT_COLOURS
   g_options.crt_colors = atoi(option_get("supermodel_crt_colors", "0"));
   if (g_options.crt_colors < 0 || g_options.crt_colors > 5)
      g_options.crt_colors = 0;
#else
   g_options.crt_colors = 0;
#endif

#ifdef HAVE_SUPERSAMPLING
   g_options.supersampling =
      atoi(option_get("supermodel_supersampling", "1"));
   if (g_options.supersampling < 1 || g_options.supersampling > 8)
      g_options.supersampling = 1;
#else
   g_options.supersampling = 1;
#endif

   g_options.upscale_mode = atoi(option_get("supermodel_upscale_mode", "2"));
   if (g_options.upscale_mode < 0 || g_options.upscale_mode > 3)
      g_options.upscale_mode = 2;

   g_options.timing_overlay = strcmp(option_get("supermodel_timing_overlay", "disabled"), "enabled") == 0;

   {
      const char *aspect = option_get("supermodel_aspect_ratio", "auto");
      g_options.aspect_ratio = strcmp(aspect, "4_3") == 0
         ? AspectRatioMode::FourThree
         : strcmp(aspect, "16_9") == 0
            ? AspectRatioMode::SixteenNine : AspectRatioMode::Auto;
      const char *widescreen = option_get(
         "supermodel_wide_screen", "disabled");
      g_options.widescreen_mode =
         strcmp(widescreen, "wide_background") == 0
            ? WidescreenMode::WidescreenWideBackground
         : strcmp(widescreen, "enabled") == 0
            ? WidescreenMode::Widescreen
            : WidescreenMode::Disabled;
   }
   g_options.no_white_flash =
      strcmp(option_get("supermodel_no_white_flash", "disabled"),
             "enabled") == 0;
   g_options.av_timing_mode =
      strcmp(option_get("supermodel_av_timing", "60hz"), "native") == 0
         ? AVTimingMode::Native57524Hz
         : AVTimingMode::Default60Hz;
   {
      const char *crosshairs = option_get("supermodel_crosshairs", "auto");
      // Accept the values used by earlier development builds so an existing
      // .opt file cannot leave the option in an undefined state.
      g_options.crosshairs = strcmp(crosshairs, "auto") == 0 ? CROSSHAIRS_AUTOMATIC
                           : strcmp(crosshairs, "enabled") == 0 ? 3u
                           : strcmp(crosshairs, "disabled") == 0 ? 0u
                           : static_cast<unsigned>(atoi(crosshairs)) & 3u;
   }

   g_options.sound_enable = strcmp(option_get("supermodel_sound_enable", "enabled"), "enabled") == 0;
   g_options.sound_volume = atoi(option_get("supermodel_sound_volume", "100"));
   g_options.music_volume = atoi(option_get("supermodel_music_volume", "100"));
   g_options.legacy_sound_dsp =
      strcmp(option_get("supermodel_scsp_dsp", "new"), "legacy") == 0;

   {
      const char *gun_input = option_get("supermodel_gun_input", "hybrid");
      g_options.gun_input = strcmp(gun_input, "lightgun") == 0 ? GunInput::Lightgun
                          : strcmp(gun_input, "mouse") == 0    ? GunInput::Mouse
                          : strcmp(gun_input, "mouse_analog") == 0
                                                               ? GunInput::MouseAnalog
                          : strcmp(gun_input, "analog") == 0   ? GunInput::AnalogSticks
                                                               : GunInput::Hybrid;
   }
   g_options.offscreen_trigger_reload =
      strcmp(option_get("supermodel_offscreen_reload_shortcut", "enabled"),
             "enabled") == 0;
   g_options.mouse_edge_offscreen_reload =
      strcmp(option_get("supermodel_mouse_edge_offscreen_reload", "disabled"),
             "enabled") == 0;
   {
      const char *star_wars_input = option_get(
         "supermodel_star_wars_input", "hybrid");
      g_options.star_wars_input =
         strcmp(star_wars_input, "lightgun") == 0
            ? StarWarsInput::Lightgun
         : strcmp(star_wars_input, "mouse_analog") == 0
            ? StarWarsInput::MouseAnalog
         : strcmp(star_wars_input, "mouse") == 0
            ? StarWarsInput::Mouse
         : strcmp(star_wars_input, "analog") == 0
            ? StarWarsInput::AnalogSticks
            : StarWarsInput::Hybrid;
   }
   g_options.star_wars_upright_x_inversion =
      strcmp(option_get("supermodel_star_wars_upright_x_inversion", "enabled"),
             "enabled") == 0;
   g_options.four_speed_shifter =
      strcmp(option_get("supermodel_four_speed_shifter", "h_gate"),
             "h_gate") == 0
         ? FourSpeedShifter::HGate
         : FourSpeedShifter::Standard;
   g_options.force_feedback =
      strcmp(option_get("supermodel_force_feedback", "enabled"),
             "enabled") == 0;
   {
      const char *response = option_get("supermodel_steering_response", "linear");
      g_options.steering_response = strcmp(response, "progressive") == 0
                                      ? SteeringResponse::Progressive
                                   : strcmp(response, "fbneo") == 0
                                      ? SteeringResponse::FBNeoLogarithmic
                                      : SteeringResponse::Linear;
   }
   g_options.steering_output_range = atoi(option_get("supermodel_steering_output_range", "100"));
   {
      const char *accelerator_range = option_get("supermodel_accelerator_output_range", "100");
      const char *brake_range = option_get("supermodel_brake_output_range", "100");
      g_options.accelerator_output_range_per_mille =
         strcmp(accelerator_range, "75.3") == 0 ? 753 : atoi(accelerator_range) * 10;
      g_options.brake_output_range_per_mille =
         strcmp(brake_range, "75.3") == 0 ? 753 : atoi(brake_range) * 10;
   }

   // Parse frame skip option
   g_options.frameskip = atoi(option_get("supermodel_frameskip", "0"));
   if (g_options.frameskip < 0) g_options.frameskip = 0;
   if (g_options.frameskip > 3) g_options.frameskip = 3;

   // Parse PowerPC frequency option
   const char* ppc_freq = option_get("supermodel_ppc_frequency", "auto");
   if (strcmp(ppc_freq, "auto") == 0)
      g_options.ppc_frequency = 0;
   else
   {
      int mhz = atoi(ppc_freq);
      g_options.ppc_frequency = (mhz > 0) ? mhz : 0;
   }

   {
      const char *threading = option_get(
         "supermodel_emulation_threading", "multi_gpu");
      g_options.emulation_threading =
         strcmp(threading, "single") == 0
            ? EmulationThreading::SingleThread
         : strcmp(threading, "multi") == 0
            ? EmulationThreading::MultiThreaded
            : EmulationThreading::MultiThreadedGPU;
   }

#ifdef __aarch64__
   g_options.jit_enable = strcmp(option_get("supermodel_jit_enable", "enabled"), "enabled") == 0;
#else
   g_options.jit_enable = false;
#endif

}
