#pragma once
#include <array>
#include <string>

namespace dex {

// Microseconds since the Unix epoch.
using timestamp_t = int64_t;

// Converters.
std::string timestampToString(timestamp_t);
timestamp_t stringToTimestamp(const std::string&);

// Read/write a 64 bit timestamp with proper byte ordering.
std::array<uint8_t, 8> timestampToBytes(timestamp_t);
timestamp_t bytesToTimestamp(const uint8_t*);

}  // namespace dex
