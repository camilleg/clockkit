#pragma once
#include "Timestamp.h"

namespace dex {

// Abstract base class.
// Clocks have frequency 1000000 Hz, except for VariableFrequencyClock.
class Clock {
   public:
    // This clock's value, when this returns (not when it was called).
    // The lag between those two may be large for ClockClient.
    virtual tp getValue() = 0;

    // The phase, or offset, between this (primary) clock and another clock.
    virtual dur getPhase(Clock& c)
    {
        return diff(getValue(), c.getValue());
    }

    virtual ~Clock() = default;

    // Cleanly kill a ClockServer or ClockClient.
    virtual void die(){};
};

}  // namespace dex
