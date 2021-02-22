#pragma once
#include "Clock.h"

namespace dex {

// All other clocks in ClockKit run at 1000000 Hz,
// but this clock's frequency can be changed.
// Its value starts at 0.
// It wraps a source clock,
// If that clock moves nonmonotonically, a ClockException is thrown.
class VariableFrequencyClock : public Clock {
   public:
    explicit VariableFrequencyClock(Clock&);

    // Returns the clock value.
    // Throws a ClockError if time is detected to have moved backwards,
    // e.g. if the source clock is a looping counter.
    // If that happens, reset this clock's value with setValue().
    timestamp_t getValue();

    // Sets the current time.
    void setValue(timestamp_t);

    inline int getFrequency() const
    {
        return frequency_;
    }
    void setFrequency(int freq);

   private:
    Clock& clockSrc_;
    timestamp_t frequencySrc_;
    timestamp_t frequency_;
    timestamp_t markerSrc_;
    timestamp_t marker_;
    void updateMarkers();

    // Helper function.
    std::pair<timestamp_t, timestamp_t> getTicks() const;
};

}  // namespace dex
