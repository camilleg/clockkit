#include "Timestamp.h"

// May include endian.h.  Defines __BYTE_ORDER and __BIG_ENDIAN.
#include <cc++/config.h>
//#if defined(__GLIBC__)
//#include <endian.h>
//#endif

#include <stdio.h>
#include <cstddef>
#include <cstring>

namespace dex {

const char* format = "<time %d %6d>";  // %i might misparse 0123 as octal.

std::string timestampToString(timestamp_t t)
{
    const int sec = t / 1000000;
    const int usec = t % 1000000;
    char buf[100];
    sprintf(buf, format, sec, usec);
    return std::string(buf);
}

timestamp_t stringToTimestamp(const std::string& t)
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

union timestampBytes {
    timestamp_t t;
    std::array<uint8_t, 8> bytes;
};

std::array<uint8_t, 8> timestampToBytes(timestamp_t time)
{
    timestampBytes u;
    u.t = REORDER(time);
    return u.bytes;
}

timestamp_t bytesToTimestamp(const uint8_t* buffer)
{
    timestampBytes u;
    std::memcpy(u.bytes.data(), buffer, 8);

    return static_cast<timestamp_t>(REORDER(u.t));
}

}  // namespace dex
