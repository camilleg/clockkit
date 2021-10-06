#include "VariableFrequencyClock.h"

namespace dex {

VariableFrequencyClock::VariableFrequencyClock(Clock& src)
    : clockSrc_(src)
    , frequencySrc_(1000000.0)
    , frequency_(1000000.0)
    , markerSrc_(clockSrc_.getValue())
    , marker_(TpFromUsec(0))
    , rolledOver_(false)
{
    // setValue(TpFromUsec(0)) works too, but -Werror=effc++ prefers the initializer list.
}

tp VariableFrequencyClock::getValue()
{
    if (rolledOver_)
        return tpInvalid;
    const auto ticksSrc = UsecFromDur(clockSrc_.getValue() - markerSrc_);
    if (ticksSrc < 0) {
        rolledOver_ = true;
        return tpInvalid;
    }
    return marker_ + DurFromUsec(ticksSrc * (frequency_ / frequencySrc_));
}

void VariableFrequencyClock::setValue(tp t)
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

void VariableFrequencyClock::setFrequency(double freq)
{
    updateMarkers();
    // A nonpositive frequency is silently ignored.
    if (freq > 0.0)
        frequency_ = freq;
}

}  // namespace dex
