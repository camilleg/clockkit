#pragma once
#include <cc++/thread.h>
#include "Common.h"
#include "Clock.h"

using namespace ost;
namespace dex {

// A system clock as microseconds since UNIX epoch.
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
     * Constructor is private.  Use SystemClock::instance() instead.
     * Copy constructor and assignment operator are private and unimplemented.
     */
    SystemClock();
    SystemClock(SystemClock& c);
    SystemClock& operator=(SystemClock& rhs);
    static SystemClock instance_;
}; // class SystemClock

}
