#pragma once
#include "SystemClock.h"

namespace dex {

// The highest resolution clock available.
class HighResolutionClock {
   public:
    static Clock& instance()
    {
        return SystemClock::instance();
    }

   private:
    HighResolutionClock();
};

}  // namespace dex
