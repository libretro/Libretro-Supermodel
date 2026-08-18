#ifndef SUPERMODEL_LIBRETRO_TIMING_H
#define SUPERMODEL_LIBRETRO_TIMING_H

#include <cstdint>

namespace LibretroTiming {

// Supermodel defaults to 60 Hz to avoid judder on ordinary displays, while
// native Model 3 timing is 57.524160 Hz. Libretro always receives the original
// 44.1 kHz audio stream: at 60 Hz that is a fixed 735 samples per frame, the
// sound board's own chunk; native timing packetizes it into a deterministic
// 766/767-sample cadence instead of changing or resampling the stream.
inline constexpr std::uint64_t kMicroHzScale = 1000000;
inline constexpr std::uint64_t kDefaultFrameRateMicroHz = 60000000;
inline constexpr std::uint64_t kNativeFrameRateMicroHz = 57524160;
inline constexpr unsigned kAudioSampleRate = 44100;
inline constexpr unsigned kStereoAudioBytesPerFrame = 2 * sizeof(std::int16_t);

inline constexpr std::uint64_t FrameRateMicroHz(bool nativeTiming)
{
   return nativeTiming ? kNativeFrameRateMicroHz : kDefaultFrameRateMicroHz;
}

inline constexpr double FramesPerSecond(std::uint64_t frameRateMicroHz)
{
   return static_cast<double>(frameRateMicroHz) /
          static_cast<double>(kMicroHzScale);
}

// Audio frames for the next video frame, carrying the fractional remainder so
// that the long-term rate is exactly kAudioSampleRate.
inline unsigned NextAudioFrames(std::uint64_t frameRateMicroHz,
                                std::uint64_t &remainder)
{
   remainder += static_cast<std::uint64_t>(kAudioSampleRate) * kMicroHzScale;
   const unsigned frames = static_cast<unsigned>(remainder / frameRateMicroHz);
   remainder %= frameRateMicroHz;
   return frames;
}

} // namespace LibretroTiming

#endif
