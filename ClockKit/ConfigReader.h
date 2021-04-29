#pragma once
#include <string>

#include "PhaseLockedClock.h"

namespace dex {
class ConfigReader {
   public:
    // Defaults.
    static const std::string defaultServer;
    static const auto defaultPort = 4444u;
    static const auto defaultTimeout = 1000u;
    static const auto defaultPhasePanic = 5000u;
    static const auto defaultUpdatePanic = 5000000u;

    // This default ctor initializes the default values.
    explicit inline ConfigReader()
        : server{"localhost"}
        , port{defaultPort}
        , timeout{defaultTimeout}
        , phasePanic{defaultPhasePanic}
        , updatePanic{defaultUpdatePanic} {};

    ~ConfigReader() = default;

    // No copy ctor, because clocks with duplicate configs are dangerous, not useful.
    ConfigReader(const ConfigReader&) = delete;

    std::string server;
    unsigned int port;
    unsigned int timeout;
    unsigned int phasePanic;
    unsigned int updatePanic;

    // Print the current values to stdout.
    void print();

    // Read values from a config file.
    // Returns true iff the config was read *and* parsed.
    bool readFrom(const char*);

    // Make a clock from the reader's values.
    PhaseLockedClock* buildClock();
};
}  // namespace dex
