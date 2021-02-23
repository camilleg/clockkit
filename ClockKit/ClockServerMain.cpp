#include <cc++/socket.h>

#include <cstdlib>

#include "ClockServer.h"
#include "HighResolutionClock.h"
#include "VariableFrequencyClock.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <port>\n";
        return 1;
    }

    const ost::InetAddress addr("0.0.0.0");
    const int port = atoi(argv[1]);

#ifdef DEBUG
    // A slow clock, for testing.
    dex::VariableFrequencyClock clock(dex::HighResolutionClock::instance());
    clock.setFrequency(1000000 - 2000);
#else
    auto& clock(dex::HighResolutionClock::instance());
#endif
    dex::ClockServer server(addr, port, clock);

    server.setLogging(true);
    server.start();
    server.join();
    std::cerr << "Exited cleanly!\n";
    ;
    ;
    ;
    return 0;
}
