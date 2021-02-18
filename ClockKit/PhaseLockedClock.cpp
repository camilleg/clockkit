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
        // cout << "PhaseLockedClock picked up exception, now out of sync." << endl;
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
    const auto updateInterval = 100000.0;        // 10 Hz.  From the config file?
    const auto variance = updateInterval * 0.1;  // +-10%
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
    if (inSync_ && primaryClock_.getValue() - lastUpdate_ > updatePanic_) {
        // Last update too long ago.
        inSync_ = false;
    }
    if (inSync_) {
        if (!updateClock())
            return;
    }
    else {
        setClock();
    }
    if (inSync_ && updatePhase())
        lastUpdate_ = primaryClock_.getValue();
}

bool PhaseLockedClock::updatePhase()
{
    try {
        enterMutex();
        const timestamp_t phase = referenceClock_.getPhase(variableFrequencyClock_);
        const timestamp_t variableValue = variableFrequencyClock_.getValue();
        const timestamp_t primaryValue = primaryClock_.getValue();
        leaveMutex();

        lastPhase_ = thisPhase_;
        lastVariableValue_ = thisVariableValue_;
        lastPrimaryValue_ = thisPrimaryValue_;

        thisPhase_ = phase;
        thisVariableValue_ = variableValue;
        thisPrimaryValue_ = primaryValue;

        // cout << "detected phase: " << ((int)phase) << endl;
        return true;
    }
    catch (ClockException &e) {
        leaveMutex();
        // cout << "PLC handling clock exception: " << e.getMessage() << endl;
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
    // cout << "primary clock frequency average: " << ((int)primaryFrequencyAvg_) << endl;

    if (thisPhase_ > phasePanic_ || thisPhase_ < -phasePanic_) {
        // The phase is too high, so declare the clock out of sync.
        inSync_ = false;
        return false;
    }

    // calculate the adjustment for the variable clock's frequency
    const double phaseDiff = thisPhase_ * 0.1;
    const double frequencyDiff = 1000000 - primaryFrequencyAvg_;
    const double variableClockFrequency = 1000000 + (frequencyDiff + phaseDiff);
    // cout << "using frequency: " << ((int)variableClockFrequency) << endl;

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
        // cout << "CLOCK IN SYNC" << endl;
    }
    catch (ClockException &e) {
        // cout << "exception while resetting to the reference clock" << endl;
        inSync_ = false;
    }
    leaveMutex();
}

}  // namespace dex
