#pragma once
#include <cc++/thread.h>  // for ost::Thread

#include "Clock.h"
#include "VariableFrequencyClock.h"

namespace dex {

/**
 * This class synchronizes one oscillator based on another.
 * It has a primary clock and a reference clock.
 * It creates a VariableFrequencyClock around the primary clock.
 * It spawns a thread that periodically measures these clocks' phase,
 * to keep them in sync.
 *
 * - Assumes the primary and reference clocks run at 1000000 Hz.
 * - The update interval is initially set at 1 second.
 * - Updates occur randomly at +-10% of the update interval,
 *   to not swamp the server with synchronous requests.
 */
class PhaseLockedClock : public Clock, private ost::Thread {
   public:
    /**
     * Creates a PhaseLockedClock around the provided primary and
     * reference clocks.
     *
     * This is an Active Object.  So, when it is created a thread
     * is started to manage it.  When the object is deleted, the thread
     * will be suspended.
     */
    explicit PhaseLockedClock(Clock& primary, Clock& reference);

    /**
     * Cleans up all resources associated with this Clock and
     * stops the thread associated with this Active Object
     */
    ~PhaseLockedClock()
    {
        terminate();
    }

    // Kill the ClockClient and its ClockServer.
    void die()
    {
        referenceClock_.die();
    }

    // Value of this clock.
    timestamp_t getValue();

    // Returns true iff we're in sync with the reference clock.
    // Sync becomes lost if the previous update was too long ago,
    // or if the phase is detected to be too great.
    // When out of sync, ClockException is thrown by
    // getValue(), getPhase(), and getOffset().
    inline bool isSynchronized() const
    {
        return inSync_;
    }

    // Phase offset from the reference clock.
    int getOffset();

    // If the phase of the PhaseLockedClock differs by more than this
    // from the reference clock's, the clock will be set to out-of-sync.
    // Call this or setUpdatePanic() while the clock is running, to compensate
    // for a crystal drifting due to temperature change (handheld or mobile),
    // or for bandwidth change (failing hotspot, WLAN degradation).
    inline void setPhasePanic(timestamp_t phasePanic)
    {
        phasePanic_ = phasePanic;
    }

    // If the previous successful update was longer ago than this,
    // the clock will be set to out-of-sync.
    inline void setUpdatePanic(timestamp_t usec)
    {
        updatePanic_ = usec;
    }

   protected:
    // Called when the thread for this Active Object is started.
    void run();

    // Called by this object's thread, each update interval.
    void update();

    // Update the clocks' markers.
    bool updatePhase();

    // Slew the clock into step.
    bool updateClock();

    /**
     * Sets the clock to the reference clock,
     * to get the clock back into sync.
     * This does not slew the clock.  It does a hard reset.
     */
    void setClock();

   private:
    explicit PhaseLockedClock(PhaseLockedClock& c);
    PhaseLockedClock& operator=(PhaseLockedClock& rhs);

    Clock& primaryClock_;
    Clock& referenceClock_;
    VariableFrequencyClock variableFrequencyClock_;

    // Are we in sync?
    bool inSync_;

    // Phase between the VFC and the reference clock.
    timestamp_t phase_;
    timestamp_t phasePrev_;

    // VFC's value.
    timestamp_t variableValue_;
    timestamp_t variableValuePrev_;

    // Primary clock's value.
    timestamp_t primaryValue_;
    timestamp_t primaryValuePrev_;

    // Average frequency of the primary clock w.r.t the reference clock.
    double primaryFrequencyAvg_;

    // See setPhasePanic() and setUpdatePanic().
    timestamp_t phasePanic_;
    timestamp_t updatePanic_;

    timestamp_t updatePrev_;
};

}  // namespace dex
