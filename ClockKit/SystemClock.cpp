#include "SystemClock.h"

#ifdef WIN32
#include <windows.h>
#endif
#include <chrono>

namespace dex {
SystemClock SystemClock::instance_;

// Needs no mutex, because no state is stored,
// and because GetSystemTimeAsFileTime and std::chrono are thread-safe.
timestamp_t SystemClock::getValue()
{
#ifdef WIN32
    FILETIME t;
    GetSystemTimeAsFileTime(&t);
    // Convert to a 64bit int, from 100 ns to usec,
    // then from windows epoch to unix epoch.
    return ((t.dwHighDateTime << 32) | t.dwLowDateTime) / 10 - 11644473600000000;
#else
    using namespace std::chrono;
    return system_clock::now().time_since_epoch() / 1us;
#endif
}
}  // namespace dex
