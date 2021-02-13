#include "Timestamp.h"
#include <cc++/config.h>
#include <stdio.h>
#include <string>
#include "Common.h"

const char* format = "<time %i %i>";

namespace dex {

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

// C++20 will have std::endian.
#if __BYTE_ORDER == __BIG_ENDIAN
#define src(i) (i)
#else
#define src(i) (7 - (i))
#endif

void Timestamp::timestampToBytes(timestamp_t time, char* buffer)
{
    const char* t = (const char*)&time;
    for (int i = 0; i < 8; ++i) buffer[i] = t[src(i)];
}

timestamp_t Timestamp::bytesToTimestamp(const char* buffer)
{
    timestamp_t time;
    char* t = (char*)&time;
    for (int i = 0; i < 8; ++i) t[i] = buffer[src(i)];
    return time;
}

}  // namespace dex
