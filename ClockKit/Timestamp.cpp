#include "Timestamp.h"

#include <cstdio>
#include <cstring>

namespace dex {

const char* format = "<time %d %6d>";  // %i might misparse 0123 as octal.

std::string StringFromTp(tp point)
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

tp TpFromString(const std::string& s)
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
    std::byte bytes[8];
};

void BytesFromTp(tp point, std::byte* buffer)
{
    timestampBytes u;
    u.t = REORDER(UsecFromTp(point));
    std::memcpy(buffer, u.bytes, 8);
}

tp TpFromBytes(const std::byte* buffer)
{
    timestampBytes u;
    std::memcpy(u.bytes, buffer, 8);
    return TpFromUsec(REORDER(u.t));
}

}  // namespace dex
