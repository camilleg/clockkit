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
#include <iostream>

const char* format = "<time %i %i>";

namespace dex {

namespace internal {
union tsBytes {
    timestamp_t ts;
    char bytes[8];
};
}  // namespace internal

std::string Timestamp::timestampToString(timestamp_t t)
{
    const int sec = t / 1000000;
    const int usec = t % 1000000;
    char buff[256];
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

void Timestamp::timestampToBytes(timestamp_t time, char* buffer)
{
    internal::tsBytes u;
#if __BYTE_ORDER != __BIG_ENDIAN
    u.ts = __builtin_bswap64(time);
#else
    u.ts = time;
#endif
    for (size_t i = 0; i < 8; ++i) {
        buffer[i] = u.bytes[i];
    }
}

timestamp_t Timestamp::bytesToTimestamp(const char* buffer)
{
    internal::tsBytes u;
    u.bytes[0] = buffer[0];
    u.bytes[1] = buffer[1];
    u.bytes[2] = buffer[2];
    u.bytes[3] = buffer[3];
    u.bytes[4] = buffer[4];
    u.bytes[5] = buffer[5];
    u.bytes[6] = buffer[6];
    u.bytes[7] = buffer[7];

    return static_cast<timestamp_t>(
#if __BYTE_ORDER != __BIG_ENDIAN
        __builtin_bswap64(
#endif
            u.ts
#if __BYTE_ORDER != __BIG_ENDIAN
            )
#endif
    );
}

}  // namespace dex
