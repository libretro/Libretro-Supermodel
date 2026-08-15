LOCAL_PATH := $(call my-dir)

CORE_DIR := $(LOCAL_PATH)/..
COMM_DIR := $(CORE_DIR)/Src/OSD/libretro/libretro-common
DEPS_DIR := $(CORE_DIR)/deps

GIT_VERSION := $(shell git -C $(CORE_DIR) rev-parse --short HEAD 2>/dev/null || echo unknown)

COREFLAGS := \
    -DANDROID -D__LIBRETRO__ -DPSS_STYLE=1 \
    -D_FILE_OFFSET_BITS=64 -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE \
    -DEGL_EGLEXT_PROTOTYPES \
    -DGIT_VERSION=\"$(GIT_VERSION)\" \
    -O3 -DNDEBUG \
    -ffunction-sections -fdata-sections \
    -Wno-write-strings \
    -I$(CORE_DIR) \
    -I$(DEPS_DIR)/ugui \
    -I$(COMM_DIR)/include \
    -I$(CORE_DIR)/Src/OSD/libretro/include \
    -I$(CORE_DIR)/Src/OSD/libretro \
    -I$(CORE_DIR)/Src \
    -I$(CORE_DIR)/Src/CPU/68K/Musashi \
    -I$(CORE_DIR)/Src/CPU/68K/Musashi/generated \
    -I$(CORE_DIR)/Src/OSD/Android/include

SOURCES_C := \
    $(CORE_DIR)/Src/Pkgs/unzip.c \
    $(CORE_DIR)/Src/Pkgs/ioapi.c \
    $(CORE_DIR)/Src/CPU/68K/Musashi/m68kcpu.c \
    $(CORE_DIR)/Src/CPU/68K/Musashi/generated/m68kops.c \
    $(CORE_DIR)/Src/CPU/68K/Musashi/generated/m68kopac.c \
    $(CORE_DIR)/Src/CPU/68K/Musashi/generated/m68kopdm.c \
    $(CORE_DIR)/Src/CPU/68K/Musashi/generated/m68kopnz.c \
    $(DEPS_DIR)/ugui/ugui.c \
    $(CORE_DIR)/Src/ugui_tools.c \
    $(COMM_DIR)/streams/file_stream.c \
    $(COMM_DIR)/streams/file_stream_transforms.c \
    $(COMM_DIR)/file/file_path.c \
    $(COMM_DIR)/file/retro_dirent.c \
    $(COMM_DIR)/vfs/vfs_implementation.c \
    $(COMM_DIR)/lists/dir_list.c \
    $(COMM_DIR)/lists/string_list.c \
    $(COMM_DIR)/string/stdstring.c \
    $(COMM_DIR)/compat/compat_strl.c \
    $(COMM_DIR)/compat/fopen_utf8.c \
    $(COMM_DIR)/compat/compat_strcasestr.c \
    $(COMM_DIR)/compat/compat_posix_string.c \
    $(COMM_DIR)/encodings/encoding_utf.c \
    $(COMM_DIR)/memmap/memalign.c \
    $(COMM_DIR)/time/rtime.c \
    $(COMM_DIR)/hash/rhash.c \
    $(COMM_DIR)/glsym/glsym_es3.c

