#include <iostream>
#include <new>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <chrono>
#include <GL/glew.h>
#if !defined(ANDROID) && !defined(CORE_GLES)
#include <glsym/rglgen.h>
#endif
#include <libretro.h>
#include "Inputs/Inputs.h"

#ifdef SUPERMODEL_WIN32
#include "DirectInputSystem.h"
#include "WinOutputs.h"
#endif

#include "Supermodel.h"
#include "OSD/libretro/CoreOptionsTypes.h"
#include "Util/Format.h"
#include "Util/NewConfig.h"
#include "Util/ConfigBuilders.h"
#include "OSD/FileSystemPath.h"
#include "GameLoader.h"
#include "libretro_cbs.h"
#include "vfs_ioapi.h"
#include "Debugger/SupermodelDebugger.h"
#if !defined(ANDROID) && !defined(CORE_GLES) || defined(USE_LEGACY3D)
#include "Graphics/Legacy3D/Legacy3D.h"
#endif
#include "Graphics/New3D/New3D.h"
#include "Model3/IEmulator.h"
#include "Model3/Model3.h"
#include "OSD/Audio.h"
#include "Graphics/New3D/VBO.h"
#include "Graphics/SuperAA.h"
#include "Sound/MPEG/MpegAudio.h"
#include "Util/BMPFile.h"
#include "OSD/DefaultConfigFile.h"
#include "libretroCrosshair.h"
#include "LibretroWrapper.h"
#include "CLibretroInputSystem.h"
#include "CLibretroOutputSystem.h"
#include "libretroGui.h"
#include "LibretroConfigProvider.h"
#include "CoreOptionsTypes.h"
#include "BundledGamesXml.h"
#include "BundledSupermodelIni.h"

// --- External Audio Hooks ---
extern void PlayCallback(void *userdata, UINT8 *stream, int len);
extern UINT32 GetAvailableAudioLen();
extern int bytes_per_frame_host;
extern retro_environment_t environ_cb;
extern retro_log_printf_t log_cb;  // defined in libretro.cpp

#ifdef ANDROID
#include <android/log.h>
#endif
#define LOG_TAG "SupermodelCore"

// --- RetroArch Logger Bridge ---
class CRetroArchLogger : public CLogger
{
public:
    void DebugLog(const char *fmt, va_list vl) override;
    void InfoLog(const char *fmt, va_list vl) override;
    void ErrorLog(const char *fmt, va_list vl) override;
};

void CRetroArchLogger::DebugLog(const char *fmt, va_list vl)
{
#ifndef DEBUG
    (void)fmt; (void)vl;
    return;
#endif
#ifdef ANDROID
    va_list vl_copy;
    va_copy(vl_copy, vl);
    __android_log_vprint(ANDROID_LOG_DEBUG, LOG_TAG, fmt, vl_copy);
    va_end(vl_copy);
#endif
    if (!log_cb) return;
    char buf[2048];
    vsnprintf(buf, sizeof(buf), fmt, vl);
    log_cb(RETRO_LOG_DEBUG, "%s", buf);
}

void CRetroArchLogger::InfoLog(const char *fmt, va_list vl)
{
#ifdef ANDROID
    va_list vl_copy;
    va_copy(vl_copy, vl);
    __android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, fmt, vl_copy);
    va_end(vl_copy);
#endif
    if (!log_cb) return;
    char buf[2048];
    vsnprintf(buf, sizeof(buf), fmt, vl);
    log_cb(RETRO_LOG_INFO, "%s\n", buf);
}

void CRetroArchLogger::ErrorLog(const char *fmt, va_list vl)
{
#ifdef ANDROID
    va_list vl_copy;
    va_copy(vl_copy, vl);
    __android_log_vprint(ANDROID_LOG_ERROR, LOG_TAG, fmt, vl_copy);
    va_end(vl_copy);
#endif
    if (!log_cb) return;
    char buf[2048];
    vsnprintf(buf, sizeof(buf), fmt, vl);
    log_cb(RETRO_LOG_ERROR, "%s\n", buf);
}

/******************************************************************************
 Global Run-time Config
******************************************************************************/

std::string LibretroWrapper::s_analysisPath;
std::string LibretroWrapper::s_configFilePath;
std::string LibretroWrapper::s_gameXMLFilePath;
std::string LibretroWrapper::s_musicXMLFilePath;
std::string LibretroWrapper::s_logFilePath;

static Util::Config::Node s_runtime_config("Global");
static LibretroWrapper* g_ctx = nullptr;

// Constants for synchronous audio
static const double MODEL3_FPS = 57.53;
static const int AUDIO_SAMPLE_RATE = 44100;
// Calculate bytes per frame once: (44100 / 57.53) * 4 bytes (stereo 16-bit)
static const int BYTES_PER_FRAME_SYNC = (int)((AUDIO_SAMPLE_RATE / MODEL3_FPS) * 4);

/*
 * Crosshair stuff
 */
static CCrosshair* s_crosshair = nullptr;

