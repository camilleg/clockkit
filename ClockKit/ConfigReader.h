#pragma once
   #include "PhaseLockedClock.h"
#include <string>

namespace dex {
  extern const string DEFAULT_CONFIG_FILE_PATH;
  extern PhaseLockedClock* PhaseLockedClockFromConfigFile(string filename);
}
