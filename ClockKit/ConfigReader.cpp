#include "ConfigReader.h"

#include <iostream>

#include "HighResolutionClock.h"

namespace dex {

// As a type-safe function, this would be too tedious.
// Also, compared to sscanf, http://en.cppreference.com/w/cpp/regex would be overkill.
// OTOH, scanf %u cannot forbid a minus sign,
// so "port:-10" becomes 2^32-10.  Just don't put negative numbers in the config file!
#define SSCANF(format, name, var, found)                                                            \
    if (sscanf(pch, " " name ":" format, var) == 1) {                                               \
        if (found)                                                                                  \
            std::cerr << "Config file " << filename << ": ignored previous line for '" name "'.\n"; \
        found = true;                                                                               \
        continue;                                                                                   \
    }

bool ConfigReader::readFrom(const char* filename)
{
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to read config file '" << filename << "'\n";
        return false;
    }

    char server[1000] = "localhost";
    auto port = defaultPort;
    auto timeout = defaultTimeout;
    auto phasePanic = defaultPhasePanic;
    auto updatePanic = defaultUpdatePanic;

    auto foundServer = false;
    auto foundPort = false;
    auto foundTimeout = false;
    auto foundPhasepanic = false;
    auto foundUpdatepanic = false;

    std::string line;
    char dummy[1000];
    while (std::getline(file, line)) {
        const char* pch = line.c_str();
        if (sscanf(pch, " %s", dummy) == EOF)  // Blank or empty line.
            continue;
        if (sscanf(pch, " %[#]", dummy) == 1)  // Comment.
            continue;
        SSCANF("%s", "server", server, foundServer);
        SSCANF("%u", "port", &port, foundPort);
        SSCANF("%u", "timeout", &timeout, foundTimeout);
        SSCANF("%u", "phasePanic", &phasePanic, foundPhasepanic);
        SSCANF("%u", "updatePanic", &updatePanic, foundUpdatepanic);
        std::cerr << "Config file " << filename << " has syntax error: " << line << "\n";
        return false;
    }

    // Update, probably from the defaults, whichever values were in the file.
    if (foundServer)
        this->server = server;
    if (foundPort)
        this->port = port;
    if (foundTimeout)
        this->timeout = timeout;
    if (foundPhasepanic)
        this->phasePanic = phasePanic;
    if (foundUpdatepanic)
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
    plc->setPhasePanic(DurFromUsec(this->phasePanic));
    plc->setUpdatePanic(DurFromUsec(this->updatePanic));
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
