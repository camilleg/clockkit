#pragma once
#include "Clock.h"

namespace dex {

// Other clocks in ClockKit run at 1000000 Hz,
// but this clock's frequency can be changed,
// to run faster or slower than the source clock that it wraps.
// Its value starts at 0 (not at usec since the epoch).

// Constructing multiple VFCs around the same SystemClock is safe because
// the latter has no state for "const in spirit" getValue() to change.
// But not around a ClockClient, because its sequence_ would get
// updated too fast, by each VFC's getValue().

class VariableFrequencyClock : public Clock {
   public:
    explicit VariableFrequencyClock(Clock&, double frequency = 1000000.0);

    // Wrapping a VFC around another VFC would be confusing, not useful.
    // It would also smell like a shallow-copy copy ctor.
    VariableFrequencyClock(VariableFrequencyClock&) = delete;
    VariableFrequencyClock(const VariableFrequencyClock&) = delete;
    VariableFrequencyClock& operator=(const VariableFrequencyClock&) = delete;

    // Returns tpInvalid if the source clock moves backwards, such as
    // a counter that wrapped around.  If that happens, call setValue().
    tp getValue();

    void setValue(tp);
    void setFrequency(double);

   private:
    Clock& clockSrc_;
    double frequency_;
    tp markerSrc_;
    tp marker_;
    bool rolledOver_;
    void updateMarkers();
};

}  // namespace dex
