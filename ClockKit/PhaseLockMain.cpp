#include <atomic>
#include <iostream>
#include <limits>
#include <thread>

#include "ConfigReader.h"

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 3) {
        std::cerr << "usage: " << argv[0] << " config_file [duration]\n";
        return 1;
    }

    dex::PhaseLockedClock* clock = dex::PhaseLockedClockFromConfigFile(argv[1]);
    if (!clock) {
        std::cerr << argv[0] << ": failed to get a clock.\n";
        return 1;
    }
    std::atomic_bool end_clocks(false);
    std::thread th_clock(&dex::PhaseLockedClock::run, clock, std::ref(end_clocks));

    const auto fTerminate = argc == 3;
    auto runtime = fTerminate ? atof(argv[2]) : 0.0;
    if (fTerminate && runtime <= 0.0) {
        end_clocks = true;
    }
    while (!end_clocks) {
        static const auto invalid = std::numeric_limits<int>::max();
        const auto offset = clock->getOffset();
        std::cout << "offset: " << (offset == invalid ? "invalid" : std::to_string(offset)) << "\n"
                  << dex::timestampToString(clock->getValue()) << std::endl;
        // endl flushes stdout, to show output even after Ctrl+C.
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if (fTerminate) {
            runtime -= 0.2;  // sec
            if (runtime <= 0.0) {
                end_clocks = true;
            }
        }
    }
    clock->die();
    th_clock.join();
    return 0;
}