#ifdef ANDROID
#define ALOG(...) __android_log_print(ANDROID_LOG_INFO, "SupermodelCore", __VA_ARGS__)
#else
#define ALOG(...)
#endif

LibretroWrapper::LibretroWrapper() :
    xOffset(0), yOffset(0), xRes(800), yRes(600),
    totalXRes(800), totalYRes(600), aaValue(0), CRTcolors(CRTcolor::None),
    upscaleMode(UpscaleMode::Bilinear)
{
      g_ctx = this;
}

LibretroWrapper::~LibretroWrapper() {}

FrameTimings LibretroWrapper::GetTimings() const
{
    if (Model3) return static_cast<CModel3*>(Model3)->GetTimings();
    return FrameTimings{};
}

static bool FileExists(const std::string& path)
{
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp)
        return false;
    fclose(fp);
    return true;
}

static bool WriteBundledAsset(const std::string& path, const unsigned char* data, unsigned int size)
{
    FILE* fp = fopen(path.c_str(), "wb");
    if (!fp)
        return false;

    const bool written = fwrite(data, 1, size, fp) == size;
    fclose(fp);
    if (written && log_cb)
        log_cb(RETRO_LOG_INFO, "[Supermodel] Extracted bundled asset: %s\n", path.c_str());
    return written;
}

static std::string ResolveSystemAsset(const std::string& systemPath,
                                      const char* fileName,
                                      const unsigned char* bundledData = nullptr,
                                      unsigned int bundledSize = 0)
{
    const std::string preferredPath = systemPath + "/" + fileName;
    if (FileExists(preferredPath))
        return preferredPath;

    // Compatibility with the directory layout used by the initial port.
    const std::string legacyPath = systemPath + "/Config/" + fileName;
    if (FileExists(legacyPath))
        return legacyPath;

    if (bundledData && bundledSize && WriteBundledAsset(preferredPath, bundledData, bundledSize))
        return preferredPath;

    return preferredPath;
}

void LibretroWrapper::InitializePaths(const std::string& systemPath)
{
    // The Supermodel subdirectory is owned by the frontend's system path.
    // Use the negotiated Libretro VFS instead of relying on filesystem helper
    // symbols that are not part of the frontend ABI.
    if (g_vfs_interface && g_vfs_interface->mkdir)
    {
        // VFS mkdir is single-level: create the parent first, then the target.
        const size_t lastSlash = systemPath.rfind('/');
        if (lastSlash != std::string::npos)
            g_vfs_interface->mkdir(systemPath.substr(0, lastSlash).c_str());
        g_vfs_interface->mkdir(systemPath.c_str());
    }

    // External assets remain authoritative. The official embedded assets are
    // retained as a first-run fallback when neither the native nor legacy
    // system-directory layout provides a file.
    s_configFilePath   = ResolveSystemAsset(systemPath, "Supermodel.ini",
                                            bundled_supermodel_ini, bundled_supermodel_ini_len);
    s_gameXMLFilePath  = ResolveSystemAsset(systemPath, "Games.xml",
                                            bundled_games_xml, bundled_games_xml_len);
    s_musicXMLFilePath = ResolveSystemAsset(systemPath, "Music.xml");
    s_logFilePath      = systemPath + "/Supermodel.log";
    s_analysisPath     = systemPath + "/Analysis/";

    std::cout << "[Supermodel] System assets: " << systemPath << std::endl;
}

[[maybe_unused]] static void GLAPIENTRY DebugCallback(GLenum /*source*/, GLenum /*type*/, GLuint id,
                                                      GLenum /*severity*/, GLsizei /*length*/,
                                                      const GLchar* message, const void* /*userParam*/)
{
    printf("OGLDebug:: 0x%X: %s\n", id, message);
}

void LibretroWrapper::UpdateScreenSize(unsigned viewWidth, unsigned viewHeight,
                                       unsigned outputWidth, unsigned outputHeight)
{
    // If dimensions match and renderers are initialized, skip costly re-init
    if (viewWidth == xRes && viewHeight == yRes &&
        outputWidth == totalXRes && outputHeight == totalYRes &&
        superAA != nullptr)
        return;

    // As in standalone Supermodel: the 4:3 game area (xRes x yRes) is centred in the
    // output frame. In widescreen the frame is wider; New3D widens full-screen
    // viewports into the sides and Render2D keeps the 2D layers at 4:3 (or stretches
    // the background layer with WideBackground).
    xRes = std::min(viewWidth, outputWidth);
    yRes = std::min(viewHeight, outputHeight);
    totalXRes = outputWidth;
    totalYRes = outputHeight;
    xOffset = (totalXRes - xRes) / 2;
    yOffset = (totalYRes - yRes) / 2;

    if (Model3)
        Model3->PauseThreads();

    InitRenderers();

    if (Model3)
        Model3->ResumeThreads();
}

