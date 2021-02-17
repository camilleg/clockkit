#pragma once
#include <string>
#include "PhaseLockedClock.h"

namespace dex {
extern PhaseLockedClock* PhaseLockedClockFromConfigFile(const std::string& filename);
}  // namespace dex
