
//----------------------------------------------------------------------------//
#ifndef DEX_PERFORMANCE_COUNTER_CPP
#define DEX_PERFORMANCE_COUNTER_CPP
//----------------------------------------------------------------------------//
#include "PerformanceCounter.h"
#include "Exceptions.h"
#include <windows.h>

//----------------------------------------------------------------------------//
namespace dex {
using namespace std;
//----------------------------------------------------------------------------//
PerformanceCounter PerformanceCounter::instance_;

//----------------------------------------------------------------------------//
PerformanceCounter::PerformanceCounter()
{
	LARGE_INTEGER rate;
	if (!QueryPerformanceFrequency(&rate))
		throw ClockException("Error getting performance counter frequency");
	freqConversion_ = 1000000.0 / (double)rate.QuadPart;
}

//----------------------------------------------------------------------------//

PerformanceCounter& PerformanceCounter::instance()
{
	return instance_;
}

//----------------------------------------------------------------------------//
timestamp_t PerformanceCounter::getValue()
{
	LARGE_INTEGER qpc;
	if (!QueryPerformanceCounter(&qpc))
		throw ClockException("Error getting performance counter");
	return (timestamp_t)(((double)qpc.QuadPart) * freqConversion_);
}


//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_PERFORMANCE_COUNTER_CPP
//----------------------------------------------------------------------------//

