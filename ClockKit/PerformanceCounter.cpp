#include "PerformanceCounter.h"
#include <windows.h>

namespace dex {
PerformanceCounter PerformanceCounter::instance_;

// QueryPerformanceFrequency and QueryPerformanceCounter *could* fail
// only on Windows 2000, which doesn't matter anymore.
// If they fail, we set freqConversion_ negative,
// and getValue() always returns zero.

PerformanceCounter::PerformanceCounter()
{
    LARGE_INTEGER rate;
    freqConversion_ = QueryPerformanceFrequency(&rate) ? 1000000.0 / rate.QuadPart : -1.0;
}

PerformanceCounter& PerformanceCounter::instance()
{
    return instance_;
}

timestamp_t PerformanceCounter::getValue()
{
    if (freqConversion_ < 0.0)
        return 0;

    LARGE_INTEGER qpc;
    (void)QueryPerformanceCounter(&qpc);
    return qpc.QuadPart * freqConversion_;
}
}  // namespace dex
