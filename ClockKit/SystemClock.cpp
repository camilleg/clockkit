#include "SystemClock.h"

namespace dex {
SystemClock SystemClock::instance_;

tp SystemClock::getValue()
{
    using namespace std::chrono;
    const auto now = system_clock::now();
    return TpFromUsec(time_point_cast<microseconds>(now).time_since_epoch().count());
}
}  // namespace dex
