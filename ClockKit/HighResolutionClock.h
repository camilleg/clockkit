#pragma once
#include "Clock.h"
#include "SystemClock.h"
#ifdef WIN32
#include "PerformanceCounter.h"
#endif

namespace dex {

/** 
 * Provides the highest resolution clock available:
 * on Windows, the PerformanceCounter; otherwise, the SystemClock.
 */
class HighResolutionClock
{
public:
    static Clock& instance()
    {
        #ifdef WIN32
            return PerformanceCounter::instance();
        #else
            return SystemClock::instance();
        #endif
    }

private:
    HighResolutionClock();
}; 

}
