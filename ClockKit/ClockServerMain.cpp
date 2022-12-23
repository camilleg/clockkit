#include <cstdlib>

#include "ClockServer.h"
#include "SystemClock.h"
#ifdef DEBUG
#include "VariableFrequencyClock.h"
#endif

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " [<ipaddr>] <port>\n";
        return 1;
    }
    const auto haveIP = argc == 3;
    const std::string addr(haveIP ? argv[1] : "127.0.0.1");
    const auto port = dex::parseInt(argv[haveIP ? 2 : 1]);

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
