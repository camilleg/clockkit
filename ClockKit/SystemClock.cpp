#include "SystemClock.h"
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

namespace dex {

SystemClock SystemClock::instance_;

// Needs no mutex, because no state is stored,
// and because GetSystemTimeAsFileTime and gettimeofday are thread-safe.
timestamp_t SystemClock::getValue()
{
    timestamp_t time;
#ifdef WIN32
    FILETIME filetime;
    GetSystemTimeAsFileTime(&filetime);
    // convert to a 64bit int.
    time = (filetime.dwHighDateTime << 32) | filetime.dwLowDateTime;
    // Convert from 100 ns to usec.
    time /= 10;
    // Convert from windows epoch to unix epoch.
    time -= 11644473600000000;
#else
    timeval now;
    gettimeofday(&now, 0);
    time = (now.tv_sec * 1000000) + now.tv_usec;
#endif
    return time;
}

}  // namespace dex
