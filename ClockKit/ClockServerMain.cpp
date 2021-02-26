#include <cc++/socket.h>

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
    // todo: There's only one thread.  If one thread per client
    // wouldn't be better, why not omit this single thread?
    server.start();
    server.join();
    return 0;
}
