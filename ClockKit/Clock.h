#pragma once
#include "Common.h"

namespace dex {

// Abstract base class.
// Clocks have frequency 1000000 Hz, except for VariableFrequencyClock.
class Clock {
   public:
    /**
     * @return The present value of the clock.
     *
     * NOTE: For ClockClient this call may take some time.
     * The return value measures the time when the function returns,
     * not when it was called.
     *
     * Most clocks tick all the time, However, some clocks such as the Win2k/XP
     * system clock tick at a slow rate.
     */
    virtual timestamp_t getValue() = 0;

    /**
     * @return The phase (or offset) between this (primary) clock and
     *         the provided (secondary) clock.
     *
     * secondaryClock + phase = primaryClock
     *
     * For clocks like RemoteClock, this measurement will be much more
     * precise than comparing getValue() on two seperate clocks.
     */
    virtual timestamp_t getPhase(Clock& c)
    {
        return getValue() - c.getValue();
    }

    virtual ~Clock() = default;
};

}  // namespace dex
