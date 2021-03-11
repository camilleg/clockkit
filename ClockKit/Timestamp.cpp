#include "Timestamp.h"

#include <cc++/config.h>
#include <stdio.h>

#include <string>

#include "Common.h"

#if defined(__GLIBC__)
#include <endian.h>
#endif

#include <array>
#include <cstddef>

const char* format = "<time %d %6d>";  // %i might misparse 0123 as octal.

namespace dex {

namespace internal {
union tsBytes {
    timestamp_t ts;
    std::array<uint8_t, 8> bytes;
};
}  // namespace internal

std::string Timestamp::timestampToString(timestamp_t t)
{
    const int sec = t / 1000000;
    const int usec = t % 1000000;
    char buff[100];
    sprintf(buff, format, sec, usec);
    return std::string(buff);
}

timestamp_t Timestamp::stringToTimestamp(std::string t)
{
    int sec, usec;
    if (sscanf(t.c_str(), format, &sec, &usec) != 2)
        return 0;
    return sec * 1000000 + usec;
}

#if __BYTE_ORDER == __BIG_ENDIAN
#define REORDER(x) (x)
#else
#define REORDER(x) (__builtin_bswap64(x))
#endif

std::array<uint8_t, 8> Timestamp::timestampToBytes(timestamp_t time)
{
    internal::tsBytes u;
    u.ts = REORDER(time);
    return u.bytes;
}

timestamp_t Timestamp::bytesToTimestamp(const uint8_t* buffer)
{
    internal::tsBytes u;
    std::memcpy(u.bytes.data(), buffer, 8);

    return static_cast<timestamp_t>(REORDER(u.ts));
}

}  // namespace dex
