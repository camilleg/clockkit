// Unit tests for only clocks, not clients or servers.

#include <thread>
#include <vector>

#include "PhaseLockedClock.h"
#include "SystemClock.h"
#include "VariableFrequencyClock.h"

using namespace dex;
using namespace std::chrono;
auto& sys = SystemClock::instance();
using vfc = VariableFrequencyClock;
using plc = PhaseLockedClock;
using std::cout, std::cerr;

// Do two clocks based on SystemClock agree?
bool clones_vfc()
{
    vfc c1(sys);
    vfc c2(sys);
    milliseconds runtime(500);
    while (runtime.count() > 0) {
        if (abs(UsecFromDur(diff(c1.getValue(), c2.getValue()))) > 20) {
            cerr << "VFC clones drifted apart.\n";
            return false;
        }
        constexpr auto wait = 10ms;
        std::this_thread::sleep_for(wait);
        runtime -= wait;
    }
    return true;
}

// Do two clocks based on SystemClock agree?
bool clones_plc()
{
    std::vector<std::thread> threads;
    std::atomic_bool end_clocks(false);
    plc c1(sys, sys);
    plc c2(sys, sys);
    threads.emplace_back(&plc::run, &c1, std::ref(end_clocks));
    threads.emplace_back(&plc::run, &c2, std::ref(end_clocks));
    milliseconds runtime(250);
    while (runtime.count() > 0) {
        if (abs(UsecFromDur(diff(c1.getValue(), c2.getValue()))) > 20) {
            cerr << "PLC clones drifted apart.\n";
            return false;
        }
        constexpr auto wait = 10ms;
        std::this_thread::sleep_for(wait);
        runtime -= wait;
    }
    end_clocks = true;
    c1.die();
    c2.die();
    for (auto& thread : threads) thread.join();
    return true;
}

// Does a slow-running vfc run at the right speed?
bool slowclock()
{
    vfc c1(sys);
    vfc c2(sys, 990000);  // Runs 1% slow.
    milliseconds runtime(200);
    dur drift;
    constexpr auto wait = 10ms;
    while (runtime.count() > 0) {
        auto d = diff(c1.getValue(), c2.getValue());
        if (abs(UsecFromDur(d) - UsecFromDur(drift)) > 100) {
            cerr << "slowclock drifted apart.\n";
            return false;
        }
        drift = drift + 100us;  // 0.01 * wait;
        std::this_thread::sleep_for(wait);
        runtime -= wait;
    }
    return true;
}

// Does a fast-running vfc run at the right speed?
bool fastclock()
{
    vfc c1(sys);
    vfc c2(sys, 1050000);  // Runs 5% fast.
    milliseconds runtime(200);
    dur drift;
    constexpr auto wait = 10ms;
    while (runtime.count() > 0) {
        auto d = diff(c1.getValue(), c2.getValue());
        // cout << d << "\t" << drift << "\n";
        if (abs(UsecFromDur(d) - UsecFromDur(drift)) > 300) {
            cerr << "fastclock drifted apart.\n";
            return false;
        }
        drift = drift - 500us;  // -0.05 * wait;
        std::this_thread::sleep_for(wait);
        runtime -= wait;
    }
    return true;
}

// Do vfcs with nondefault speed agree?
bool speedyclocks()
{
    vfc c1(sys, 5000000);
    vfc c2(sys, 5000000);
    milliseconds runtime(200);
    constexpr auto wait = 10ms;
    while (runtime.count() > 0) {
        // cout << c1.getValue() << " " << c2.getValue() << "\n";
        if (abs(UsecFromDur(diff(c1.getValue(), c2.getValue()))) > 20) {
            cerr << "speedyclocks drifted apart.\n";
            return false;
        }
        std::this_thread::sleep_for(wait);
        runtime -= wait;
    }
    return true;
}

int main(int argc, char* argv[])
{
    if (!clones_vfc())
        return 1;
    if (!clones_plc())
        return 1;
    if (!slowclock())
        return 1;
    if (!fastclock())
        return 1;
    if (!speedyclocks())
        return 1;
    return 0;
}
