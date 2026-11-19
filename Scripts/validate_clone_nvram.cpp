#include <algorithm>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "OSD/libretro/LibretroNvramSettings.h"

namespace fs = std::filesystem;
using namespace LibretroNvramSettings;

namespace {

struct Block {
  size_t offset = 0;
  size_t size = 0;
};

uint32_t ReadU32(const std::vector<uint8_t> &data, size_t offset)
{
  if (offset + 4 > data.size())
    throw std::runtime_error("truncated block header");
  return static_cast<uint32_t>(data[offset]) |
         (static_cast<uint32_t>(data[offset + 1]) << 8) |
         (static_cast<uint32_t>(data[offset + 2]) << 16) |
         (static_cast<uint32_t>(data[offset + 3]) << 24);
}

std::vector<uint8_t> ReadFile(const fs::path &path)
{
  std::ifstream stream(path, std::ios::binary);
  if (!stream)
    throw std::runtime_error("cannot open " + path.string());
  stream.seekg(0, std::ios::end);
  const auto length = stream.tellg();
  if (length < 0)
    throw std::runtime_error("cannot size " + path.string());
  std::vector<uint8_t> data(static_cast<size_t>(length));
  stream.seekg(0, std::ios::beg);
  stream.read(reinterpret_cast<char *>(data.data()), length);
  if (!stream)
    throw std::runtime_error("cannot read " + path.string());
  return data;
}

Block FindBlock(const std::vector<uint8_t> &data, const std::string &wanted)
{
  size_t cursor = 0;
  while (cursor + 12 <= data.size())
  {
    const uint32_t length = ReadU32(data, cursor);
    const uint32_t nameLength = ReadU32(data, cursor + 4);
    const uint32_t commentLength = ReadU32(data, cursor + 8);
    const uint64_t headerLength = 12ull + nameLength + commentLength;
    if (length < headerLength || cursor + length > data.size())
      break;
    const size_t nameOffset = cursor + 12;
    std::string name(reinterpret_cast<const char *>(data.data() + nameOffset),
                     nameLength);
    while (!name.empty() && name.back() == '\0')
      name.pop_back();
    if (name == wanted)
    {
      const size_t payload = nameOffset + nameLength + commentLength;
      return {payload, cursor + length - payload};
    }
    cursor += length;
  }
  throw std::runtime_error("block " + wanted + " not found");
}

size_t DifferenceCount(const uint8_t *left, size_t leftSize,
                       const uint8_t *right, size_t rightSize)
{
  const size_t common = std::min(leftSize, rightSize);
  size_t different = std::max(leftSize, rightSize) - common;
  for (size_t i = 0; i < common; ++i)
    different += left[i] != right[i];
  return different;
}

bool IntegrityValid(const Game &game, const uint16_t *words)
{
  using namespace LibretroNvramSettings::Detail;
  const Family family = GetFamily(&game);
  if (family == Family::FightingVipers2 ||
      family == Family::VirtuaFighter3)
    return true; // These families store the managed fields in Backup RAM.
  if (family == Family::SegaRally2 || family == Family::StarWars)
    return words[5] == Crc16(words, 8, 63, 0xffff, 0xffff);
  if (family == Family::DirtDevils || family == Family::Lamachin ||
      family == Family::LeMans24 || family == Family::LostWorld ||
      family == Family::MagTruck || family == Family::VirtualOn2)
    return words[3] == SegaA3Checksum(words);
  return words[3] == Crc16(words, 6, 34, 0, 0);
}

Game MakeGame(const SupportedGame &entry)
{
  Game game;
  game.name = entry.name;
  if (entry.parent)
    game.parent = entry.parent;
  return game;
}

const char *SampleGroup(Setting setting)
{
  switch (setting)
  {
  case Setting::Country:       return "country";
  case Setting::Difficulty:    return "difficulty";
  case Setting::LinkMode:      return "link";
  case Setting::LinkId:        return "link-id";
  case Setting::Cabinet:       return "cabinet";
  case Setting::SpecialCar:    return "special-car";
  case Setting::DisplayType:   return "display-type";
  case Setting::ChristmasMode: return "christmas-mode";
  case Setting::Vocal:         return "vocal";
  case Setting::LeverFeedback: return "deluxe-lever-feedback";
  case Setting::DefaultView:   return "default-view";
  default:                     return nullptr;
  }
}

std::set<size_t> IntegrityBytes(const Game &game)
{
  using namespace LibretroNvramSettings::Detail;
  const Family family = GetFamily(&game);
  if (family == Family::FightingVipers2 ||
      family == Family::VirtuaFighter3)
    return {};
  const size_t word = family == Family::SegaRally2 ||
                      family == Family::StarWars ? 5 : 3;
  return {word * 2, word * 2 + 1};
}

unsigned ValidateAcquiredSamples(
  const Game &game, const std::vector<uint8_t> &nativeBlob,
  const Block &nativeEeprom, const Block &nativeBackup,
  const fs::path &campaignRoot, bool requireComplete)
{
  const fs::path saves = campaignRoot / game.name / "saves";
  if (!fs::is_directory(saves))
    return 0;

  uint16_t nativeWords[64];
  std::memcpy(nativeWords, nativeBlob.data() + nativeEeprom.offset,
              sizeof(nativeWords));
  std::vector<uint8_t> nativeBackupData(
    nativeBlob.begin() + nativeBackup.offset,
    nativeBlob.begin() + nativeBackup.offset + 0x20000);
  const std::set<size_t> integrityBytes = IntegrityBytes(game);
  unsigned acquired = 0;

  for (unsigned raw = 0; raw < static_cast<unsigned>(Setting::Count); ++raw)
  {
    const Setting setting = static_cast<Setting>(raw);
    if (setting == Setting::ProvideCalibrationData)
      continue;
    const SettingInfo *info = GetSettingInfo(&game, setting);
    const char *group = SampleGroup(setting);
    if (!info || !group)
      continue;

    struct Expected {
      std::string key;
      uint16_t words[64];
      std::vector<uint8_t> backup;
    };
    std::vector<Expected> expected;
    std::set<size_t> eepromSignature;
    std::set<size_t> backupSignature;
    for (size_t valueIndex = 0; valueIndex < info->valueCount; ++valueIndex)
    {
      Expected candidate;
      candidate.key = info->values[valueIndex].key;
      std::memcpy(candidate.words, nativeWords, sizeof(candidate.words));
      candidate.backup = nativeBackupData;
      Selection selection;
      selection.values[raw] = info->values[valueIndex].key;
      const ApplyResult result = Apply(game, candidate.words,
                                       candidate.backup.data(),
                                       candidate.backup.size(), selection);
      if (result == ApplyResult::Unsupported ||
          result == ApplyResult::InvalidLayout)
        throw std::runtime_error("cannot build expected sample signature");
      const uint8_t *candidateEeprom =
        reinterpret_cast<const uint8_t *>(candidate.words);
      const uint8_t *baseEeprom =
        reinterpret_cast<const uint8_t *>(nativeWords);
      for (size_t byte = 0; byte < sizeof(candidate.words); ++byte)
        if (candidateEeprom[byte] != baseEeprom[byte] &&
            !integrityBytes.count(byte))
          eepromSignature.insert(byte);
      for (size_t byte = 0; byte < candidate.backup.size(); ++byte)
        if (candidate.backup[byte] != nativeBackupData[byte])
          backupSignature.insert(byte);
      expected.push_back(std::move(candidate));
    }

    std::vector<fs::path> samples;
    const std::string prefix = game.name + "-" + group + "-step-";
    for (const fs::directory_entry &entry : fs::directory_iterator(saves))
    {
      const std::string name = entry.path().filename().string();
      if (entry.is_regular_file() && name.compare(0, prefix.size(), prefix) == 0 &&
          entry.path().extension() == ".srm")
        samples.push_back(entry.path());
    }
    std::sort(samples.begin(), samples.end());
    if (requireComplete && samples.size() != info->valueCount)
      throw std::runtime_error(
        std::string(group) + " acquired " + std::to_string(samples.size()) +
        "/" + std::to_string(info->valueCount) + " samples");

    std::map<std::string, unsigned> matchedKeys;
    std::vector<std::string> observed;
    for (const fs::path &samplePath : samples)
    {
      const std::vector<uint8_t> sampleBlob = ReadFile(samplePath);
      const Block sampleEeprom = FindBlock(sampleBlob, "93C46");
      const Block sampleBackup = FindBlock(sampleBlob, "Backup RAM");
      if (sampleEeprom.size < 128 || sampleBackup.size < 0x20000)
        throw std::runtime_error("short acquired sample block");
      uint16_t actualWords[64];
      std::memcpy(actualWords, sampleBlob.data() + sampleEeprom.offset,
                  sizeof(actualWords));
      if (!IntegrityValid(game, actualWords))
        throw std::runtime_error(samplePath.filename().string() +
                                 " has invalid integrity data");

      std::vector<std::string> matches;
      for (const Expected &candidate : expected)
      {
        const uint8_t *candidateEeprom =
          reinterpret_cast<const uint8_t *>(candidate.words);
        bool same = true;
        for (size_t byte : eepromSignature)
          same &= sampleBlob[sampleEeprom.offset + byte] ==
                  candidateEeprom[byte];
        for (size_t byte : backupSignature)
          same &= sampleBlob[sampleBackup.offset + byte] ==
                  candidate.backup[byte];
        if (same)
          matches.push_back(candidate.key);
      }
      if (matches.size() != 1)
        throw std::runtime_error(
          samplePath.filename().string() + " matched " +
          std::to_string(matches.size()) + " values");
      ++matchedKeys[matches.front()];
      observed.push_back(samplePath.stem().string() + "=" + matches.front());
      ++acquired;
    }
    if (requireComplete && matchedKeys.size() != info->valueCount)
    {
      std::ostringstream detail;
      detail << group << " does not cover every parent value; observed=";
      for (size_t index = 0; index < observed.size(); ++index)
      {
        if (index)
          detail << ',';
        detail << observed[index];
      }
      detail << "; missing=";
      bool first = true;
      for (const Expected &candidate : expected)
      {
        if (matchedKeys.count(candidate.key))
          continue;
        if (!first)
          detail << ',';
        detail << candidate.key;
        first = false;
      }
      throw std::runtime_error(detail.str());
    }
  }
  return acquired;
}

} // namespace

