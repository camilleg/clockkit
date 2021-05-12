#include "ConfigReader.h"

#include <iostream>

#include "HighResolutionClock.h"

namespace dex {

bool ConfigReader::readFrom(const char* filename)
{
    std::ifstream file(filename);
    if (!file.good())
        return false;

    // Values only initialized to have a sane empty state, these should never
    // make it into the object
    // XXX maybe use asserts here
    std::string server;
    auto port = 0u;
    auto timeout = 0u;
    auto phasePanic = 0u;
    auto updatePanic = 0u;
    bool found = false;

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
        return false;

    // Overwrite values only if the full config parsed ok.
    this->server = server;
    this->port = port;
    this->timeout = timeout;
    this->phasePanic = phasePanic;
    this->updatePanic = updatePanic;
    return true;
}

PhaseLockedClock* ConfigReader::buildClock()
{
    const ost::InetHostAddress addr(this->server.c_str());
    client_ = new ClockClient(addr, this->port);
    client_->setTimeout(this->timeout);
    client_->setAcknowledge(true);
    PhaseLockedClock* plc = new PhaseLockedClock(HighResolutionClock::instance(), *client_);
    plc->setPhasePanic(this->phasePanic);
    plc->setUpdatePanic(this->updatePanic);
    return plc;
}

void ConfigReader::print()
{
    std::cout << "config [server:" << server << "]\n"
              << "config [port:" << port << "]\n"
              << "config [timeout:" << timeout << "]\n"
              << "config [phasePanic:" << phasePanic << "]\n"
              << "config [updatePanic:" << updatePanic << "]" << std::endl;
}

}  // namespace dex
