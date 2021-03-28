#pragma once
#include "Clock.h"
#include "Common.h"

namespace dex {

// A singleton system clock.
class SystemClock : public Clock {
   public:
    static SystemClock& instance();
    timestamp_t getValue();

   private:
    /**
     * Constructor is private.  Use SystemClock::instance() instead.
     * Copy constructor and assignment operator are private and unimplemented.
     */
    // XXX do we even need the constructor or can it be `delete`d?
    SystemClock();
    SystemClock(SystemClock& c);
    SystemClock& operator=(SystemClock& rhs);
    static SystemClock instance_;
};

}  // namespace dex
