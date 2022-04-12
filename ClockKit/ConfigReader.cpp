#include "ConfigReader.h"

#include <fstream>

#include "SystemClock.h"

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

    auto foundServer = false;
    auto foundPort = false;
    auto foundTimeout = false;
    auto foundPhasepanic = false;
    auto foundUpdatepanic = false;

    std::string line;
    char dummy[1000];
    while (std::getline(file, line)) {
        const char* pch = line.c_str();
        if (sscanf(pch, " %990s", dummy) == EOF)  // Blank or empty.
            continue;
        if (sscanf(pch, " %990[#]", dummy) == 1)  // Comment.
            continue;
        SSCANF("%990s", "server", server, foundServer);
        SSCANF("%990u", "port", &port, foundPort);
        SSCANF("%990u", "timeout", &timeout, foundTimeout);
        SSCANF("%990u", "phasePanic", &phasePanic, foundPhasepanic);
        SSCANF("%990u", "updatePanic", &updatePanic, foundUpdatepanic);
        std::cerr << "Syntax error in config file " << filename << ": " << line << "\n";
        return false;
    }
    return true;
}

std::pair<PhaseLockedClock*, ClockClient*> ConfigReader::buildClock()
{
    auto cli = new ClockClient(kissnet::endpoint(server, port));
    cli->setTimeout(timeout);
    cli->setAcknowledge(true);
    auto plc = new PhaseLockedClock(SystemClock::instance(), *cli);
    plc->setPhasePanic(DurFromUsec(phasePanic));
    plc->setUpdatePanic(DurFromUsec(updatePanic));
    return {plc, cli};
}

void ConfigReader::print()
{
    std::cout << "config:\n  server: " << server << "\n  port: " << port << "\n  timeout: " << timeout
              << " μs\n  phasePanic: " << phasePanic << " μs\n  updatePanic: " << updatePanic << " μs" << std::endl;
}

}  // namespace dex