SOURCES_CXX := \
    $(CORE_DIR)/Src/CPU/PowerPC/PPCDisasm.cpp \
    $(CORE_DIR)/Src/BlockFile.cpp \
    $(CORE_DIR)/Src/Model3/93C46.cpp \
    $(CORE_DIR)/Src/Util/BitRegister.cpp \
    $(CORE_DIR)/Src/Model3/JTAG.cpp \
    $(CORE_DIR)/Src/Pkgs/imgui/imgui.cpp \
    $(CORE_DIR)/Src/Pkgs/imgui/imgui_draw.cpp \
    $(CORE_DIR)/Src/Pkgs/imgui/imgui_tables.cpp \
    $(CORE_DIR)/Src/Pkgs/imgui/imgui_widgets.cpp \
    $(CORE_DIR)/Src/Pkgs/imgui/imgui_impl_opengl3.cpp \
    $(CORE_DIR)/Src/Graphics/Shader.cpp \
    $(CORE_DIR)/Src/Graphics/GLSLVersion.cpp \
    $(CORE_DIR)/Src/Model3/Real3D.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/New3D.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/Mat4.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/Model.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/PolyHeader.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/TextureBank.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/VBO.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/Vec.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/R3DShader.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/R3DFloat.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/R3DScrollFog.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/R3DFrameBuffers.cpp \
    $(CORE_DIR)/Src/Graphics/New3D/GLSLShader.cpp \
    $(CORE_DIR)/Src/Graphics/FBO.cpp \
    $(CORE_DIR)/Src/Graphics/SuperAA.cpp \
    $(CORE_DIR)/Src/Graphics/Render2D.cpp \
    $(CORE_DIR)/Src/Model3/TileGen.cpp \
    $(CORE_DIR)/Src/Model3/Model3.cpp \
    $(CORE_DIR)/Src/CPU/PowerPC/ppc.cpp \
    $(CORE_DIR)/Src/Model3/SoundBoard.cpp \
    $(CORE_DIR)/Src/Sound/SCSP.cpp \
    $(CORE_DIR)/Src/Sound/SCSPDSP.cpp \
    $(CORE_DIR)/Src/CPU/68K/68K.cpp \
    $(CORE_DIR)/Src/Model3/DSB.cpp \
    $(CORE_DIR)/Src/CPU/Z80/Z80.cpp \
    $(CORE_DIR)/Src/Model3/IRQ.cpp \
    $(CORE_DIR)/Src/Model3/53C810.cpp \
    $(CORE_DIR)/Src/Model3/PCI.cpp \
    $(CORE_DIR)/Src/Model3/RTC72421.cpp \
    $(CORE_DIR)/Src/Model3/DriveBoard/DriveBoard.cpp \
    $(CORE_DIR)/Src/Model3/DriveBoard/WheelBoard.cpp \
    $(CORE_DIR)/Src/Model3/DriveBoard/JoystickBoard.cpp \
    $(CORE_DIR)/Src/Model3/DriveBoard/SkiBoard.cpp \
    $(CORE_DIR)/Src/Model3/DriveBoard/BillBoard.cpp \
    $(CORE_DIR)/Src/Model3/MPC10x.cpp \
    $(CORE_DIR)/Src/Inputs/Input.cpp \
    $(CORE_DIR)/Src/Inputs/Inputs.cpp \
    $(CORE_DIR)/Src/Inputs/InputSource.cpp \
    $(CORE_DIR)/Src/Inputs/InputSystem.cpp \
    $(CORE_DIR)/Src/Inputs/InputTypes.cpp \
    $(CORE_DIR)/Src/Inputs/MultiInputSource.cpp \
    $(CORE_DIR)/Src/OSD/Outputs.cpp \
    $(CORE_DIR)/Src/Sound/MPEG/MpegAudio.cpp \
    $(CORE_DIR)/Src/Model3/Crypto.cpp \
    $(CORE_DIR)/Src/OSD/Logger.cpp \
    $(CORE_DIR)/Src/Util/Format.cpp \
    $(CORE_DIR)/Src/Util/NewConfig.cpp \
    $(CORE_DIR)/Src/Util/ByteSwap.cpp \
    $(CORE_DIR)/Src/Util/ConfigBuilders.cpp \
    $(CORE_DIR)/Src/GameLoader.cpp \
    $(CORE_DIR)/Src/Pkgs/tinyxml2.cpp \
    $(CORE_DIR)/Src/ROMSet.cpp \
    $(CORE_DIR)/Src/OSD/libretro/libretroAudio.cpp \
    $(CORE_DIR)/Src/OSD/libretro/libretroThread.cpp \
    $(CORE_DIR)/Src/OSD/libretro/libretroCrosshair.cpp \
    $(CORE_DIR)/Src/OSD/libretro/libretroGui.cpp \
    $(CORE_DIR)/Src/OSD/libretro/LibretroBlockFileMemory.cpp \
    $(CORE_DIR)/Src/OSD/libretro/CLibretroInputSystem.cpp \
    $(CORE_DIR)/Src/OSD/libretro/CLibretroOutputSystem.cpp \
    $(CORE_DIR)/Src/OSD/libretro/LibretroWrapper.cpp \
    $(CORE_DIR)/Src/OSD/libretro/vfs_ioapi.cpp \
    $(CORE_DIR)/Src/OSD/libretro/libretro.cpp \
    $(CORE_DIR)/Src/OSD/Unix/FileSystemPath.cpp

include $(CLEAR_VARS)
LOCAL_MODULE := retro

LOCAL_SRC_FILES := $(SOURCES_C) $(SOURCES_CXX)

# arm64-v8a: JIT dynarec + NEON
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
    LOCAL_SRC_FILES += $(CORE_DIR)/Src/CPU/PowerPC/Jit/JitArm64.cpp
    ARCH_FLAGS := -DHAVE_PPC_JIT -DHAVE_NEON -D__ARM_NEON__
endif

# armeabi-v7a: NEON
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_ARM_NEON  := true
    ARCH_FLAGS      := -DHAVE_NEON -D__ARM_NEON__ \
                       -march=armv7-a -mfloat-abi=softfp -mfpu=neon
endif

LOCAL_CFLAGS   := $(COREFLAGS) $(ARCH_FLAGS)
LOCAL_CXXFLAGS := $(COREFLAGS) $(ARCH_FLAGS) -std=c++17
LOCAL_LDFLAGS  := -Wl,--no-undefined -Wl,-version-script,$(CORE_DIR)/link.T
LOCAL_LDLIBS   := -lGLESv3 -llog -lz

include $(BUILD_SHARED_LIBRARY)
