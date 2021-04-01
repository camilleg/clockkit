#pragma once
#include <array>
#include <string>

namespace dex {

// Microseconds since the Unix epoch.
using timestamp_t = int64_t;

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
    static std::array<uint8_t, 8> timestampToBytes(timestamp_t time);

    // Reads a 64 bit timestamp with proper byte ordering.
    static timestamp_t bytesToTimestamp(const uint8_t* buffer);
};

}  // namespace dex
