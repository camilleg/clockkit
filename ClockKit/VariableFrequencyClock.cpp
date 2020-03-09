//----------------------------------------------------------------------------//
#ifndef DEX_VARIABLE_FREQUENCY_CLOCK_CPP
#define DEX_VARIABLE_FREQUENCY_CLOCK_CPP
//----------------------------------------------------------------------------//
#include "VariableFrequencyClock.h"
#include "Exceptions.h"
//----------------------------------------------------------------------------//
namespace dex {
//----------------------------------------------------------------------------//
VariableFrequencyClock::VariableFrequencyClock(Clock& master)
	:masterClock_(master)
{
	masterFrequency_ = 1000000; // master clock assumed to run at usec ticks
	slaveFrequency_ = 1000000;
	masterMarker_ = masterClock_.getValue();
	slaveMarker_ = 0;	
}


//----------------------------------------------------------------------------//
timestamp_t VariableFrequencyClock::getValue()
{
	timestamp_t master = masterClock_.getValue();
	timestamp_t masterTicks = master - masterMarker_;
	if (masterTicks < 0) throw ClockException("Clock Rollover Detected");
	timestamp_t slaveTicks = (masterTicks * slaveFrequency_) / masterFrequency_;
	return slaveMarker_ + slaveTicks;
}


//----------------------------------------------------------------------------//	
void VariableFrequencyClock::setValue(timestamp_t t)
{
	masterMarker_ = masterClock_.getValue();
	slaveMarker_ = t;
}


//----------------------------------------------------------------------------//	
int VariableFrequencyClock::getFrequency()
{
	return (int) slaveFrequency_;	
}


//----------------------------------------------------------------------------//
void VariableFrequencyClock::setFrequency(int freq)
{
	updateMarkers();
	if (freq <= 0) throw ClockException("frequency set to negative rate");
	slaveFrequency_ = (timestamp_t) freq;	
}


//----------------------------------------------------------------------------//
void VariableFrequencyClock::updateMarkers()
{
	timestamp_t master = masterClock_.getValue();
	timestamp_t masterTicks = master - masterMarker_;
	if (masterTicks < 0) throw ClockException("Clock Rollover Detected");;
	timestamp_t slaveTicks = (masterTicks * slaveFrequency_) / masterFrequency_;
	
	masterMarker_ = master;
	slaveMarker_ += slaveTicks;
}


//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_VARIABLE_FREQUENCY_CLOCK_CPP
//----------------------------------------------------------------------------//

