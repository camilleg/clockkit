#include "PhaseLockedClock.h"

#include <cstdlib>
#include <mutex>
#include <random>
#include <thread>

#ifdef DEBUG
// The extra output from these cout<<'s breaks "make test".
using std::cout;
using std::endl;
#endif

namespace dex {

using namespace std::chrono;

// A guard for the vfc and refclock's getPhase().
// One mutex for *all* PLCs may be too careful in practice,
// but it lets multiple PLCs use the same reference clock.
std::mutex mutexPLC;
using Guard = const std::lock_guard<std::mutex>;

PhaseLockedClock::PhaseLockedClock(Clock &primary, Clock &reference)
    : primaryClock_(primary)
    , referenceClock_(reference)
    , variableFrequencyClock_(primary)
    , inSync_(false)
    , phase_(0s)
    , phasePrev_(0s)
    , variableValue_(0s)
    , variableValuePrev_(0s)
    , primaryValue_(0s)
    , primaryValuePrev_(0s)
    , primaryFrequencyAvg_(1000000.0)
    , phasePanic_(5ms)
    , updatePanic_(5s)
    , updatePrev_(0s)
{
}

tp PhaseLockedClock::getValue()
{
    if (!inSync_)
        return tpInvalid;
    Guard guard(mutexPLC);
    return variableFrequencyClock_.getValue();
}

dur PhaseLockedClock::getOffset()
{
    return inSync_ ? phase_ : durInvalid;
}

void PhaseLockedClock::run(std::atomic_bool &end_clocks)
{
    // Smear how often we update(), to not overload the server with simultaneous requests.
    std::uniform_real_distribution<double> vary(1 / 1.05, 1.05);  // +-5%
    std::default_random_engine randNumGen;
    randNumGen.seed(std::random_device{}());
    while (!end_clocks) {
        update();
        std::this_thread::sleep_for(200ms * vary(randNumGen));
    }
}

void PhaseLockedClock::update()
{
    if (updatePrev_ == tpInvalid) {
        inSync_ = false;
    }
    else {
        const auto pv = primaryValue();
        if (pv == tpInvalid) {
            inSync_ = false;
        }
        else {
            // This logic is brittle, because of how inSync_ is used and set willy-nilly.
            // pv, updatePrev_, and updatePanic_ are not invalid.
            if (inSync_ && pv > updatePrev_ + updatePanic_) {
                // The previous update was too long ago.
                inSync_ = false;
            }
        }
    }

    if (inSync_) {
        if (!updateClock())
            return;
    }
    else {
        setClock();
    }
    if (updatePhase()) {
        updatePrev_ = primaryValue();
    }
}

bool PhaseLockedClock::updatePhase()
{
    if (!inSync_)
        return false;
    mutexPLC.lock();
    const auto phase = referenceClock_.getPhase(variableFrequencyClock_);
    const auto variableValue = variableFrequencyClock_.getValue();
    const auto tmp = primaryValue();  // Not guarded by the mutex.  But read all 3 numbers at once.
    mutexPLC.unlock();
    if (phase == durInvalid) {
#ifdef DEBUG
        cout << "lost sync: problem with referenceClock_" << endl;
#endif
        inSync_ = false;
        return false;
    }
    if (variableValue == tpInvalid) {
#ifdef DEBUG
        cout << "lost sync: problem with vfc_" << endl;
#endif
        inSync_ = false;
        return false;
    }
    if (tmp == tpInvalid) {
#ifdef DEBUG
        cout << "lost sync: problem with primaryClock_" << endl;
#endif
        inSync_ = false;
        return false;
    }

    // None of these are invalid, because their values come from the next 3 lines.
    phasePrev_ = phase_;
    variableValuePrev_ = variableValue_;
    primaryValuePrev_ = primaryValue_;

    // We've checked that none of these are invalid.
    phase_ = phase;
    variableValue_ = variableValue;
    primaryValue_ = tmp;

#ifdef DEBUG
    cout << "phase := " << phase << endl;
#endif
    return true;
}

bool PhaseLockedClock::updateClock()
{
    if (phase_ == durInvalid) {
#ifdef DEBUG
        cout << "lost sync: invalid phase" << endl;
#endif
        inSync_ = false;
        return false;
    }
    // abs() would need -std=c++1z, or UsecFromDur.
    if (phase_ > phasePanic_ || phase_ < -phasePanic_) {
        // The phase is too large.
#ifdef DEBUG
        cout << "lost sync: abs(" << phase_ << ") > " << phasePanic_ << endl;
#endif
        inSync_ = false;
        return false;
    }

    {
        // Measure referenceClock_'s elapsed time.
        // None of these are invalid.
        const auto referenceValuePrev = variableValuePrev_ + phasePrev_;
        const auto referenceValue = variableValue_ + phase_;
        const auto referenceElapsed = UsecFromDur(referenceValue - referenceValuePrev);

        // Estimate the primary clock's frequency.
        // None of these are invalid.
        // Average away noise with an IIR filter.
        const auto ticks = UsecFromDur(primaryValue_ - primaryValuePrev_);
        const auto primaryFrequency = ticks * 1000000.0 / referenceElapsed;
        primaryFrequencyAvg_ += (primaryFrequency - primaryFrequencyAvg_) * 0.1;
    }
#ifdef DEBUG
    cout << "primary clock's frequency average = " << int(primaryFrequencyAvg_) << endl;
#endif

    // Adjust the variable clock's frequency.
    // phase_ isn't invalid.
    const auto phaseDiff = UsecFromDur(phase_) * 0.1;
    const auto frequencyDiff = 1000000.0 - primaryFrequencyAvg_;
    const auto variableClockFrequency = 1000000.0 + frequencyDiff + phaseDiff;
#ifdef DEBUG
    cout << "frequency := " << variableClockFrequency << endl;
#endif
    Guard guard(mutexPLC);
    variableFrequencyClock_.setFrequency(variableClockFrequency);
    return true;
}

void PhaseLockedClock::setClock()
{
#ifdef DEBUG
    cout << "resyncing" << endl;
#endif
    inSync_ = true;
    Guard guard(mutexPLC);
    variableFrequencyClock_.setValue(referenceClock_.getValue());
}

}  // namespace dex
