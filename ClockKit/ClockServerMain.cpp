#include <cc++/socket.h>  // Only for ost::InetAddress.
#include <cstdlib>

#include "ClockServer.h"
#include "HighResolutionClock.h"
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
    dex::VariableFrequencyClock clock(dex::HighResolutionClock::instance());
    clock.setFrequency(1000000 - 2000);
#else
    auto& clock(dex::HighResolutionClock::instance());
#endif
    dex::ClockServer server(ost::InetAddress("0.0.0.0"), port, clock);
    server.setLogging(true);
    server.run();
    return 0;
}
