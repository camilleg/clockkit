#include "PhaseLockedClock.h"

#include <cstdlib>
#include <iostream>

#include "Exceptions.h"

using namespace std;
using namespace ost;
namespace dex {

PhaseLockedClock::PhaseLockedClock(Clock &primary, Clock &reference)
    : Thread(2)  // high priority thread
    , primaryClock_(primary)
    , referenceClock_(reference)
    , variableFrequencyClock_(primary)
    , inSync_(false)
    , thisPhase_{0}
    , lastPhase_{0}
    , thisVariableValue_{0}
    , lastVariableValue_{0}
    , thisPrimaryValue_{0}
    , lastPrimaryValue_{0}
    , primaryFrequencyAvg_(1000000)
    , phasePanic_(5000)
    , updatePanic_(5000000)
    , lastUpdate_(0)
{
    start();
}

timestamp_t PhaseLockedClock::getValue()
{
    if (!inSync_)
        throw ClockException("PhaseLockedClock not in sync");
    try {
        enterMutex();
        timestamp_t time = variableFrequencyClock_.getValue();
        leaveMutex();
        return time;
    }
    catch (ClockException &e) {
        leaveMutex();
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
    enterMutex();
    const int offset = thisPhase_;
    leaveMutex();
    return offset;
}

void PhaseLockedClock::run()
{
    // These are in usec.
    const auto updateInterval = 1000000.0;
    const auto variance = updateInterval * 0.1;  // 10%
    const auto base = updateInterval - variance * 0.5;
    while (!testCancel()) {
        update();
        const auto random = (rand() % int(variance * 1000.0)) * 0.001;
        const auto sleep_ms = int((base + random) * 0.001);
        sleep(sleep_ms);
    }
    exit();
}

void PhaseLockedClock::update()
{
    if (inSync_ && (primaryClock_.getValue() - lastUpdate_) > updatePanic_) {
#ifdef DEBUG
        cout << "last update too long ago." << endl;
#endif
        inSync_ = false;
    }
    if (!inSync_) {
#ifdef DEBUG
        cout << "CLOCK OUT OF SYNC" << endl;
#endif
        setClock();
        if (inSync_ && updatePhase())
            lastUpdate_ = primaryClock_.getValue();
        return;
    }
    if (!updatePhase()) {
#ifdef DEBUG
        cout << "PHASE UPDATE FAILED" << endl;
#endif
        return;
    }
    if (!updateClock()) {
#ifdef DEBUG
        cout << "CLOCK UPDATE FAILED" << endl;
#endif
        return;
    }
    // Mark a timestamp for sucessful update.
    lastUpdate_ = primaryClock_.getValue();
}

bool PhaseLockedClock::updatePhase()
{
    try {
        enterMutex();
        timestamp_t phase = referenceClock_.getPhase(variableFrequencyClock_);
        timestamp_t variableValue = variableFrequencyClock_.getValue();
        timestamp_t primaryValue = primaryClock_.getValue();
        leaveMutex();

        lastPhase_ = thisPhase_;
        lastVariableValue_ = thisVariableValue_;
        lastPrimaryValue_ = thisPrimaryValue_;

        thisPhase_ = phase;
        thisVariableValue_ = variableValue;
        thisPrimaryValue_ = primaryValue;

#ifdef DEBUG
        cout << "detected phase: " << ((int)phase) << endl;
#endif
        return true;
    }
    catch (ClockException &e) {
        leaveMutex();
#ifdef DEBUG
        cout << "PLC handling clock exception: " << e.getMessage() << endl;
#endif
        return false;
    }
}

bool PhaseLockedClock::updateClock()
{
    // calculate the elapsed time in seconds on the reference clock
    const timestamp_t lastReferenceValue = lastVariableValue_ + lastPhase_;
    const timestamp_t thisReferenceValue = thisVariableValue_ + thisPhase_;
    const double referenceElapsed = thisReferenceValue - lastReferenceValue;

    // find the primary clock's frequency (filter for noise)
    const double primaryTicks = thisPrimaryValue_ - lastPrimaryValue_;
    const double primaryFrequency = 1e6 * primaryTicks / referenceElapsed;
    primaryFrequencyAvg_ += (primaryFrequency - primaryFrequencyAvg_) * 0.1;
#ifdef DEBUG
    cout << "primary clock frequency average: " << ((int)primaryFrequencyAvg_) << endl;
#endif

    if (thisPhase_ > phasePanic_ || thisPhase_ < -phasePanic_) {
        // The phase is too high, so declare the clock out of sync.
        inSync_ = false;
        return false;
    }

    // calculate the adjustment for the variable clock's frequency
    const double phaseDiff = thisPhase_ * 0.1;
    const double frequencyDiff = 1000000 - primaryFrequencyAvg_;
    const double variableClockFrequency = 1000000 + (frequencyDiff + phaseDiff);
#ifdef DEBUG
    cout << "using frequency: " << ((int)variableClockFrequency) << endl;
#endif

    enterMutex();
    variableFrequencyClock_.setFrequency((int)variableClockFrequency);
    leaveMutex();
    return true;
}

void PhaseLockedClock::setClock()
{
    enterMutex();
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
    leaveMutex();
}

}  // namespace dex
