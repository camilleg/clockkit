#pragma once
#include "Clock.h"

namespace dex {

/**
 * All other clocks in ClockKit run at 1000000 Hz,
 * but this clock's frequency can be changed.
 * It wraps a master clock.
 * If that clock moves nonmonotonically, a ClockException is thrown.
 */
class VariableFrequencyClock : public Clock {
   public:
    /**
     * Creates a new variable frequency clock based on the
     * provided master clock.
     * The clock's value is initally set at 0.
     * The clock's frequency is initially set at 1000000 (usec ticks)
     */
    VariableFrequencyClock(Clock& master);

    /**
     * Returns the clock value.
     * Throws a ClockError if time is detected to have moved backwards.
     * This is usefull for slaving a variable frequency clock to a counter
     * that may loop.  In the case of an error, the client should reset the
     * clock's value using setClockValue()
     */
    timestamp_t getValue();

    /**
     * Sets the current time
     */
    void setValue(timestamp_t t);

    /**
     * Gets the frequency of this clock
     */
    int getFrequency();

    /**
     * Sets the frequency of this clock
     */
    void setFrequency(int freq);

   private:
    Clock& masterClock_;
    timestamp_t masterFrequency_;
    timestamp_t slaveFrequency_;
    timestamp_t masterMarker_;
    timestamp_t slaveMarker_;
    void updateMarkers();
};

}  // namespace dex
