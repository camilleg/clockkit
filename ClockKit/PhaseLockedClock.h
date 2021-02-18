#pragma once
#include <cc++/thread.h>

#include "Clock.h"
#include "VariableFrequencyClock.h"

using namespace ost;
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
class PhaseLockedClock : public Clock, private Thread, private Mutex {
   public:
    /**
     * Creates a PhaseLockedClock around the provided primary and
     * reference clocks.  The update panic is initially set at
     * 5 seconds and the phase offset panic is initially set at 5 msec.
     *
     * Note: This is na Active Object.  So, when it is creted a thread
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

    /**
     * Returns the value of this PhaseLockedClock.  If for some reason
     * the clock has become out of sync with the refernce clock then
     * this call will throw a ClockException.
     */
    timestamp_t getValue();

    /**
     * @return true iff the clock is in sync with the reference clock.
     * This clock may become out of sync with the reference clock
     * in a number of ways.  If the last update was too long gao,
     * or the phase is detected to be too great, then the clock
     * goes into out-of-sync mode and will throw a ClockException
     * on any attempt to getValue() or getPhase()
     */
    inline bool isSynchronized() const
    {
        return inSync_;
    }

    /**
     * @return the phase offset from the reference clock.
     * - Throws a ClockException if out-of-sync.
     */
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

    // If the last successful update was longer ago than this,
    // the clock will be set to out-of-sync.
    inline void setUpdatePanic(timestamp_t updatePanic)
    {
        updatePanic_ = updatePanic;
    }

   protected:
    // Called when the thread for this Active Object is started.
    void run();

    // Called by this object's thread, each update interval.
    void update();

    // Tries to update the markers for the clocks.
    bool updatePhase();

    // Tries to update the clock to slew it into step.
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

    // Is the clock in sync?
    bool inSync_;

    // Phase between the VFC and the reference clock.
    timestamp_t thisPhase_;
    timestamp_t lastPhase_;

    // VFC's value.
    timestamp_t thisVariableValue_;
    timestamp_t lastVariableValue_;

    // Primary clock's value.
    timestamp_t thisPrimaryValue_;
    timestamp_t lastPrimaryValue_;

    // Average frequency of the primary clock w.r.t the reference clock.
    double primaryFrequencyAvg_;

    // See setPhasePanic() and setUpdatePanic().
    timestamp_t phasePanic_;
    timestamp_t updatePanic_;

    // Time of the last successful update.
    timestamp_t lastUpdate_;
};

}  // namespace dex
