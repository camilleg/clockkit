#include "VariableFrequencyClock.h"

#include "Exceptions.h"

namespace dex {

VariableFrequencyClock::VariableFrequencyClock(Clock& src)
    : clockSrc_(src)
    , frequencySrc_(1000000)
    , frequency_(1000000)
    , markerSrc_(clockSrc_.getValue())
    , marker_(0)
{
    // setValue(0); would also work, but -Werror=effc++ says to do this in the initializer list.
}

std::pair<timestamp_t, timestamp_t> VariableFrequencyClock::getTicks() const
{
    const timestamp_t ticksSrc = clockSrc_.getValue() - markerSrc_;
    if (ticksSrc < 0)
        throw ClockException("Clock Rollover Detected");
    const timestamp_t ticks = ticksSrc * frequency_ / frequencySrc_;
    return std::make_pair(ticksSrc, ticks);
}

timestamp_t VariableFrequencyClock::getValue()
{
    return marker_ + getTicks().second;
}

void VariableFrequencyClock::setValue(timestamp_t t)
{
    marker_ = t;
    markerSrc_ = clockSrc_.getValue();
}

void VariableFrequencyClock::updateMarkers()
{
    setValue(getValue());
}

void VariableFrequencyClock::setFrequency(int freq)
{
    updateMarkers();
    // A nonpositive frequency is silently ignored.
    if (freq > 0)
        frequency_ = freq;
}

}  // namespace dex
