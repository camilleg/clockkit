//----------------------------------------------------------------------------//
#ifndef DEX_CLOCK_H
#define DEX_CLOCK_H
//----------------------------------------------------------------------------//
#include "Common.h"
//----------------------------------------------------------------------------//
namespace dex {
//----------------------------------------------------------------------------//

/**
 * An abstract class that represents a basic clock
 * The frequency for all clocks is 1000000 Hz with the notable exception
 * of VariableFrequencyClock.
 */
class Clock
{
public:

	/**
	 * @return The present value of the clock.
	 * 
	 * NOTE: For ClockClient this call may take some time.
	 * The return value measures the time when the function returns, 
	 * not when it was called.
     *
     * Most clocks tick all the time, However, some clocks such as the Win2k/XP
     * system clock tick at a slow rate. 
	 */
	virtual timestamp_t getValue() = 0;
	
	/**
	 * @return The phase (or offset) between the this (primary) clock and
	 *         the provided (secondary) clock.
	 * 
	 * secondaryClock + phase = primaryClock
	 * 
	 * For clocks like RemoteClock, this measurement will be much more 
	 * percise than comparing getValue() on two seperate clocks.
	 * 
	 * The default implementation is:
	 * this->getValue() - c.getValue();	
	 */	
	virtual timestamp_t getPhase(Clock& c);	
};


//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_CLOCK_H
//----------------------------------------------------------------------------//

