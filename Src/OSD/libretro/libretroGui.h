#ifndef _LIBRETRO_GUI_H_
#define _LIBRETRO_GUI_H_

#include <string>
#include <vector>
#include <map>
#include "../../Util/NewConfig.h"
#include "../../Model3/Model3.h"
#include "GameLoader.h"

class CLibretroInputSystem;

// Satisfies the call in LibretroWrapper::Emulate
std::vector<std::string> RunGUI(const std::string& configPath, Util::Config::Node& config);

// The actual frame-render call to be placed in your retro_run
void Libretro_UpdateGUI(Util::Config::Node& config,
                        const std::map<std::string, Game>& games,
                        bool& menuOpen,
                        CLibretroInputSystem* inputSys);

void Libretro_ShutdownGUI();

// In-game timing overlay: call after glBlitFramebuffer, before video_cb
void Libretro_InitOverlay(const char* glslVersion);
void Libretro_ShutdownOverlay();
void Libretro_DrawTimingOverlay(const FrameTimings& t, int displayW, int displayH, float gpuMs = 0.0f);

#endif