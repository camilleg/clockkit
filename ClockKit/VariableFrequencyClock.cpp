#include "VariableFrequencyClock.h"

#include "Exceptions.h"
#include "limits"

namespace dex {

VariableFrequencyClock::VariableFrequencyClock(Clock& src)
    : clockSrc_(src)
    , frequencySrc_(1000000)
    , frequency_(1000000)
    , markerSrc_(clockSrc_.getValue())
    , marker_(0)
    , rolledOver_(false)
{
    // setValue(0); would also work, but -Werror=effc++ says to do this in the initializer list.
}

timestamp_t VariableFrequencyClock::getValue()
{
    // Typically -2147483647 usec, or -35 minutes, obviously invalid.
    static const auto invalid = -std::numeric_limits<int>::max();
    if (rolledOver_)
        return invalid;

    const timestamp_t ticksSrc = clockSrc_.getValue() - markerSrc_;
    if (ticksSrc < 0) {
        rolledOver_ = true;
        return invalid;
    }
    return marker_ + ticksSrc * frequency_ / frequencySrc_;
}

void VariableFrequencyClock::setValue(timestamp_t t)
{
    marker_ = t;
    markerSrc_ = clockSrc_.getValue();
    rolledOver_ = false;
}

void VariableFrequencyClock::updateMarkers()
{
    if (!rolledOver_)
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
