#include "PhaseLockedClock.h"

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

#ifdef DEBUG
// The extra output from these cout<<'s breaks "make test".
using std::cout;
using std::endl;
#endif

namespace dex {

// A guard for the vfc and refclock's getPhase().
// One mutex for *all* PLCs may be too careful in practice,
// but it lets multiple PLCs use the same reference clock.
std::mutex mutexPLC;

// Typically 9223372036854775807 usec, or 293,000 years, obviously invalid.
static constexpr auto invalid = std::numeric_limits<timestamp_t>::max();

PhaseLockedClock::PhaseLockedClock(Clock &primary, Clock &reference)
    : primaryClock_(primary)
    , referenceClock_(reference)
    , variableFrequencyClock_(primary)
    , inSync_(false)
    , phase_{0}
    , phasePrev_{0}
    , variableValue_{0}
    , variableValuePrev_{0}
    , primaryValue_{0}
    , primaryValuePrev_{0}
    , primaryFrequencyAvg_(1000000.0)
    , phasePanic_(5000)
    , updatePanic_(5000000)
    , updatePrev_(0)
{
}

timestamp_t PhaseLockedClock::getValue()
{
    if (!inSync_)
        return invalid;
    const std::lock_guard<std::mutex> lock(mutexPLC);
    return variableFrequencyClock_.getValue();
}

int PhaseLockedClock::getOffset()
{
    if (!inSync_)
        return std::numeric_limits<int>::max();  // == clockkit.cpp ckOffset() int invalid.
    return phase_;
}

void PhaseLockedClock::run(std::atomic_bool &end_clocks)
{
    // Smear how often we update(), to not overload the server with simultaneous requests.
    std::uniform_real_distribution<double> vary(0.95, 1.05);  // +-5%
    std::default_random_engine randNumGen;
    randNumGen.seed(std::random_device{}());
    while (!end_clocks) {
        update();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(200ms * vary(randNumGen));
    }
}

void PhaseLockedClock::update()
{
    // This logic is brittle, because of how inSync_ is used and set willy-nilly.
    if (inSync_ && primaryValue() > updatePrev_ + updatePanic_) {
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
    if (phase == invalid) {
#ifdef DEBUG
        cout << "lost sync: problem with referenceClock_" << endl;
#endif
        inSync_ = false;
        return false;
    }

    phasePrev_ = phase_;
    variableValuePrev_ = variableValue_;
    primaryValuePrev_ = primaryValue_;

    phase_ = phase;  // Not invalid, because that was just checked.
    variableValue_ = variableValue;
    primaryValue_ = tmp;

#ifdef DEBUG
    cout << "phase := " << phase << endl;
#endif
    return true;
}

bool PhaseLockedClock::updateClock()
{
    if (phase_ == invalid) {
#ifdef DEBUG
        cout << "lost sync: invalid phase" << endl;
#endif
        inSync_ = false;
        return false;
    }
    // todo: phasePanic_ unsigned, or timestamp_t phase_ unsigned, or abs().
    if (phase_ > phasePanic_ || phase_ < -phasePanic_) {
        // The phase is too large.
#ifdef DEBUG
        cout << "lost sync: " << phase_ << " > " << phasePanic_ << endl;
#endif
        inSync_ = false;
        return false;
    }

    // todo: tidy this mishmash of int usec and double.
    {
        // Measure referenceClock_'s elapsed time.
        const auto referenceValuePrev = variableValuePrev_ + phasePrev_;
        const auto referenceValue = variableValue_ + phase_;
        const double referenceElapsed = referenceValue - referenceValuePrev;

        // Estimate the primary clock's frequency.
        // Average away noise with an IIR filter.
        const auto ticks = primaryValue_ - primaryValuePrev_;
        const auto primaryFrequency = 1000000.0 * ticks / referenceElapsed;
        primaryFrequencyAvg_ += (primaryFrequency - primaryFrequencyAvg_) * 0.1;
    }
#ifdef DEBUG
    cout << "primary clock's frequency average = " << int(primaryFrequencyAvg_) << endl;
#endif

    // Adjust the variable clock's frequency.
    const auto phaseDiff = phase_ * 0.1;
    const auto frequencyDiff = 1000000.0 - primaryFrequencyAvg_;
    const int variableClockFrequency = 1000000 + frequencyDiff + phaseDiff;
#ifdef DEBUG
    cout << "frequency := " << variableClockFrequency << endl;
#endif
    const std::lock_guard<std::mutex> lock(mutexPLC);
    variableFrequencyClock_.setFrequency(variableClockFrequency);
    return true;
}

void PhaseLockedClock::setClock()
{
#ifdef DEBUG
    cout << "resyncing" << endl;
#endif
    inSync_ = true;
    const std::lock_guard<std::mutex> lock(mutexPLC);
    variableFrequencyClock_.setValue(referenceClock_.getValue());
}

}  // namespace dex