int main(int argc, char **argv)
{
  if (argc < 3 || argc > 5)
  {
    std::cerr << "usage: validate_clone_nvram CATALOG_ROOT ANALYSIS_ROOT "
                 "[CAMPAIGN_ROOT [--complete]]\n";
    return 2;
  }

  const fs::path catalogRoot = fs::path(argv[1]);
  const fs::path analysisRoot = fs::path(argv[2]);
  const bool validateSamples = argc >= 4;
  const fs::path campaignRoot = validateSamples ? fs::path(argv[3]) : fs::path();
  const bool requireComplete = argc == 5 && std::string(argv[4]) == "--complete";
  size_t supportedCount = 0;
  const SupportedGame *supported = GetSupportedGames(supportedCount);
  unsigned cloneCount = 0;
  unsigned valueCount = 0;
  unsigned failureCount = 0;
  unsigned acquiredCount = 0;

  std::cout << "clone\tparent\teeprom_diff\tbackup_diff\tvalues\tresult\n";
  for (size_t index = 0; index < supportedCount; ++index)
  {
    const SupportedGame &entry = supported[index];
    if (!entry.parent)
      continue;
    const Game game = MakeGame(entry);

    unsigned cloneValues = 0;
    for (unsigned raw = 0; raw < static_cast<unsigned>(Setting::Count); ++raw)
    {
      const Setting setting = static_cast<Setting>(raw);
      if (setting == Setting::ProvideCalibrationData)
        continue; // Operational control, not a parent recipe sample.
      if (const SettingInfo *info = GetSettingInfo(&game, setting))
        cloneValues += static_cast<unsigned>(info->valueCount);
    }
    if (cloneValues == 0)
      continue;

    ++cloneCount;
    valueCount += cloneValues;
    std::string result = "ok";
    size_t eepromDiff = 0;
    size_t backupDiff = 0;
    try
    {
      const fs::path clonePath = catalogRoot / game.name / "saves" /
        (game.name + "-menu-structure.srm");
      const fs::path parentPath = analysisRoot / game.parent / "game-base.srm";
      const std::vector<uint8_t> cloneBlob = ReadFile(clonePath);
      const std::vector<uint8_t> parentBlob = ReadFile(parentPath);
      const Block cloneEeprom = FindBlock(cloneBlob, "93C46");
      const Block cloneBackup = FindBlock(cloneBlob, "Backup RAM");
      const Block parentEeprom = FindBlock(parentBlob, "93C46");
      const Block parentBackup = FindBlock(parentBlob, "Backup RAM");
      if (cloneEeprom.size < 128 || cloneBackup.size < 0x20000)
        throw std::runtime_error("short clone NVRAM block");

      eepromDiff = DifferenceCount(
        cloneBlob.data() + cloneEeprom.offset, cloneEeprom.size,
        parentBlob.data() + parentEeprom.offset, parentEeprom.size);
      backupDiff = DifferenceCount(
        cloneBlob.data() + cloneBackup.offset, cloneBackup.size,
        parentBlob.data() + parentBackup.offset, parentBackup.size);

      uint16_t nativeWords[64];
      std::memcpy(nativeWords, cloneBlob.data() + cloneEeprom.offset,
                  sizeof(nativeWords));
      std::vector<uint8_t> nativeBackup(
        cloneBlob.begin() + cloneBackup.offset,
        cloneBlob.begin() + cloneBackup.offset + 0x20000);

      if (validateSamples)
        acquiredCount += ValidateAcquiredSamples(
          game, cloneBlob, cloneEeprom, cloneBackup, campaignRoot,
          requireComplete);

      for (unsigned raw = 0; raw < static_cast<unsigned>(Setting::Count); ++raw)
      {
        const Setting setting = static_cast<Setting>(raw);
        if (setting == Setting::ProvideCalibrationData)
          continue;
        const SettingInfo *info = GetSettingInfo(&game, setting);
        if (!info)
          continue;
        for (size_t valueIndex = 0; valueIndex < info->valueCount; ++valueIndex)
        {
          uint16_t words[64];
          std::memcpy(words, nativeWords, sizeof(words));
          std::vector<uint8_t> backup = nativeBackup;
          Selection selection;
          selection.values[raw] = info->values[valueIndex].key;
          const ApplyResult first = Apply(game, words, backup.data(),
                                          backup.size(), selection);
          if (first == ApplyResult::Unsupported ||
              first == ApplyResult::InvalidLayout)
            throw std::runtime_error(
              std::string(info->label) + "=" + info->values[valueIndex].key +
              " rejected by clone layout");
          if (!IntegrityValid(game, words))
            throw std::runtime_error(
              std::string(info->label) + "=" + info->values[valueIndex].key +
              " produced invalid integrity data");

          // Simulate save-container persistence and a second application.
          std::vector<uint8_t> persisted = cloneBlob;
          std::memcpy(persisted.data() + cloneEeprom.offset, words,
                      sizeof(words));
          std::memcpy(persisted.data() + cloneBackup.offset, backup.data(),
                      backup.size());
          const Block persistedEeprom = FindBlock(persisted, "93C46");
          const Block persistedBackup = FindBlock(persisted, "Backup RAM");
          uint16_t reloadedWords[64];
          std::memcpy(reloadedWords, persisted.data() + persistedEeprom.offset,
                      sizeof(reloadedWords));
          std::vector<uint8_t> reloadedBackup(
            persisted.begin() + persistedBackup.offset,
            persisted.begin() + persistedBackup.offset + 0x20000);
          const ApplyResult second = Apply(
            game, reloadedWords, reloadedBackup.data(), reloadedBackup.size(),
            selection);
          if (second != ApplyResult::Unchanged)
            throw std::runtime_error(
              std::string(info->label) + "=" + info->values[valueIndex].key +
              " did not persist exactly");
        }
      }
    }
    catch (const std::exception &error)
    {
      result = error.what();
      ++failureCount;
    }

    std::cout << game.name << '\t' << game.parent << '\t' << eepromDiff
              << '\t' << backupDiff << '\t' << cloneValues << '\t'
              << result << '\n';
  }

  std::cerr << "validated " << valueCount << " values across " << cloneCount
            << " release clones; acquired_samples=" << acquiredCount
            << "; failures=" << failureCount << '\n';
  const bool completeSamplesOkay = !requireComplete || acquiredCount == valueCount;
  return failureCount == 0 && cloneCount == 35 &&
         completeSamplesOkay ? 0 : 1;
}
