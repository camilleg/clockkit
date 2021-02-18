#pragma once
#include <string>

namespace dex {

// Microseconds since the Unix epoch.
#ifdef WIN32
typedef __int64 timestamp_t;
#else
typedef long long timestamp_t;
#endif

class Timestamp {
   public:
    // Conversion functions only.  No members or methods or instances.
    Timestamp() = delete;
    Timestamp(Timestamp& t) = delete;
    ~Timestamp() = delete;

    // Converts a timestamp to a string.
    static std::string timestampToString(timestamp_t t);

    // Converts a string to a timestamp.
    static timestamp_t stringToTimestamp(std::string t);

    // Writes a 64 bit timestamp with proper byte ordering.
    static void timestampToBytes(timestamp_t time, char* buffer);

    // Reads a 64 bit timestamp with proper byte ordering.
    static timestamp_t bytesToTimestamp(const char* buffer);
};

}  // namespace dex
