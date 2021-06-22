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
dex::ConfigReader config;  // Contains a ClockClient*, which may not be destructed before plc is.
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

// Prefix externally visible functions with ck, meaning ClockKit.
// This is especially for languages like Tcl, which invoke these functions
// as bare names, not qualified by the name of their containing module.

static void atexit_handler()
{
#ifdef DEBUG
    std::cerr << "clockkit.cpp: atexit.\n";
#endif
    ckTerminate();
}

void ckInitialize(const char* filename)
{
    if (!plc) {
        if (filename)
            config.readFrom(filename);
        plc = config.buildClock();
    }
    test_ok("ckInitialize");
#ifdef DEBUG
    std::cerr << "clockkit.cpp: atexit registered.\n";
#endif
    (void)std::atexit(atexit_handler); // Call ckTerminate even if pkill'ed, i.e., got a SIGTERM.
    th_clock = new std::thread(&dex::PhaseLockedClock::run, plc, std::ref(end_clocks));
}

void ckTerminate()
{
#ifdef DEBUG
    std::cerr << "clockkit.cpp: ckTerminate.\n";
#endif
    if (plc) {
        delete plc;
        plc = nullptr;
    }
}

void ckKill()
{
#ifdef DEBUG
    std::cerr << "clockkit.cpp: ckKill.\n";
#endif
    if (plc)
      plc->die();
}

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
