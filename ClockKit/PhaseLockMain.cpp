#include <atomic>
#include <iostream>
#include <limits>
#include <thread>

#include "ConfigReader.h"

using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 3) {
        std::cerr << "usage: " << argv[0] << " config_file [duration]\n";
        return 1;
    }

    dex::ConfigReader config;
    if (!config.readFrom(argv[1])) {
        std::cerr << argv[0] << ": failed to parse config file '" << argv[1] << "'.\n";
        return 1;
    }
    config.print();

    dex::PhaseLockedClock* plc = config.buildClock();
    if (!plc) {
        std::cerr << argv[0] << ": failed to get a clock.\n";
        return 1;
    }

    const auto fTerminate = argc == 3;
    std::chrono::microseconds runtime(fTerminate ? int64_t(1000000 * atof(argv[2])) : 0);
    std::atomic_bool end_clocks(fTerminate && runtime.count() <= 0);
    std::thread th_clock(&dex::PhaseLockedClock::run, plc, std::ref(end_clocks));

    while (!end_clocks) {
        static const auto invalid = std::numeric_limits<int>::max();
        const auto offset = plc->getOffset();  // In microseconds.
        std::cout << "offset: " << (offset == invalid ? "invalid" : std::to_string(offset)) << "\n"
                  << dex::timestampToString(plc->getValue()) << std::endl;
        // endl flushes stdout, to show output even after Ctrl+C.
        std::this_thread::sleep_for(200ms);
        if (fTerminate) {
            runtime -= 200ms;
            if (runtime.count() <= 0) {
                end_clocks = true;
            }
        }
    }

    plc->die();
    th_clock.join();
    delete plc;
    return 0;
}
