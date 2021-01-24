#include "Clock.h"

namespace dex {
timestamp_t Clock::getPhase(Clock& c)
{
    return getValue() - c.getValue();
}
}
