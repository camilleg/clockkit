#pragma once
#include <string>
#include "PhaseLockedClock.h"

namespace dex {
extern const std::string DEFAULT_CONFIG_FILE_PATH;
extern PhaseLockedClock* PhaseLockedClockFromConfigFile(std::string filename);
}  // namespace dex
