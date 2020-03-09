
//----------------------------------------------------------------------------//
#ifndef DEX_SYSTEM_CLOCK_H
#define DEX_SYSTEM_CLOCK_H
//----------------------------------------------------------------------------//
#include <cc++/thread.h>
#include "Common.h"
#include "Clock.h"
//----------------------------------------------------------------------------//
using namespace ost;
namespace dex {
//----------------------------------------------------------------------------//

/**
 * Provides a system clock as microseconds (usec) since UNIX epoch.
 * 
 * Note, in windows, the resolution of this clock may be very bad.
 * Look into using PerformanceCounter to improve accuracy.
 * 
 * Use instance() to obtain an instance of this object.
 *
 * This class makes use of a mutex to ensure that many threads can
 * safely call the clock. In some OS implementations, calls to 
 * gettimeofday() may not be thread safe. 
 */
class SystemClock : public Clock, private Mutex
{
public:

	/**
	 * @return A singelton instance of the system clock.
	 */
	static SystemClock& instance();
	
	/**
	 * @return the system clock as a timestamp value
	 *         in microseconds since UNIX epoch.
	 */
    timestamp_t getValue();
	
private:
    
    /**
     * Constructor is private.  Use SystemClock::instance() to get
     * an instance of the SystemClock singelton.
     */
	SystemClock();
    
    /**
     * Copy Constructor is private and unimplemented.
     */
	SystemClock(SystemClock& c);
    
    /**
     * Assignment Operator is private and unimplemented.
     */
    SystemClock& operator=(SystemClock& rhs);

    /**
     * Static variable for the instane of the singelton.
     */
    static SystemClock instance_;
    
}; // class SystemClock

//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_SYSTEM_CLOCK_H
//----------------------------------------------------------------------------//

