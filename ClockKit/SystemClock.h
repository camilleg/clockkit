#pragma once
#include "Clock.h"

namespace dex {
class SystemClock : public Clock {
   public:
    static SystemClock& instance()
    {
        return instance_;
    }
    tp getValue();
    SystemClock(SystemClock&) = delete;
    SystemClock& operator=(SystemClock&) = delete;

   private:
    SystemClock()
    {
    }  // Singleton.  Use SystemClock::instance() instead.
    static SystemClock instance_;
};
}  // namespace dex
