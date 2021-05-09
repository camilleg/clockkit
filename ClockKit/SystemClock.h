#pragma once
#include "Clock.h"

namespace dex {

// A singleton system clock.
class SystemClock : public Clock {
   public:
    static SystemClock& instance() { return instance_; }
    timestamp_t getValue();

   private:
    // Constructor is private.  Use SystemClock::instance() instead.
    // Copy constructor and assignment operator are private and unimplemented.
    SystemClock() {}
    SystemClock(SystemClock& c);
    SystemClock& operator=(SystemClock& rhs);
    static SystemClock instance_;
};

}  // namespace dex
