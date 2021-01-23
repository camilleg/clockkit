#ifndef DEX_CLOCK_CPP
#define DEX_CLOCK_CPP

#include "Clock.h"

namespace dex {
    timestamp_t Clock::getPhase(Clock& c)
    {
	return ( getValue() - c.getValue() );	
    }
}

#endif
