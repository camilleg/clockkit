#pragma once
#include <atomic>

#include "VariableFrequencyClock.h"

namespace dex {

// This class reads two clocks, primaryClock_ and referenceClock_,
// both assumed to run at 1000000 Hz.
// primaryClock_ is usually a HighResolutionClock.
// referenceClock_ is usually a ClockClient.
// It makes a VariableFrequencyClock, whose phase and frequency
// it keeps locked to those of referenceClock_.
class PhaseLockedClock : public Clock {
   public:
    // Destroy the primary and reference clocks only after destroying this, lest this segfault.
    explicit PhaseLockedClock(Clock& primary, Clock& reference);

    ~PhaseLockedClock() = default;

    // Kill the referenceClock_, which is likely a ClockClient, and its ClockServer.
    void die()
    {
        referenceClock_.die();
    }

    timestamp_t getValue();

    // Return whether we're in sync with referenceClock_.
    // Sync becomes lost if the vfc's previous update was too long ago,
    // or if the vfc's offset relative to referenceClock_ becomes too large.
    inline bool isSynchronized() const
    {
        return inSync_;
    }

    // Phase offset of vfc relative to referenceClock_, i.e., phase_.
    int getOffset();

    inline void setPhasePanic(timestamp_t phasePanic)
    {
        phasePanic_ = phasePanic;
    }

    inline void setUpdatePanic(timestamp_t usec)
    {
        updatePanic_ = usec;
    }

    // Call update() periodically, until the caller sets the arg to true.
    void run(std::atomic_bool&);

   protected:
    // Called periodically by run().
    // Calls updatePhase() and updateClock() to adjust the vfc.
    void update();

    bool updatePhase();
    bool updateClock();

    // Hard-reset to referenceClock_, to regain sync.
    void setClock();

   private:
    explicit PhaseLockedClock(PhaseLockedClock&);
    PhaseLockedClock& operator=(PhaseLockedClock&);

    Clock& primaryClock_;
    Clock& referenceClock_;
    VariableFrequencyClock variableFrequencyClock_;

    timestamp_t primaryValue() const
    {
        return primaryClock_.getValue();
    }

    bool inSync_;

    // Phase between vfc and referenceClock_.
    timestamp_t phase_;
    timestamp_t phasePrev_;

    // Value of vfc.
    timestamp_t variableValue_;
    timestamp_t variableValuePrev_;

    // Value of primaryClock_.
    timestamp_t primaryValue_;
    timestamp_t primaryValuePrev_;

    // Average frequency of primaryClock_, in Hz.
    double primaryFrequencyAvg_;

    // Thresholds set by setPhasePanic() and setUpdatePanic().
    timestamp_t phasePanic_;
    timestamp_t updatePanic_;

    // When updatePhase() last succeeded.
    timestamp_t updatePrev_;
};

}  // namespace dex
