#include <cstdlib>

#include "ClockServer.h"
#include "SystemClock.h"
#ifdef DEBUG
#include "VariableFrequencyClock.h"
#endif

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <port>\n";
        return 1;
    }
    const auto port = atoi(argv[1]);

#ifdef DEBUG
    // A slow clock, for testing.
    dex::VariableFrequencyClock clock(dex::SystemClock::instance());
    clock.setFrequency(1000000.0 - 2000.0);
#else
    auto& clock(dex::SystemClock::instance());
#endif
    dex::ClockServer server(kissnet::endpoint("0.0.0.0", port), clock);
    server.setLogging(true);
    server.run();
    return 0;
}
