#include "clockkit.h"

#include <atomic>
#include <limits>
#include <thread>

#include "ClockClient.h"
#include "ConfigReader.h"
#include "HighResolutionClock.h"

// Hide locals in this anonymous namespace, not by declaring static.
// To verify: readelf -sW clockkit.o | c++filt -t | grep -v UND
namespace {
dex::PhaseLockedClock* plc = nullptr;
std::thread* th_clock = nullptr;
std::atomic_bool end_clocks(false);  // .load() and .store() are implicit.
std::string strTime;                 // Static storage for the pointer returned by ckTimeAsString().
void test_ok(const char* func)
{
// #define DEBUG
#ifdef DEBUG
    if (!plc)
        std::cerr << "clockkit.cpp: no clock in " << func << ".\n";
#endif
}
}  // namespace

// Prefix externally visible functions with ck.

void ckInitialize(const char* filename)
{
    if (!plc) {
        dex::ConfigReader config;
        if (filename)
            config.readFrom(filename);
        plc = config.buildClock();
    }
    test_ok("ckInitialize");
    th_clock = new std::thread(&dex::PhaseLockedClock::run, plc, std::ref(end_clocks));
}

// Add void ckTerminate() to kill the thread cleanly, so */testcase.sh needn't pkill?

dex::timestamp_t ckTimeAsValue()
{
    test_ok("ckTimeAsValue");
    return plc ? plc->getValue() : 0;
    // "Zero usec since the epoch" is obviously invalid.
}

const char* ckTimeAsString()
{
    test_ok("ckTimeAsString");
    if (!plc)
        return "";
    strTime = dex::timestampToString(plc->getValue());
    return strTime.c_str();
}

bool ckInSync()
{
    test_ok("ckInSync");
    return plc && plc->isSynchronized();
}

int ckOffset()
{
    test_ok("ckOffset");
    // Typically 2147483647 usec, or 35 minutes, obviously invalid.
    static const int invalid = std::numeric_limits<int>::max();
    return plc ? plc->getOffset() : invalid;
}
