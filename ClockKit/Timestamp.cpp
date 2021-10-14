#include "Timestamp.h"

// May include endian.h.  Defines __BYTE_ORDER and __BIG_ENDIAN.
#include <cc++/config.h>
//#if defined(__GLIBC__)
//#include <endian.h>
//#endif

#include <cstddef>
#include <cstdio>
#include <cstring>

namespace dex {

const char* format = "<time %d %6d>";  // %i might misparse 0123 as octal.

std::string timestampToString(tp point)
{
    // Specially treating point==tpInvalid would be a breaking change.
    // Convert it conventionally, like everything else in Timestamp.h.
    const auto t = UsecFromTp(point);
    const int sec = t / 1000000;
    const int usec = t % 1000000;
    char buf[10 + (22 - 6) + 7];  // <time  >, sec, usec
    sprintf(buf, format, sec, usec);
    return std::string(buf);
}

tp stringToTimestamp(const std::string& s)
{
    int sec, usec;
    if (sscanf(s.c_str(), format, &sec, &usec) != 2)
        return tpInvalid;
    return TpFromUsec(sec * 1000000 + usec);
}

#if __BYTE_ORDER == __BIG_ENDIAN
#define REORDER(x) (x)
#else
#define REORDER(x) (__builtin_bswap64(x))
#endif

union timestampBytes {
    int64_t t;
    std::array<std::byte, 8> bytes;
};

std::array<std::byte, 8> timestampToBytes(tp point)
{
    timestampBytes u;
    u.t = REORDER(UsecFromTp(point));
    return u.bytes;
}

tp bytesToTimestamp(const std::byte* buffer)
{
    timestampBytes u;
    std::memcpy(u.bytes.data(), buffer, 8);
    return TpFromUsec(REORDER(u.t));
}

}  // namespace dex
