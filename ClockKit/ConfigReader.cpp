#include "ConfigReader.h"

#include <iostream>

#include "ClockClient.h"
#include "HighResolutionClock.h"

namespace dex {

PhaseLockedClock* PhaseLockedClockFromConfigFile(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "failed to open config file '" << filename << "'\n";
        return nullptr;
    }

    std::string server = "localhost";
    auto port = 4444;
    auto timeout = 1000;
    auto phasePanic = 5000;
    auto updatePanic = 5000000;
    auto found = false;

    while (!file.eof()) {
        std::string line;
        file >> line;
        const auto pos = line.find(":");
        if (pos < 0)
            break;
        const auto name(line.substr(0, pos));
        const auto value(line.substr(pos + 1));

        if (name == "server") {
            server = value;
            found = true;
        }
        else if (name == "port") {
            port = atoi(value.c_str());
            found = true;
        }
        else if (name == "timeout") {
            timeout = atoi(value.c_str());
            found = true;
        }
        else if (name == "phasePanic") {
            phasePanic = atoi(value.c_str());
            found = true;
        }
        else if (name == "updatePanic") {
            updatePanic = atoi(value.c_str());
            found = true;
        }
    }
    file.close();
    if (!found)
        std::cerr << "using defaults because of useless config file '" << filename << "'\n";

    // TODO separate printing from object creation
    std::cout << "config [server:" << server << "]\n"
              << "config [port:" << port << "]\n"
              << "config [timeout:" << timeout << "]\n"
              << "config [phasePanic:" << phasePanic << "]\n"
              << "config [updatePanic:" << updatePanic << "]" << std::endl;

    const ost::InetHostAddress addr(server.c_str());
    ClockClient* client = new ClockClient(addr, port);
    client->setTimeout(timeout);
    client->setAcknowledge(true);
    PhaseLockedClock* plc = new PhaseLockedClock(HighResolutionClock::instance(), *client);
    plc->setPhasePanic(phasePanic);
    plc->setUpdatePanic(updatePanic);
    return plc;
}

}  // namespace dex