void LibretroWrapper::SaveFrameBuffer(const std::string& file)
{
    std::shared_ptr<uint8_t> pixels(new uint8_t[totalXRes * totalYRes * 4], std::default_delete<uint8_t[]>());
    glReadPixels(0, 0, totalXRes, totalYRes, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());
    Util::WriteSurfaceToBMP<Util::RGBA8>(file, pixels.get(), totalXRes, totalYRes, true);
}

void LibretroWrapper::Screenshot()
{
    time_t now = std::time(nullptr);
    tm* ltm = std::localtime(&now);
    std::string file = Util::Format() << FileSystemPath::GetPath(FileSystemPath::Screenshots)
        << "Screenshot_" << std::setfill('0') << std::setw(4) << (1900 + ltm->tm_year)
        << '-' << std::setw(2) << (1 + ltm->tm_mon)
        << '-' << std::setw(2) << ltm->tm_mday
        << "_(" << std::setw(2) << ltm->tm_hour
        << '-' << std::setw(2) << ltm->tm_min
        << '-' << std::setw(2) << ltm->tm_sec
        << ").bmp";

    std::cout << "Screenshot created: " << file << std::endl;
    this->SaveFrameBuffer(file);
}

/******************************************************************************
 Save States and NVRAM
******************************************************************************/

static const int STATE_FILE_VERSION = 6;  // keep in sync with standalone Supermodel
static unsigned s_saveSlot = 0;           // save state slot #

static void SaveState(IEmulator *Model3)
{
  CBlockFile  SaveState;
  std::string file_path = Util::Format() << FileSystemPath::GetPath(FileSystemPath::Saves) << Model3->GetGame().name << ".st" << s_saveSlot;
  
  if (Result::OKAY != SaveState.Create(file_path, "Supermodel Save State", "Supermodel Version " SUPERMODEL_VERSION))
  {
    ErrorLog("Unable to save state to '%s'.", file_path.c_str());
    return;
  }

  int32_t fileVersion = STATE_FILE_VERSION;
  SaveState.Write(&fileVersion, sizeof(fileVersion));
  SaveState.Write(Model3->GetGame().name);

  Model3->SaveState(&SaveState);
  SaveState.Close();
  InfoLog("Saved state to '%s'.", file_path.c_str());
}

static void LoadState(IEmulator *Model3, std::string file_path = std::string())
{
  CBlockFile  SaveState;

  if (file_path.empty())
    file_path = Util::Format() << FileSystemPath::GetPath(FileSystemPath::Saves) << Model3->GetGame().name << ".st" << s_saveSlot;

  if (Result::OKAY != SaveState.Load(file_path))
  {
    ErrorLog("Unable to load state from '%s'.", file_path.c_str());
    return;
  }

  if (Result::OKAY != SaveState.FindBlock("Supermodel Save State"))
  {
    ErrorLog("'%s' does not appear to be a valid save state file.", file_path.c_str());
    return;
  }

  int32_t fileVersion;
  SaveState.Read(&fileVersion, sizeof(fileVersion));
  if (fileVersion != STATE_FILE_VERSION)
  {
    ErrorLog("'%s' is incompatible with this version of Supermodel.", file_path.c_str());
    return;
  }

  Model3->LoadState(&SaveState);
  SaveState.Close();
  InfoLog("Loaded state from '%s'.", file_path.c_str());
}

/******************************************************************************
 Video Callbacks
******************************************************************************/

static CInputs *videoInputs = NULL;
static uint32_t currentInputs = 0;

bool BeginFrameVideo()
{
  return true;
}

void EndFrameVideo()
{
  // Show crosshairs for light gun games
  if (videoInputs && s_crosshair)
  {
    // Render2D/New3D restore framebuffer 0 after drawing to Supermodel's
    // off-screen target. In the standalone build that is the window
    // backbuffer, but in Libretro it is not the image later blitted to the
    // frontend. Rebind Supermodel's target so the crosshair becomes part of
    // the submitted frame.
    glBindFramebuffer(GL_FRAMEBUFFER, g_ctx->getSuperModelFBO());
    s_crosshair->Update(currentInputs, videoInputs, g_ctx->getXOffset(), g_ctx->getYOffset(), g_ctx->getXRes(), g_ctx->getYRes());
  }
}

