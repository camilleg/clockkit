// Only for testing.
// A self-contained program with threads for a server and some clients.
// An example that does not read any config file.

#include <thread>
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
    std::thread th_server(&ClockServer::run, &server);

#if 0
    // For just one cli, pointers aren't needed.
    ClockClient client(ost::InetHostAddress("127.0.0.1"), port);
    client.setTimeout(1000);
    client.setAcknowledge(true);
    PhaseLockedClock plc(clockHiRes, client);
    plc.setPhasePanic(5000);
    plc.setUpdatePanic(5000000);
#endif

    std::atomic_bool end_clocks(false);
    std::vector<ClockClient*> clients;
    std::vector<PhaseLockedClock*> clocks;
    std::vector<std::thread> threads;
    for (auto i = 0; i < numClients; ++i) {
        auto cli = new ClockClient(ost::InetHostAddress("127.0.0.1"), port);
        // Don't bother to clients.emplace_back(), because ClockClient's
        // private copy constructor causes baroque workarounds,
        // https://stackoverflow.com/q/17007977/2097284, or maybe std::move.
        clients.push_back(cli);
        auto plc = new PhaseLockedClock(clockHiRes, *cli);
        plc->setPhasePanic(5000);
        plc->setUpdatePanic(5000000);
        clocks.push_back(plc);
        threads.emplace_back(&PhaseLockedClock::run, plc, std::ref(end_clocks));
    }

    while (runtime > 0.0) {
        for (const auto plc : clocks)
            std::cout << "offset: " << plc->getOffset() << "\n"
                      << timestampToString(plc->getValue()) << std::endl;
        std::cout << std::endl;
        const auto msec = 600;
        std::this_thread::sleep_for(std::chrono::milliseconds(msec));
        runtime -= msec * 0.001;
    }

    for (const auto plc : clocks) {
        plc->die();
        delete plc;
    }
    end_clocks = true;
    for (auto& thread : threads) thread.join();
    for (const auto client : clients) delete client;

    th_server.join();
    return 0;
}
