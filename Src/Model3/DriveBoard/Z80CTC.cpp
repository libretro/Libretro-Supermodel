#include "Z80CTC.h"

// Partial emulation of the Z80 CTC sufficient for the Model 3 drive board.
// Reference: http://www.z80.info/zip/z80ctc.pdf

void Z80CTC::Write(UINT32 channel, UINT8 value)
{
  auto &state = m_ch[channel];

  if (value & 0x01) // D0 = 1: control word
  {
    state.control = value;
    state.interruptEnabled = (value & 0x80) != 0;
    state.counterMode = (value & 0x40) != 0;
    state.prescaler256 = (value & 0x20) != 0;
    state.triggerRising = (value & 0x10) != 0;
    state.manualTrigger = (value & 0x08) != 0;
    state.waitingForTimeConstant = (value & 0x04) != 0;

    if (value & 0x02) // Software reset
    {
      state.counter = 0;
      state.running = false;
    }
  }
  else if (state.waitingForTimeConstant)
  {
    const bool automaticTrigger = !state.manualTrigger;

    // The register is 8-bit, where zero encodes an effective value of 256.
    // Preserve the raw byte in the state and expand it when it is consumed.
    state.timeConstant = value;
    state.counter = value ? value : 256;
    state.waitingForTimeConstant = false;
    state.running = automaticTrigger;
  }
}

UINT32 Z80CTC::CalcFrequency(UINT32 channel, UINT32 inputFrequency)
{
  auto &state = m_ch[channel];

  if (!state.counterMode)
  {
    const UINT32 prescaler = state.prescaler256 ? 256 : 16;
    const UINT32 timeConstant = state.timeConstant ? state.timeConstant : 256;
    return inputFrequency / (timeConstant * prescaler);
  }

  // In the configuration used by the drive board, a counter channel receives
  // its input from the preceding CTC channel.
  if (state.triggerRising)
    return CalcFrequency(channel - 1, inputFrequency) / state.counter;

  return 0;
}

bool Z80CTC::TimerRunning(UINT32 channel) const
{
  return m_ch[channel].running;
}

void Z80CTC::Reset()
{
  for (auto &state : m_ch)
    state = Channel{};
}

void Z80CTC::SaveState(CBlockFile *saveState) const
{
  saveState->Write(m_ch, sizeof(m_ch));
}

void Z80CTC::LoadState(CBlockFile *saveState)
{
  saveState->Read(m_ch, sizeof(m_ch));
}
