//----------------------------------------------------------------------------//
#ifndef DEX_HIGH_RESOLUTION_CLOCK_H
#define DEX_HIGH_RESOLUTION_CLOCK_H
//----------------------------------------------------------------------------//
#include "Clock.h"
#include "SystemClock.h"

#ifdef WIN32
#include "PerformanceCounter.h"
#endif
//----------------------------------------------------------------------------//
namespace dex {


/** 
 * Provides the higest resolution clock available on a given system.
 * On unix systems, this willjust be the SystemClock.
 * However, on Windows builds this will return an instance of the 
 * PerformanceCounter.
 */
class HighResolutionClock
{
public:
    
    /**
     * @return an instance of a Clock which has high resolution on this system.
     */
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

//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_HIGH_RESOLUTION_CLOCK_H
//----------------------------------------------------------------------------//

