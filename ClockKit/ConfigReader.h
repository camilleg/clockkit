#pragma once
#include <string>

#include "PhaseLockedClock.h"

namespace dex {
class ConfigReader {
   public:
    ConfigReader() = delete;
    ~ConfigReader() = delete;
};
// TODO incorporate that more nicely
extern PhaseLockedClock* PhaseLockedClockFromConfigFile(const std::string& filename);
}  // namespace dex
