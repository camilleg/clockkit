#pragma once
#include <cc++/thread.h>
#include "Clock.h"
#include "Common.h"

using namespace ost;
namespace dex {

// A singleton system clock.
class SystemClock : public Clock, private Mutex {
   public:
    static SystemClock& instance();
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
};

}  // namespace dex
