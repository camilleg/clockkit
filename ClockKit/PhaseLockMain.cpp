#include <atomic>
#include <limits>
#include <thread>

#include "ConfigReader.h"

using namespace std::chrono;
using namespace dex;

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 3) {
        std::cerr << "usage: " << argv[0] << " config_file [duration]\n";
        return 1;
    }

    ConfigReader config;
    if (!config.readFrom(argv[1])) {
        std::cerr << argv[0] << ": failed to parse config file '" << argv[1] << "'.\n";
        return 1;
    }
    config.print();
    auto [plc, cli] = config.buildClock();

    const auto fTerminate = argc == 3;
    microseconds runtime(fTerminate ? int64_t(1000000 * parseFloat(argv[2])) : 0);
    std::atomic_bool end_clocks(fTerminate && runtime.count() <= 0);
    std::thread th_clock(&PhaseLockedClock::run, plc, std::ref(end_clocks));

    while (!end_clocks) {
        const auto offset = UsecFromDur(plc->getOffset());
        std::cout << "offset: " << (offset == usecInvalid ? "invalid" : std::to_string(offset)) << "\n"
                  << StringFromTp(plc->getValue()) << std::endl;
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
    delete cli;
    return 0;
}
