#include "SystemClock.h"
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <limits>

namespace dex {

SystemClock SystemClock::instance_;

// Needs no mutex, because no state is stored,
// and because GetSystemTimeAsFileTime and gettimeofday are thread-safe.
timestamp_t SystemClock::getValue()
{
#ifdef WIN32
    FILETIME filetime;
    GetSystemTimeAsFileTime(&filetime);
    // Convert to a 64bit int, from 100 ns to usec,
    // then from windows epoch to unix epoch.
    return ((filetime.dwHighDateTime << 32) | filetime.dwLowDateTime) / 10 - 11644473600000000;
#else
    static constexpr auto invalid = std::numeric_limits<timestamp_t>::max();
    timeval now;
    return gettimeofday(&now, nullptr) < 0 ? invalid : now.tv_sec * 1000000 + now.tv_usec;
#endif
}

}  // namespace dex
