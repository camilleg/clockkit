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
using std::cerr;

// A countdown timer.
class Countdown {
    const dur step_;
    dur runtime_;

   public:
    Countdown(dur step, dur runtime)
        : step_(step)
        , runtime_(runtime)
    {
    }
    // Wait, then return true iff the countdown is still running.
    operator bool()
    {
        std::this_thread::sleep_for(step_);
        runtime_ -= step_;
        return runtime_.count() > 0;
    }
};

// Do two clocks based on SystemClock agree?
bool clones_vfc()
{
    vfc c1(sys);
    vfc c2(sys);
    Countdown countdown(10ms, 500ms);
    while (countdown) {
        if (abs(UsecFromDur(diff(c1.getValue(), c2.getValue()))) > 20) {
            cerr << "VFC clones drifted apart.\n";
            return false;
        }
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
    Countdown countdown(10ms, 250ms);
    while (countdown) {
        if (abs(UsecFromDur(diff(c1.getValue(), c2.getValue()))) > 20) {
            cerr << "PLC clones drifted apart.\n";
            return false;
        }
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
    dur drift;
    Countdown countdown(10ms, 250ms);
    while (countdown) {
        drift = drift + 100us;  // 1% of 10ms
        auto d = diff(c1.getValue(), c2.getValue());
        if (abs(UsecFromDur(d) - UsecFromDur(drift)) > 100) {
            cerr << "slowclock drifted apart.\n";
            return false;
        }
    }
    return true;
}

// Does a fast-running vfc run at the right speed?
bool fastclock()
{
    vfc c1(sys);
    vfc c2(sys, 1050000);  // Runs 5% fast.
    dur drift;
    Countdown countdown(8ms, 200ms);
    while (countdown) {
        drift = drift - 400us;  // -5% of 8ms
        auto d = diff(c1.getValue(), c2.getValue());
        // cerr << d << "\t" << drift << "\n";
        if (abs(UsecFromDur(d) - UsecFromDur(drift)) > 300) {
            cerr << "fastclock drifted apart.\n";
            return false;
        }
    }
    return true;
}

// Do vfcs with nondefault speed agree?
bool speedyclocks()
{
    vfc c1(sys, 5000000);
    vfc c2(sys, 5000000);
    Countdown countdown(10ms, 200ms);
    while (countdown) {
        const auto t1 = c1.getValue();
        const auto t2 = c2.getValue();
        // cerr << t1 << " " << t2 << "\n";
        if (abs(UsecFromDur(diff(t1, t2))) > 10) {
            cerr << "speedyclocks drifted apart.\n";
            return false;
        }
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
