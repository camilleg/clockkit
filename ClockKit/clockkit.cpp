#include "clockkit.h"

#include "ClockClient.h"
#include "ConfigReader.h"
#include "HighResolutionClock.h"
#include "limits"

dex::PhaseLockedClock* ckClock = nullptr;
std::string ckTimeString;  // Static storage for the pointer returned by ckTimeAsString().

void ckInitialize(const char* path)
{
    if (!ckClock)
        ckClock = dex::PhaseLockedClockFromConfigFile(std::string(path));
}

dex::timestamp_t ckTimeAsValue()
{
    return ckClock ? ckClock->getValue() : 0;
    // "Zero usec since the epoch" is obviously invalid.
}

const char* ckTimeAsString()
{
    if (!ckClock)
        return "";
    ckTimeString = dex::timestampToString(ckClock->getValue());
    return ckTimeString.c_str();
}

bool ckInSync()
{
    return ckClock && ckClock->isSynchronized();
}

int ckOffset()
{
    // Typically 2147483647 usec, or 35 minutes, obviously invalid.
    static const int invalid = std::numeric_limits<int>::max();
    return ckClock ? ckClock->getOffset() : invalid;
}
