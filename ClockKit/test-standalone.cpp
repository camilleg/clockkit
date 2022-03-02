// Only for testing.
// A self-contained program with threads for a server and some clients.
// An example that does not read any config file.

#include <thread>
#include <vector>

#include "ClockClient.h"
#include "ClockServer.h"
#include "PhaseLockedClock.h"
#include "SystemClock.h"

using namespace dex;
using namespace std::chrono;

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "usage: " << argv[0] << " port num_clients duration\n";
        return 1;
    }
    const auto port = parseInt(argv[1]);
    const auto numClients = parseInt(argv[2]);
    if (numClients < 1) {
        std::cerr << argv[0] << ": at least 1 client.\n";
        return 1;
    }
    microseconds runtime(int64_t(1000000 * parseFloat(argv[3])));

    auto& clockSystem = SystemClock::instance();
    ClockServer server(kissnet::endpoint("127.0.0.1", port), clockSystem);
    server.setLogging(true);
    std::thread th_server(&ClockServer::run, &server);

#if 0
    // For just one cli, pointers aren't needed.
    ClockClient client(kissnet::endpoint("127.0.0.1", port));
    client.setTimeout(1000);
    client.setAcknowledge(true);
    PhaseLockedClock plc(clockSystem, client);
    plc.setPhasePanic(5ms);
    plc.setUpdatePanic(5s);
#endif

    std::atomic_bool end_clocks(false);
    std::vector<ClockClient*> clients;
    std::vector<PhaseLockedClock*> clocks;
    std::vector<std::thread> threads;
    for (auto i = 0; i < numClients; ++i) {
        auto cli = new ClockClient(kissnet::endpoint("127.0.0.1", port));
        // Don't bother to clients.emplace_back(), because ClockClient's
        // private copy constructor causes baroque workarounds,
        // https://stackoverflow.com/q/17007977/2097284, or maybe std::move.
        clients.push_back(cli);
        auto plc = new PhaseLockedClock(clockSystem, *cli);
        plc->setPhasePanic(5ms);
        plc->setUpdatePanic(5s);
        clocks.push_back(plc);
        threads.emplace_back(&PhaseLockedClock::run, plc, std::ref(end_clocks));
    }

    while (runtime.count() > 0) {
        for (const auto plc : clocks)
            std::cout << "offset: " << UsecFromDur(plc->getOffset()) << "\n"
                      << StringFromTp(plc->getValue()) << std::endl;
        std::cout << std::endl;
        constexpr auto wait = 600ms;
        std::this_thread::sleep_for(wait);
        runtime -= wait;
    }

    end_clocks = true;  // Tell each plc::run() to quit, in about 200ms.
    for (const auto plc : clocks) plc->die();
    for (auto& thread : threads) thread.join();
    for (const auto plc : clocks) delete plc;
    for (const auto cli : clients) delete cli;
    th_server.join();
    return 0;
}
