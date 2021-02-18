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

std::pair<timestamp_t, timestamp_t> VariableFrequencyClock::getTicks()
{
    const timestamp_t master = masterClock_.getValue();
    const timestamp_t masterTicks = master - masterMarker_;
    if (masterTicks < 0)
        throw ClockException("Clock Rollover Detected");
    const timestamp_t slaveTicks = (masterTicks * slaveFrequency_) / masterFrequency_;
    return std::make_pair(masterTicks, slaveTicks);
}

timestamp_t VariableFrequencyClock::getValue()
{
    return slaveMarker_ + getTicks().second;
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
    slaveMarker_ += getTicks().second;
    masterMarker_ = masterClock_.getValue();
}

}  // namespace dex
