#ifndef DEX_PERFORMANCE_COUNTER_H
#define DEX_PERFORMANCE_COUNTER_H

#include "Clock.h"

namespace dex {
  // This Windows-only class represents the CPU's high resolution timer,
  // adjusted to tick at 1000000 Hz.
  // It may wrap around.  It's not necessarly monotonic.
  class PerformanceCounter : public Clock {
  public:
    // Thread-safe singleton.
    static PerformanceCounter& instance();
    // System clock, in microseconds since UNIX epoch.
    virtual timestamp_t getValue();

  private:
    PerformanceCounter();
    PerformanceCounter(PerformanceCounter&);
    PerformanceCounter& operator=(PerformanceCounter&);
    static PerformanceCounter instance_;
    double freqConversion_;
  };
}
#endif
