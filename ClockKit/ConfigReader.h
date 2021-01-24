#pragma once
#include <string>
#include "PhaseLockedClock.h"

namespace dex {
extern const string DEFAULT_CONFIG_FILE_PATH;
extern PhaseLockedClock* PhaseLockedClockFromConfigFile(string filename);
}  // namespace dex
