#include "Timestamp.h"
#include <cc++/config.h>
#include <stdio.h>
#include <string>
#include "Common.h"

namespace dex {

std::string Timestamp::timestampToString(timestamp_t t)
{
    const int sec = t / 1000000;
    const int usec = t % 1000000;
    char buff[256];
    sprintf(buff, "<time %i %i>", sec, usec);
    return std::string(buff);
}

timestamp_t Timestamp::stringToTimestamp(std::string t)
{
    int sec, usec;
    if (sscanf(t.c_str(), "<time %i %i>", &sec, &usec) != 2)
        return 0;
    return sec * 1000000 + usec;
}

void Timestamp::timestampToBytes(timestamp_t time, char* buffer)
{
    const char* t = (char*)&time;
    if (__BYTE_ORDER == __BIG_ENDIAN) {
        for (int i = 0; i < 8; ++i) buffer[i] = t[i];
    }
    else {
        for (int i = 0; i < 8; ++i) buffer[i] = t[7 - i];
    }
}

timestamp_t Timestamp::bytesToTimestamp(const char* buffer)
{
    timestamp_t time;
    char* t = (char*)&time;
    if (__BYTE_ORDER == __BIG_ENDIAN) {
        for (int i = 0; i < 8; i++) t[i] = buffer[i];
    }
    else {
        for (int i = 0; i < 8; i++) t[i] = buffer[7 - i];
    }
    return time;
}

}  // namespace dex
