
//----------------------------------------------------------------------------//
#ifndef DEX_PHASE_LOCKED_CLOCK_H
#define DEX_PHASE_LOCKED_CLOCK_H
//----------------------------------------------------------------------------//
#include <cc++/thread.h>
#include "Clock.h"
#include "VariableFrequencyClock.h"
//----------------------------------------------------------------------------//
using namespace ost;
namespace dex {
//----------------------------------------------------------------------------//

/**
 * A method to synchronize one oscillator based on another.
 * Basically, the PhaseLockedClock is constructed around a primary clock
 * and a reference clock.  The PhaseLockClock then creates a
 * VariableFrequency clock around the primary clock.  It also spawns a thread
 * that periodically measures the phase of the VariableFrequencyClock and
 * the reference clock in order to keep them in sync.
 *
 * - Assumes the primary and reference clocks run at 1000000 Hz
 * - The update interval is initially set at 1 second
 * - the actuial updates occur randomly at up to %10 of the update interval.
 *   this is to prevent the server from being swamped with synchronus requests
 */
class PhaseLockedClock : public Clock, private Thread, private Mutex
{
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
	PhaseLockedClock(Clock& primary, Clock& reference);
    
    /**
     * Cleans up all resources associated with this Clock and
     * stops the thread associated with this Active Object
     */
    ~PhaseLockedClock();
    
    // returns the value of the PLC
    // may throw a ClockException if clock goes out of sync
    
    /**
     * Returns the value of this PhaseLockedClock.  If for some reason
     * the clock has become out of sync with the refernce clock then
     * this call will throw a ClockException.
     */
    timestamp_t getValue();
    
    /**
     * @return true if the clock is in sync with the reference clock.
     *         false otherwise.
     *
     * This clock may become out of sync with the reference clock
     * in a number of ways.  If the last update was too long gao,
     * or the phase is detected to be too great, then the clock
     * goes into out-of-sync mode and will throw a ClockException 
     * on any attempt to getValue() or getPhase()
     */
    bool isSynchronized();
    
    /**
     * @return the phase offset from the reference clock.
     * - Throws a ClockException if out-of-sync.
     */
    int getOffset();
    
    /**
     * If the phase of the PhaseLockedClock is detected to be far 
     * enough away from the reference clock, then this panics
     * and sets the mode to out-of-sync.
     *
     * @param phasePanic the maximum allowable phase away
     *        from the reference clock given in microseconds (usec)
     */
    void setPhasePanic(timestamp_t phasePanic);
    
    /**
     * If the time since the last sucessful update is more than
     * this ammount, then the clock panics and goes into 
     * out-of-sync mode.
     *
     * @param updatePanic the maxumum allowable time between
     *        sucessful updates before a panic.  Given in 
     *        microseconds (usec).
     */
    void setUpdatePanic(timestamp_t updatePanic);
    
        
protected:

    /**
     * When the thread for this Active Object is started,
     * it calls this run function.
     */
    void run();

    /**
     * The thread for this object calls the update method once
     * every update interval (give or take %10).
     */
    void update();
    
    
    /**
     * Tries to update the markers for the clocks.
     * @return true on success.
     */
    bool updatePhase();

    /**
     * Tries to update the clock to slew it into step.
     * @return true on success.
     */    
    bool updateClock();

    /**
     * Sets the clock to the reference clock. This action is
     * taken to get the clock back into sync after entering
     * out-of-sync mode.
     *
     * Note: this does not slew the clock. It does a hard reset.
     */
    void setClock();

private:

	PhaseLockedClock(PhaseLockedClock& c);
	PhaseLockedClock& operator=(PhaseLockedClock& rhs);

    Clock& primaryClock_;
    
	Clock& referenceClock_;

	VariableFrequencyClock variableFrequencyClock_;

    int updateInterval_; // in usec
    
    // true if the clock is in sync.
    bool inSync_;
    
    // phase between the VFC and the reference clock
    timestamp_t thisPhase_;
    timestamp_t lastPhase_;
    
    // value of the VFC 
    timestamp_t thisVariableValue_;
    timestamp_t lastVariableValue_;
    
    // value of the primary clock
    timestamp_t thisPrimaryValue_;
    timestamp_t lastPrimaryValue_;
    
    // the average measurement of the primary clock's frequency
    // with respect to the reference clock.
    double primaryFrequencyAvg_;
    
    // if the phase is detected more than this
    // then the clock goes out of sync
    // in usec
    timestamp_t phasePanic_;
    
    // if the last update is longer than this
    // then the clock goes out of sync
    // in usec
    timestamp_t updatePanic_;
    
    // the timestamp of the last sucessfull update.
    timestamp_t lastUpdate_;
    
}; // class PhaseLockedClock
//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_PHASE_LOCKED_CLOCK_H
//----------------------------------------------------------------------------//

