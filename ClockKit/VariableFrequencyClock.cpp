#include "VariableFrequencyClock.h"

namespace dex {

VariableFrequencyClock::VariableFrequencyClock(Clock& src, double frequency)
    : clockSrc_(src)
    , frequency_(frequency)
    , markerSrc_(src.getValue())
    , marker_(tp0)
    , rolledOver_(false)
{
    // setValue(tp0) works too, but -Werror=effc++ prefers the initializer list.
}

tp VariableFrequencyClock::getValue()
{
    if (rolledOver_)
        return tpInvalid;
    const auto tSrc = diff(clockSrc_.getValue(), markerSrc_);
    if (tSrc == durInvalid)
        return tpInvalid;
    const auto ticksSrc = UsecFromDur(tSrc);
    if (ticksSrc < 0) {
        rolledOver_ = true;
        return tpInvalid;
    }
    constexpr auto frequencySrc_ = 1000000.0;  // Because clockSrc_ isn't also a VFC.
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
    if (freq <= 0.0)
        return;
    frequency_ = freq;
    updateMarkers();
}

}  // namespace dex
