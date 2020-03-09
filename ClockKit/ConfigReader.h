//----------------------------------------------------------------------------//
#ifndef DEX_CONFIG_READER_H
#define DEX_CONFIG_READER_H
//----------------------------------------------------------------------------//
#include <string>
#include "PhaseLockedClock.h"
//----------------------------------------------------------------------------//
using namespace std;

namespace dex {
//----------------------------------------------------------------------------//

extern const string DEFAULT_CONFIG_FILE_PATH;

extern PhaseLockedClock* PhaseLockedClockFromConfigFile(string filename);


//----------------------------------------------------------------------------//
} // namespace dex

//----------------------------------------------------------------------------//
#endif //DEX_CONFIG_READER_H

