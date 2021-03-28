#include "PhaseLockedClock.h"

#include <cstdlib>
#include <iostream>
#include <mutex>

#include "Exceptions.h"

#ifdef DEBUG
// The extra output from these cout<<'s breaks "make test".
using std::cout;
using std::endl;
#endif

namespace dex {

// One mutex for *all* PLC's is too careful, but not a problem in practice.
std::mutex mutexPLC;

PhaseLockedClock::PhaseLockedClock(Clock &primary, Clock &reference)
    : Thread(2)  // high priority thread
    , primaryClock_(primary)
    , referenceClock_(reference)
    , variableFrequencyClock_(primary)
    , inSync_(false)
    , phase_{0}
    , phasePrev_{0}
    , variableValue_{0}
    , variableValuePrev_{0}
    , primaryValue_{0}
    , primaryValuePrev_{0}
    , primaryFrequencyAvg_(1000000)
    , phasePanic_(5000)
    , updatePanic_(5000000)
    , updatePrev_(0)
{
    start();
}

timestamp_t PhaseLockedClock::getValue()
{
    if (!inSync_)
        throw ClockException("PhaseLockedClock not in sync");
    const std::lock_guard<std::mutex> lock(mutexPLC);
    try {
        timestamp_t time = variableFrequencyClock_.getValue();
        return time;
    }
    catch (ClockException &e) {
#ifdef DEBUG
        cout << "PhaseLockedClock picked up exception, now out of sync." << endl;
#endif
        inSync_ = false;
        throw e;
    }
}

int PhaseLockedClock::getOffset()
{
    if (!inSync_)
        throw ClockException("PhaseLockedClock not in sync");
    const std::lock_guard<std::mutex> lock(mutexPLC);
    return phase_;
}

void PhaseLockedClock::run()
{
    constexpr auto updateInterval_usec = 200000;              // 5 Hz.  From the config file?
    constexpr auto variance_usec = updateInterval_usec / 10;  // +-5%, so +- 5 msec.
    constexpr auto base_usec = updateInterval_usec - variance_usec / 2;
    while (!testCancel()) {  // ost::Thread::testCancel()
        update();
        const auto random_usec = rand() % variance_usec;
        sleep((base_usec + random_usec) / 1000 /*msec*/);  // ost::Thread::sleep()
    }
    // There's no need to ost::Thread::exit().
}

void PhaseLockedClock::update()
{
    // This logic is brittle, because of how inSync_ is used and set everywhere.
    if (inSync_ && primaryClock_.getValue() > updatePrev_ + updatePanic_) {
        // The previous update was too long ago.
        inSync_ = false;
    }
    if (inSync_) {
        if (!updateClock())
            return;
    }
    else {
        setClock();
    }
    if (updatePhase()) {
        updatePrev_ = primaryClock_.getValue();
    }
}

bool PhaseLockedClock::updatePhase()
{
    if (!inSync_)
        return false;
    try {
        mutexPLC.lock();
        const timestamp_t phase = referenceClock_.getPhase(variableFrequencyClock_);
        const timestamp_t variableValue = variableFrequencyClock_.getValue();
        const timestamp_t primaryValue = primaryClock_.getValue();
        mutexPLC.unlock();

        phasePrev_ = phase_;
        variableValuePrev_ = variableValue_;
        primaryValuePrev_ = primaryValue_;

        phase_ = phase;
        variableValue_ = variableValue;
        primaryValue_ = primaryValue;

#ifdef DEBUG
        cout << "detected phase: " << phase << endl;
#endif
        return true;
    }
    catch (ClockException &e) {
#ifdef DEBUG
        cout << "PhaseLockedClock caught exception: " << e.getMessage() << endl;
#endif
        return false;
    }
}

bool PhaseLockedClock::updateClock()
{
    // todo: tidy this mishmash of int usec and double.

    // Calculate the elapsed time in seconds on the reference clock.
    const timestamp_t referenceValuePrev = variableValuePrev_ + phasePrev_;
    const timestamp_t referenceValue = variableValue_ + phase_;
    const double referenceElapsed = referenceValue - referenceValuePrev;

    // Find the primary clock's frequency; filter for noise.
    const double primaryTicks = primaryValue_ - primaryValuePrev_;
    const auto primaryFrequency = 1000000.0 * primaryTicks / referenceElapsed;
    primaryFrequencyAvg_ += (primaryFrequency - primaryFrequencyAvg_) * 0.1;
#ifdef DEBUG
    cout << "primary clock frequency average: " << int(primaryFrequencyAvg_) << endl;
#endif

    // todo: phasePanic_ unsigned, or timestamp_t phase_ unsigned, or abs().
    if (phase_ > phasePanic_ || phase_ < -phasePanic_) {
        // The phase is too large.
        inSync_ = false;
        return false;
    }

    // Calculate the adjustment for the variable clock's frequency.
    const auto phaseDiff = phase_ * 0.1;
    const auto frequencyDiff = 1000000.0 - primaryFrequencyAvg_;
    const int variableClockFrequency = 1000000 + frequencyDiff + phaseDiff;
#ifdef DEBUG
    cout << "using frequency: " << variableClockFrequency << endl;
#endif

    const std::lock_guard<std::mutex> lock(mutexPLC);
    variableFrequencyClock_.setFrequency(variableClockFrequency);
    return true;
}

void PhaseLockedClock::setClock()
{
    const std::lock_guard<std::mutex> lock(mutexPLC);
    try {
        variableFrequencyClock_.setValue(referenceClock_.getValue());
        inSync_ = true;
#ifdef DEBUG
        cout << "CLOCK IN SYNC" << endl;
#endif
    }
    catch (ClockException &e) {
#ifdef DEBUG
        cout << "exception while resetting to the reference clock" << endl;
#endif
        inSync_ = false;
    }
}

}  // namespace dex
