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
    const auto t = diff(clockSrc_.getValue(), markerSrc_);
    if (t == durInvalid)
        return tpInvalid;
    const auto ticksSrc = UsecFromDur(t);
    if (ticksSrc < 0) {
        rolledOver_ = true;
        return tpInvalid;
    }
    // ticksSrc isn't invalid.
    // marker_ is often invalid at the start of make test-30.
    return marker_ == tpInvalid ? tpInvalid : marker_ + DurFromUsec(ticksSrc * (frequency_ / frequencySrc_));
}

void VariableFrequencyClock::setValue(tp t)
{
    marker_ = t;                        // possibly tpInvalid
    markerSrc_ = clockSrc_.getValue();  // possibly tpInvalid
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
