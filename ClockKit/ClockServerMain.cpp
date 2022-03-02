#include <cstdlib>

#include "ClockServer.h"
#include "SystemClock.h"
#ifdef DEBUG
#include "VariableFrequencyClock.h"
#endif

int main(int argc, char* argv[])
{
    auto addr = std::string("127.0.0.1");
    auto port = 4444;

    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " [<ipaddr>] <port>\n";
        return 1;
    }
    else if (argc == 3) {
        addr = std::string(argv[1]);
        port = dex::parseInt(argv[2]);
    }
    else {
        port = dex::parseInt(argv[1]);
    }

#ifdef DEBUG
    // A slow clock, for testing.
    dex::VariableFrequencyClock clock(dex::SystemClock::instance());
    clock.setFrequency(1000000.0 - 2000.0);
#else
    auto& clock(dex::SystemClock::instance());
#endif
    dex::ClockServer server(kissnet::endpoint(addr, port), clock);
    server.setLogging(true);
    server.run();
    return 0;
}
