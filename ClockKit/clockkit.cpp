#include "clockkit.h"

#include <atomic>
#include <limits>
#include <thread>

#include "ClockClient.h"
#include "ConfigReader.h"

using namespace dex;

// Hide locals in this anonymous namespace, not by declaring static.
// To verify: readelf -sW clockkit.o | c++filt -t | grep -v UND
namespace {
PhaseLockedClock* plc = nullptr;
ClockClient* cli = nullptr;
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

void atexit_handler()
{
#ifdef DEBUG
    std::cerr << "clockkit.cpp: atexit.\n";
#endif
    ckTerminate();
}
}  // namespace

// Prefix externally visible functions with ck, meaning ClockKit.
// This is especially for languages like Tcl, which invoke these functions
// as bare names, not qualified by the name of their containing module.

void ckInitialize(const char* filename)
{
    if (!plc) {
        ConfigReader config;
        if (filename)
            if (!config.readFrom(filename)) {
                std::cerr << "clockkit.cpp: failed to parse config file '" << filename << "'.\n";
                return;
            }
        auto plc_and_cli = config.buildClock();
        plc = plc_and_cli.first;
        cli = plc_and_cli.second;
    }
    test_ok("ckInitialize");
#ifdef DEBUG
    std::cerr << "clockkit.cpp: atexit registered.\n";
#endif
    (void)std::atexit(atexit_handler);  // Call ckTerminate even if pkill'ed, i.e., got a SIGTERM.
    th_clock = new std::thread(&PhaseLockedClock::run, plc, std::ref(end_clocks));
}

void ckTerminate()
{
#ifdef DEBUG
    std::cerr << "clockkit.cpp: ckTerminate.\n";
#endif
    delete plc;
    plc = nullptr;
    delete cli;
    cli = nullptr;
}

void ckKill()
{
#ifdef DEBUG
    std::cerr << "clockkit.cpp: ckKill.\n";
#endif
    if (plc)
        plc->die();
}

// Usec since the Unix epoch.
int64_t ckTimeAsValue()
{
    test_ok("ckTimeAsValue");
    return plc ? UsecFromTp(plc->getValue()) : usecInvalid;
}

const char* ckTimeAsString()
{
    test_ok("ckTimeAsString");
    if (!plc)
        return "";
    strTime = StringFromTp(plc->getValue());
    return strTime.c_str();
}

bool ckInSync()
{
    test_ok("ckInSync");
    return plc && plc->isSynchronized();
}

int64_t ckOffset()
{
    test_ok("ckOffset");
    return plc ? UsecFromDur(plc->getOffset()) : usecInvalid;
}
