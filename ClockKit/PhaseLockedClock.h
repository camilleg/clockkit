#pragma once
#include <atomic>

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
 * - It varies randomly +-10%,
 *   to not swamp the server with simultaneous requests.
 */
class PhaseLockedClock : public Clock {
   public:
    // Construct a PhaseLockedClock around a primary and reference clock.
    explicit PhaseLockedClock(Clock& primary, Clock& reference);

    ~PhaseLockedClock() = default;

    // Kill the ClockClient and its ClockServer.
    void die()
    {
        referenceClock_.die();
    }

    timestamp_t getValue();

    // Return whether we're in sync with our reference clock.
    // Sync becomes lost if the previous update was too long ago,
    // or if the phase is detected to be too great.
    inline bool isSynchronized() const
    {
        return inSync_;
    }

    // Phase offset from our reference clock.
    int getOffset();

    // If our PhaseLockedClock's phase differs from our reference
    // clock's by more than this, we get set to out of sync.
    // Call this or setUpdatePanic(), to compensate for a crystal
    // drifting due to temperature change (handheld or mobile),
    // or for bandwidth change (failing hotspot, WLAN degradation).
    inline void setPhasePanic(timestamp_t phasePanic)
    {
        phasePanic_ = phasePanic;
    }

    // If the previous successful update was longer ago than this,
    // we will be set to out-of-sync.
    inline void setUpdatePanic(timestamp_t usec)
    {
        updatePanic_ = usec;
    }

    // Call update() periodically, until the caller sets the flag to true.
    void run(std::atomic_bool&);

   protected:
    // Called periodically by run().
    // Call updatePhase() and updateClock().
    void update();

    // Update our clocks' markers.
    bool updatePhase();

    // Slew into step.
    bool updateClock();

    // Hard-reset to our reference clock, to regain sync.
    void setClock();

   private:
    explicit PhaseLockedClock(PhaseLockedClock& c);
    PhaseLockedClock& operator=(PhaseLockedClock& rhs);

    Clock& primaryClock_;
    Clock& referenceClock_;
    VariableFrequencyClock variableFrequencyClock_;

    // Are we in sync?
    bool inSync_;

    // Phase between the VFC and our reference clock.
    timestamp_t phase_;
    timestamp_t phasePrev_;

    // VFC's value.
    timestamp_t variableValue_;
    timestamp_t variableValuePrev_;

    // Primary clock's value.
    timestamp_t primaryValue_;
    timestamp_t primaryValuePrev_;

    // Average frequency, in Hz, of our primary clock w.r.t our reference clock.
    double primaryFrequencyAvg_;

    // See setPhasePanic() and setUpdatePanic().
    timestamp_t phasePanic_;
    timestamp_t updatePanic_;

    timestamp_t updatePrev_;
};

}  // namespace dex