/******************************************************************************
 Frame Timing & Init
******************************************************************************/
bool LibretroWrapper::InitRenderers()
{
    delete Render2D; Render2D = nullptr;
    delete Render3D; Render3D = nullptr;
    delete superAA;  superAA  = nullptr;

    // Destroy previous libretro-managed FBO if any
    if (m_libretrFBO) {
        glDeleteFramebuffers(1,  &m_libretrFBO);   m_libretrFBO   = 0;
        glDeleteTextures(1,      &m_libretrTex);    m_libretrTex   = 0;
        glDeleteRenderbuffers(1, &m_libretrDepth);  m_libretrDepth = 0;
    }

    superAA = new SuperAA(aaValue, CRTcolors);
    superAA->Init(totalXRes, totalYRes);

    GLuint renderTarget = superAA->GetTargetID();

    // Libretro always needs a base-resolution resolved target. With supersampling
    // or CRT colour active, SuperAA renders its high-resolution input down into
    // this FBO. Without that pass, the Model 3 renderers draw here directly. Never
    // submit SuperAA's high-resolution source FBO to the frontend: it is the
    // unprocessed image, and at 2x or more only its lower-left part would show.
    glGenFramebuffers(1, &m_libretrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_libretrFBO);

    glGenTextures(1, &m_libretrTex);
    glBindTexture(GL_TEXTURE_2D, m_libretrTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 totalXRes, totalYRes,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, m_libretrTex, 0);

    glGenRenderbuffers(1, &m_libretrDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_libretrDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          totalXRes, totalYRes);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, m_libretrDepth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        ErrorLog("[Supermodel] Libretro FBO incomplete: 0x%X", status);
    else
        InfoLog("[Supermodel] Libretro resolved FBO created: %ux%u (id=%u)",
                totalXRes, totalYRes, m_libretrFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (renderTarget != 0)
        superAA->SetOutputTarget(m_libretrFBO);
    else
        renderTarget = m_libretrFBO;

    Render2D = new CRender2D(s_runtime_config);
    // Builds that include both renderers (HAVE_LEGACY3D, from the Makefile) select
    // one at runtime through the 3D Renderer option. Where Legacy3D is not built in,
    // New3D is the only choice; RENDERER=legacy builds are fixed to Legacy3D.
    Render3D =
#if defined(USE_LEGACY3D)
        (IRender3D*)new Legacy3D::CLegacy3D(s_runtime_config);
#elif defined(HAVE_LEGACY3D)
        s_runtime_config["New3DEngine"].ValueAs<bool>()
        ? (IRender3D*)new New3D::CNew3D(s_runtime_config, Model3->GetGame().name)
        : (IRender3D*)new Legacy3D::CLegacy3D(s_runtime_config);
#else
        (IRender3D*)new New3D::CNew3D(s_runtime_config, Model3->GetGame().name);
#endif

     unsigned render_xRes = xRes;
    unsigned render_xOffset = xOffset;

    if (Result::OKAY != Render2D->Init(
            render_xOffset*aaValue, yOffset*aaValue,
            render_xRes*aaValue, yRes*aaValue,
            totalXRes*aaValue, totalYRes*aaValue,
            renderTarget, upscaleMode))   // ← use renderTarget, not superAA->GetTargetID()
        return false;

    if (Result::OKAY != Render3D->Init(
            render_xOffset*aaValue, yOffset*aaValue,
            render_xRes*aaValue, yRes*aaValue,
            totalXRes*aaValue, totalYRes*aaValue,
            renderTarget))               // ← same here
        return false;

    // Initialize crosshair if it exists and hasn't been initialized yet
    if (s_crosshair)
    {
        if (s_crosshair->Init() != Result::OKAY)
        {
            InfoLog("[Supermodel] Crosshair initialization failed (Assets missing?), falling back to vector mode.");
        }
    }

    Model3->AttachRenderers(Render2D, Render3D, superAA);
    return true;
}

int LibretroWrapper::SuperModelInit(const Game &game) {

  initialState = s_runtime_config["InitStateFile"].ValueAs<std::string>();

  gameHasLightguns = false;
  quit = false;
  paused = false;
  dumpTimings = false;

  // Initialize and load ROMs
  if (Result::OKAY != Model3->Init())
    return 1;
  if (Model3->LoadGame(game, rom_set) != Result::OKAY)
    return 1;
  rom_set = ROMSet();  

  MpegDec::LoadCustomTracks(s_musicXMLFilePath, game);

  totalXRes = xRes = s_runtime_config["XResolution"].ValueAs<unsigned>();
  totalYRes = yRes = s_runtime_config["YResolution"].ValueAs<unsigned>();
  snprintf(baseTitleStr, sizeof(baseTitleStr), "Supermodel - %s", game.title.c_str());

  SetAudioType(game.audio);
  if (Result::OKAY != OpenAudio(s_runtime_config))
    return 1;

  gameHasLightguns = !!(game.inputs &
    (Game::INPUT_GUN1 | Game::INPUT_GUN2 |
     Game::INPUT_ANALOG_GUN1 | Game::INPUT_ANALOG_GUN2));
  currentInputs = game.inputs;
  
  if (gameHasLightguns)
    videoInputs = Inputs;
  else
    videoInputs = nullptr;

  // --- FORCE FEEDBACK & OUTPUTS INITIALIZATION ---
  
  // 1. Initialize the concrete Output system if it hasn't been already
  if (Outputs == nullptr) {
      Outputs = new CLibretroOutputSystem();
  }

  // 2. Fetch the Rumble Interface from the Libretro environment
  // (Assuming you have access to the environment callback here)
  retro_rumble_interface rumble;
  if (environ_cb && environ_cb(RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE, &rumble)) {
      // We must cast the IInputSystem pointer to our specific Libretro implementation
      // to access the rumble setter.
      auto* libInput = static_cast<CLibretroInputSystem*>(m_inputSystem.get());
      if (libInput) {
          libInput->SetRumbleInterface(rumble);
          printf("[Supermodel] Libretro Rumble Interface initialized.\n");
      }
  }

  // 3. Attach everything to the Model 3 Core
  Model3->AttachInputs(Inputs);
  Model3->AttachOutputs(Outputs);
  CModel3* m3 = dynamic_cast<CModel3*>(Model3);
  if (m3 && log_cb)
    log_cb(RETRO_LOG_INFO, "[Supermodel] DriveBoard ptr=%p IsAttached=%d\n", 
           (void*)m3->GetDriveBoard(), m3->GetDriveBoard()->IsAttached() ? 1 : 0);
  
  Model3->Reset();

  if (!initialState.empty())
    LoadState(Model3, initialState);

  fpsFramesElapsed = 0;
  return 0;
}

int LibretroWrapper::Supermodel(const Game &game, bool skipRender)
{
#ifndef SUPERMODEL_DEBUGGER
    (void)game;
#endif
    const auto engineStart = std::chrono::steady_clock::now();
    if (paused)
    {
        Model3->RenderFrame();
        lastEngineMs = std::chrono::duration<float, std::milli>(
            std::chrono::steady_clock::now() - engineStart).count();
        lastAudioSubmitMs = 0.0f;
    }
    else
    {
        Model3->RunFrame(skipRender);

        const auto audioStart = std::chrono::steady_clock::now();
        lastEngineMs = std::chrono::duration<float, std::milli>(
            audioStart - engineStart).count();

        // One frame of emulated time is one frame's worth of audio, full stop:
        // 44100 / 57.53 = 766 samples. This used to be computed from the MEASURED
        // framerate instead, which is a positive feedback loop under RetroArch's
        // synchronous audio driver: fps dips -> 44100/fps sends MORE samples than
        // real time -> the driver blocks in audio_batch_cb to absorb the excess ->
        // fps dips further -> even more samples. Measured on Daytona 2: the sample
        // count climbed 766 -> 1047 while audio_batch_cb blocked for up to 36 ms
        // per frame, producing exactly the recurring 30-50 ms frame spikes.
        // The frontend already resamples to keep audio and video in sync; the core
        // must simply hand it one frame of audio per frame.
        PlayCallback(NULL, NULL, BYTES_PER_FRAME_SYNC);
        lastAudioSubmitMs = std::chrono::duration<float, std::milli>(
            std::chrono::steady_clock::now() - audioStart).count();
    }

    if (Inputs->uiExit->Pressed())
    {
      quit = true;
    }
    else if (Inputs->uiReset->Pressed())
    {
      Reset();
    }
    else if (Inputs->uiPause->Pressed())
    {
      paused = !paused;
      if (paused) Model3->PauseThreads();
      else        Model3->ResumeThreads();

      if (Outputs != NULL)
        Outputs->SetValue(OutputPause, paused);
    }
    else if (Inputs->uiSaveState->Pressed())
    {
      if (!paused) Model3->PauseThreads();
      SaveState(Model3);
      if (!paused) Model3->ResumeThreads();
    }
    else if (Inputs->uiChangeSlot->Pressed())
    {
      ++s_saveSlot;
      s_saveSlot %= 10;
      printf("Save slot: %d\n", s_saveSlot);
    }
    else if (Inputs->uiLoadState->Pressed())
    {
      if (!paused) Model3->PauseThreads();
      LoadState(Model3);
      if (!paused) Model3->ResumeThreads();
    }
    else if (Inputs->uiMusicVolUp->Pressed())
    {
      if (!Model3->GetGame().mpeg_board.empty()) {
        int vol = (std::min)(200, s_runtime_config["MusicVolume"].ValueAs<int>() + 10);
        s_runtime_config.Get("MusicVolume").SetValue(vol);
      }
    }
    else if (Inputs->uiMusicVolDown->Pressed())
    {
      if (!Model3->GetGame().mpeg_board.empty()) {
        int vol = (std::max)(0, s_runtime_config["MusicVolume"].ValueAs<int>() - 10);
        s_runtime_config.Get("MusicVolume").SetValue(vol);
      }
    }
    else if (Inputs->uiSoundVolUp->Pressed())
    {
      int vol = (std::min)(200, s_runtime_config["SoundVolume"].ValueAs<int>() + 10);
      s_runtime_config.Get("SoundVolume").SetValue(vol);
    }
    else if (Inputs->uiSoundVolDown->Pressed())
    {
      int vol = (std::max)(0, s_runtime_config["SoundVolume"].ValueAs<int>() - 10);
      s_runtime_config.Get("SoundVolume").SetValue(vol);
    }
#ifdef SUPERMODEL_DEBUGGER
    else if (Inputs->uiDumpInpState->Pressed())
    {
      Inputs->DumpState(&game);
    }
    else if (Inputs->uiDumpTimings->Pressed())
    {
      dumpTimings = !dumpTimings;
    }
#endif
    else if (Inputs->uiSelectCrosshairs->Pressed() && gameHasLightguns)
    {
      int crosshairs = (s_runtime_config["Crosshairs"].ValueAs<unsigned>() + 1) & 3;
      s_runtime_config.Get("Crosshairs").SetValue(crosshairs);
    }
    else if (Inputs->uiClearNVRAM->Pressed())
    {
      Model3->ClearNVRAM();
      puts("NVRAM cleared.");
    }
    else if (Inputs->uiToggleFrLimit->Pressed())
    {
      s_runtime_config.Get("Throttle").SetValue(!s_runtime_config["Throttle"].ValueAs<bool>());
    }
    else if (Inputs->uiScreenshot->Pressed())
    {
      Screenshot();
    }

   if (s_runtime_config["ShowFrameRate"].ValueAs<bool>())
    {
      fpsFramesElapsed += 1;
    }

    if (dumpTimings && !paused)
    {
      CModel3 *M = dynamic_cast<CModel3 *>(Model3);
      if (M) M->DumpTimings();
    }

  return 0;
}

void LibretroWrapper::ShutDownSupermodel()
{
  if (Model3)
    Model3->PauseThreads();

  // Stop rumble after the drive-board thread has paused, while the Libretro
  // input interface is still alive. Otherwise the thread can overwrite an
  // earlier stop command during teardown.
  auto libretroInput = std::static_pointer_cast<CLibretroInputSystem>(m_inputSystem);
  if (libretroInput)
    libretroInput->StopAllRumble();

  // NOTE: NVRAM is now saved by retro_unload_game() to the libretro buffer
  // Don't call SaveNVRAM() here - it would save to a file, which we don't want

  CloseAudio();

  delete Model3;
  Model3 = nullptr;

  delete Inputs;
  Inputs = nullptr;

  delete Outputs;
  Outputs = nullptr;

  m_inputSystem.reset();
  videoInputs = nullptr;
  currentInputs = 0;

  delete Render2D;
  Render2D = nullptr;
  delete Render3D;
  Render3D = nullptr;
  delete superAA;
  superAA = nullptr;

  if (m_libretrFBO)
  {
    glDeleteFramebuffers(1, &m_libretrFBO);
    m_libretrFBO = 0;
  }
  if (m_libretrTex)
  {
    glDeleteTextures(1, &m_libretrTex);
    m_libretrTex = 0;
  }
  if (m_libretrDepth)
  {
    glDeleteRenderbuffers(1, &m_libretrDepth);
    m_libretrDepth = 0;
  }

  delete s_crosshair;
  s_crosshair = nullptr;

  game = Game();
  rom_set = ROMSet();
}

/******************************************************************************
 Entry Point and Command Line Processing
******************************************************************************/

static void WriteDefaultConfigurationFileIfNotPresent()
{
    FILE* fp = fopen(LibretroWrapper::s_configFilePath.c_str(), "r");
    if (fp) { fclose(fp); return; }

    fp = fopen(LibretroWrapper::s_configFilePath.c_str(), "w");
    if (!fp)
    {
        ErrorLog("Unable to write default configuration file to %s", LibretroWrapper::s_configFilePath.c_str());
        return;
    }
    fputs(s_defaultConfigFileContents, fp);
    fclose(fp);
}

// Create and configure inputs
Result LibretroWrapper::ConfigureInputs(CInputs *Inputs, Util::Config::Node *fileConfig, Util::Config::Node *runtimeConfig, const Game &game, bool configure)
{
  static constexpr char configFileComment[] = {
    ";\n"
    "; Supermodel Configuration File\n"
    ";\n"
  };

  Inputs->LoadFromConfig(*runtimeConfig);

  if (configure)
  {
    Util::Config::Node *fileConfigRoot = game.name.empty() ? fileConfig : fileConfig->TryGet(game.name);
    if (fileConfigRoot == nullptr)
      fileConfigRoot = &fileConfig->Add(game.name);

    if (Inputs->ConfigureInputs(game, xOffset, yOffset, xRes, yRes))
    {
      Inputs->StoreToConfig(fileConfigRoot);
      Util::Config::WriteINIFile(s_configFilePath, *fileConfig, configFileComment);
      Inputs->StoreToConfig(runtimeConfig);
    }
  }

  return Result::OKAY;
}

// Same sequence as the uiReset hotkey handler: the render/sound threads must be parked
// while the machine is reset, or they keep touching state that Reset() is tearing down.
void LibretroWrapper::Reset()
{
    if (!Model3)
        return;
    if (!paused) Model3->PauseThreads();
    Model3->Reset();
    if (!paused) Model3->ResumeThreads();
    InfoLog("Model 3 reset.");
}

int LibretroWrapper::Emulate(const char* romPath)
{
    WriteDefaultConfigurationFileIfNotPresent();

    // Route ALL of Supermodel's logging (InfoLog/DebugLog/ErrorLog, including the
    // DumpTimings profiler output) through the RetroArch log callback, on every
    // platform — not just Android. Previously non-Android builds used a file/console
    // logger via CreateLogger(), so InfoLog never reached the RetroArch frontend log
    // and the profiler lines were invisible.
    auto ra_logger = std::make_shared<CRetroArchLogger>();
    SetLogger(ra_logger);

    char* argv[] = { (char*)"supermodel", (char*)romPath };
    int argc = 2;
    auto cmd_line = LibretroConfigProvider::ParseCommandLine(argc, argv);
    if (cmd_line.error) return 1;

    InfoLog("Supermodel Version " SUPERMODEL_VERSION);
  
    bool rom_specified = !cmd_line.rom_files.empty();
    if (!rom_specified && !cmd_line.print_games && !cmd_line.config_inputs && !cmd_line.print_inputs)
    {
        ErrorLog("No ROM file specified.");
        return 0;
    }
    
    // Load and Merge Configuration
    Util::Config::Node fileConfig("Global");
    {
        Util::Config::Node fileConfigWithDefaults("Global");
        Util::Config::Node config3("Global");
        Util::Config::Node config4("Global");
        Util::Config::FromINIFile(&fileConfig, s_configFilePath);
        Util::Config::MergeINISections(&fileConfigWithDefaults, LibretroConfigProvider::DefaultConfig(s_gameXMLFilePath), fileConfig); 
        Util::Config::MergeINISections(&config3, fileConfigWithDefaults, cmd_line.config);    

	config3.Set("GameXMLFile", s_gameXMLFilePath);
        if (rom_specified || cmd_line.print_games)
        {
            std::string xml_file = config3["GameXMLFile"].ValueAs<std::string>();

            // Set VFS backend for ROM ZIP loading (bypasses Android scoped storage)
            if (g_vfs_interface) {
                static zlib_filefunc64_def vfs_filefunc;
                fill_retro_vfs_filefunc64(&vfs_filefunc, g_vfs_interface);
                GameLoader::SetZipFilefunc(&vfs_filefunc);
            }

            // Load Games.xml via VFS when available (Android scoped storage safe),
            // falling back to the bundled bytes embedded in the binary.
            std::string xml_text;
            bool xml_loaded = false;
            if (g_vfs_interface) {
                auto *f = g_vfs_interface->open(xml_file.c_str(), RETRO_VFS_FILE_ACCESS_READ, RETRO_VFS_FILE_ACCESS_HINT_NONE);
                if (f) {
                    int64_t size = g_vfs_interface->size(f);
                    if (size > 0) {
                        xml_text.resize((size_t)size);
                        g_vfs_interface->read(f, xml_text.data(), (uint64_t)size);
                        xml_loaded = true;
                        log_cb(RETRO_LOG_INFO, "[Supermodel] Loaded Games.xml via VFS (%s)\n", xml_file.c_str());
                    }
                    g_vfs_interface->close(f);
                }
            }
            if (!xml_loaded) {
                log_cb(RETRO_LOG_INFO, "[Supermodel] Using bundled Games.xml\n");
                xml_text.assign(reinterpret_cast<const char *>(bundled_games_xml), bundled_games_xml_len);
            }

            GameLoader loader(xml_text.data(), xml_text.size(), xml_file);
            if (loader.Load(&game, &rom_set, *cmd_line.rom_files.begin()))
                return 1;
            Util::Config::MergeINISections(&config4, config3, fileConfig[game.name]);   
        }
        else
            config4 = config3;
            
        Util::Config::MergeINISections(&s_runtime_config, config4, cmd_line.config);

        // After the merge, so an explicit core option beats the on-disk Supermodel.ini
        LibretroConfigProvider::ApplyDrivingLayout(s_runtime_config);
        LibretroConfigProvider::ApplyGunLayout(s_runtime_config,
            (game.inputs & (Game::INPUT_ANALOG_GUN1 | Game::INPUT_ANALOG_GUN2)) != 0);
        s_runtime_config.Get("UpscaleMode").SetValue(g_options.upscale_mode);
        s_runtime_config.Get("New3DEngine").SetValue(g_options.renderer_3d == Renderer3D::New3D);
        s_runtime_config.Get("Supersampling").SetValue(g_options.supersampling);
        s_runtime_config.Get("CRTcolors").SetValue(
            g_options.renderer_3d == Renderer3D::New3D ? g_options.crt_colors : 0);
        s_runtime_config.Get("LegacySoundDSP").SetValue(g_options.legacy_sound_dsp);
        s_runtime_config.Get("MultiThreaded").SetValue(
            g_options.emulation_threading != EmulationThreading::SingleThread);
        s_runtime_config.Get("GPUMultiThreaded").SetValue(
            g_options.emulation_threading == EmulationThreading::MultiThreadedGPU);
        InfoLog("Libretro emulation threading: MultiThreaded=%d, GPUMultiThreaded=%d",
                s_runtime_config["MultiThreaded"].ValueAs<bool>(),
                s_runtime_config["GPUMultiThreaded"].ValueAs<bool>());
    }

    if (Model3 || Inputs || Outputs)
    {
        ErrorLog("A Supermodel session is already active.");
        return 1;
    }

    aaValue   = s_runtime_config["Supersampling"].ValueAs<int>();
    CRTcolors = (CRTcolor)s_runtime_config["CRTcolors"].ValueAs<int>();   // 0 = None; was never read, left indeterminate
    upscaleMode = static_cast<UpscaleMode>(s_runtime_config["UpscaleMode"].ValueAs<int>());

    auto inputSystem = std::make_shared<CLibretroInputSystem>();
    auto inputs = std::make_unique<CInputs>(inputSystem);

    if (!inputs->Initialize())
    {
      ErrorLog("Failed to initialize input system.");
      return 1;
    }

    // Allocate crosshair object (Initialization deferred to InitRenderers)
    if (s_crosshair) delete s_crosshair;
    s_crosshair = new CCrosshair(s_runtime_config);

    auto model3 = std::make_unique<CModel3>(s_runtime_config);
    if (ConfigureInputs(inputs.get(), &fileConfig, &s_runtime_config, game, cmd_line.config_inputs) != Result::OKAY)
    {
        delete s_crosshair;
        s_crosshair = nullptr;
        return 1;
    }

    if (!rom_specified)
    {
        delete s_crosshair;
        s_crosshair = nullptr;
        return 1;
    }

    // Fire up Supermodel
    m_inputSystem = std::move(inputSystem);
    Model3 = model3.release();
    Inputs = inputs.release();
    return 0;
}

bool LibretroWrapper::InitGL()
{
    static bool glsym_done = false;
    if (!glsym_done)
    {
#if !defined(ANDROID) && !defined(CORE_GLES)
        rglgen_resolve_symbols(hw_render.get_proc_address);
#endif
        glsym_done = true;
    }

    // CRITICAL: Ensure internal textures (fonts, UI) are 1-byte aligned 
    // to match the fix in libretro.cpp. This prevents artifacts on legacy GL drivers.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glDisable(GL_DITHER);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return InitRenderers();
}

GLuint LibretroWrapper::getSuperModelFBO() const
{
    return m_libretrFBO;
}

void LibretroWrapper::SetWidescreen(bool enabled, bool wideBackground)
{
    // WideBackground is read by Render2D every frame, so it applies live.
    try {
        s_runtime_config.Get("WideBackground").SetValue(enabled && wideBackground);
    }
    catch (const std::range_error&) {
        s_runtime_config.Add("WideBackground").SetValue(enabled && wideBackground);
    }

    // Check if the value actually changed
    bool current_value = s_runtime_config["WideScreen"].ValueAsDefault<bool>(false);
    if (current_value == enabled) {
        // No change, skip reinitialization
        return;
    }

    try {
        s_runtime_config.Get("WideScreen").SetValue(enabled);
    }
    catch (const std::range_error&) {
        s_runtime_config.Add("WideScreen").SetValue(enabled);
    }

    // Only reinit renderers if they already exist (i.e. GL context is live).
    // On initial load, InitRenderers() will be called later by context_reset()
    // and will pick up the correct WideScreen value from s_runtime_config.
    if (Render2D == nullptr) {
        return;
    }

    if (Model3) Model3->PauseThreads();
    InitRenderers();
    if (Model3) Model3->ResumeThreads();
}

void LibretroWrapper::SetServiceOnSticks(bool enabled)
{
    auto* libretroInput = dynamic_cast<CLibretroInputSystem*>(m_inputSystem.get());
    if (libretroInput)
        libretroInput->SetServiceOnSticks(enabled);
}

// Re-applies the Driving Controls Layout to a running game: rewrite the gear/pedal
// mappings in the runtime config, then have Supermodel re-parse its input mappings, as
// it does when content is loaded. RetroArch's Restart (retro_reset) only resets the
// board, so without this a layout change needed the content to be closed and reloaded.
void LibretroWrapper::ApplyDrivingLayout()
{
    LibretroConfigProvider::ApplyDrivingLayout(s_runtime_config);
    if (!Inputs)
        return;

    const bool pauseThreads = Model3 && !paused;
    if (pauseThreads)
        Model3->PauseThreads();
    Inputs->LoadFromConfig(s_runtime_config);
    if (pauseThreads)
        Model3->ResumeThreads();
}

void LibretroWrapper::SetCrosshairs(unsigned mask)
{
    mask &= 3u;
    try {
        s_runtime_config.Get("Crosshairs").SetValue(static_cast<int>(mask));
    }
    catch (const std::range_error&) {
        s_runtime_config.Add("Crosshairs").SetValue(static_cast<int>(mask));
    }
}

void LibretroWrapper::SetSoundVolume(int volume)
{
    s_runtime_config.Get("SoundVolume").SetValue(volume);
}

void LibretroWrapper::SetMusicVolume(int volume)
{
    s_runtime_config.Get("MusicVolume").SetValue(volume);
}
