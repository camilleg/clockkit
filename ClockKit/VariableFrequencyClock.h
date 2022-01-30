#pragma once
#include "Clock.h"

namespace dex {

// All other clocks in ClockKit run at 1000000 Hz,
// but this clock's frequency can be changed.
// Its value starts at 0.
// It wraps a source clock.
class VariableFrequencyClock : public Clock {
   public:
    explicit VariableFrequencyClock(Clock&);

    // Returns the clock's value, in usec (*not* since the epoch).
    // Returns a big negative number (obviously invalid, because
    // the clock starts at zero) if the source clock moves backwards,
    // such as a counter that wrapped around.
    // If that happens, call setValue().
    tp getValue();

    // Sets the current time (the clock's value).
    void setValue(tp);

    double getFrequency() const
    {
        return frequency_;
    }
    void setFrequency(double);

   private:
    Clock& clockSrc_;
    double frequencySrc_;
    double frequency_;
    tp markerSrc_;
    tp marker_;
    bool rolledOver_;
    void updateMarkers();
};

}  // namespace dex
