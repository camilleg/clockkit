// Only for testing.
// A self-contained program with threads for a server and some clients.
// An example that does not read any config file.

#include <vector>

#include "ClockClient.h"
#include "ClockServer.h"
#include "HighResolutionClock.h"
#include "PhaseLockedClock.h"

using namespace dex;

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "usage: " << argv[0] << " port num_clients duration\n";
        return 1;
    }
    const auto port = atoi(argv[1]);
    const auto numClients = atoi(argv[2]);
    auto runtime = atof(argv[3]);

    auto& clockHiRes = HighResolutionClock::instance();
    ClockServer server(ost::InetAddress("0.0.0.0"), port, clockHiRes);
    server.setLogging(true);
    server.start();

#if 0
    // For just one cli, pointers aren't needed.
    ClockClient client(ost::InetHostAddress("127.0.0.1"), port);
    client.setTimeout(1000);
    client.setAcknowledge(true);
    PhaseLockedClock clock(clockHiRes, client);
    clock.setPhasePanic(5000);
    clock.setUpdatePanic(5000000);
#endif

    std::vector<ClockClient*> clients;
    std::vector<PhaseLockedClock*> clocks;
    for (auto i = 0; i < numClients; ++i) {
        auto cli = new ClockClient(ost::InetHostAddress("127.0.0.1"), port);
        clients.push_back(cli);
        auto clock = new PhaseLockedClock(clockHiRes, *cli);
        clock->setPhasePanic(5000);
        clock->setUpdatePanic(5000000);
        clocks.push_back(clock);
    }

    while (runtime > 0.0) {
        for (const auto clock : clocks)
            std::cout << "offset: " << clock->getOffset() << "\n"
                      << timestampToString(clock->getValue()) << std::endl;
        std::cout << std::endl;
        const auto msec = 600;
        ost::Thread::sleep(msec);
        runtime -= msec * 0.001;
    }
    for (const auto clock : clocks) clock->die();
    server.join();
    return 0;
}
