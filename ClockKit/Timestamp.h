#pragma once
#include <array>
#include <chrono>
#include <cstddef>
#include <limits>
#include <string>

namespace dex {

// Duration and time point are the fundamental types,
// with microsecond resolution.

using dur = std::chrono::duration<int64_t, std::micro>;
using tp = std::chrono::time_point<std::chrono::system_clock, dur>;

// Converters.

std::string timestampToString(tp);
tp stringToTimestamp(const std::string&);

// Read/write a 64 bit timestamp with proper byte ordering.
// Convert xxxInvalid conventionally, without special treatment.
void timestampToBytes(tp, std::byte*);
tp bytesToTimestamp(const std::byte*);

dur constexpr DurFromUsec(int64_t t)
{
    return dur(t);
}
tp constexpr TpFromUsec(int64_t t)
{
    return tp(dur(t));
}

int64_t constexpr UsecFromTp(tp t)
{
    return t.time_since_epoch() / std::chrono::microseconds(1) /*aka 1us*/;
}
int64_t constexpr UsecFromDur(dur t)
{
    return UsecFromTp(tp(t));
}

// Obviously invalid values.  9223372036854775807 usec, or 293,000 years.
constexpr int64_t usecInvalid = std::numeric_limits<int64_t>::max();
constexpr tp tpInvalid = TpFromUsec(usecInvalid);
constexpr dur durInvalid = DurFromUsec(usecInvalid);

// Avoid arithmetic with tpInvalid.
#if 0
// This compiles, but it recurses infinitely and thus overflows the stack.
dur constexpr operator-(const tp a, const tp b)
#else
dur constexpr diff(const tp a, const tp b)
#endif
{
    return a == tpInvalid || b == tpInvalid ? durInvalid : a - b;
}

}  // namespace dex
