
//----------------------------------------------------------------------------//
#ifndef DEX_SYSTEM_CLOCK_CPP
#define DEX_SYSTEM_CLOCK_CPP
//----------------------------------------------------------------------------//
#include "SystemClock.h"
#ifdef WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
#endif

//----------------------------------------------------------------------------//
namespace dex {
//----------------------------------------------------------------------------//
SystemClock SystemClock::instance_;

SystemClock::SystemClock() {}

//----------------------------------------------------------------------------//

SystemClock& SystemClock::instance()
{
	return instance_;	
}

//----------------------------------------------------------------------------//
timestamp_t SystemClock::getValue()
{

    timestamp_t time;
    enterMutex();

#ifdef WIN32

	FILETIME filetime;
	GetSystemTimeAsFileTime(&filetime);
	// convert to a 64bit int.
	time = 0;
	time = filetime.dwHighDateTime;
	time <<= 32;
	time |= filetime.dwLowDateTime;
	// convert from 100 n-sec units to usec:
	time /= 10;
	// convert from windows epoch to unix epoch.
	time -= 11644473600000000;

#else

	timeval now;
    gettimeofday(&now, 0);
    time = now.tv_sec;
    time *= 1000000;
    time += now.tv_usec;

#endif

    leaveMutex();
    return time;
}



//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_SYSTEM_CLOCK_CPP
//----------------------------------------------------------------------------//

