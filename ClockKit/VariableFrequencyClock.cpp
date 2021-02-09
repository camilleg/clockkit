#include "VariableFrequencyClock.h"
#include "Exceptions.h"

namespace dex {

VariableFrequencyClock::VariableFrequencyClock(Clock& master)
    : masterClock_(master)
    , masterFrequency_(1000000)
    , slaveFrequency_(1000000)
    , masterMarker_(masterClock_.getValue())
    , slaveMarker_(0)
{
}

timestamp_t VariableFrequencyClock::getValue()
{
    const timestamp_t master = masterClock_.getValue();
    const timestamp_t masterTicks = master - masterMarker_;
    if (masterTicks < 0)
        throw ClockException("Clock Rollover Detected");
    const timestamp_t slaveTicks = (masterTicks * slaveFrequency_) / masterFrequency_;
    return slaveMarker_ + slaveTicks;
}

void VariableFrequencyClock::setValue(timestamp_t t)
{
    masterMarker_ = masterClock_.getValue();
    slaveMarker_ = t;
}

void VariableFrequencyClock::setFrequency(int freq)
{
    updateMarkers();
    if (freq <= 0)
        throw ClockException("frequency set to negative rate");
    slaveFrequency_ = freq;
}

void VariableFrequencyClock::updateMarkers()
{
    const timestamp_t master = masterClock_.getValue();
    const timestamp_t masterTicks = master - masterMarker_;
    if (masterTicks < 0)
        throw ClockException("Clock Rollover Detected");
    const timestamp_t slaveTicks = (masterTicks * slaveFrequency_) / masterFrequency_;
    slaveMarker_ += slaveTicks;
    // todo: decopypaste the above with getValue()
    masterMarker_ = master;
}

}  // namespace dex
