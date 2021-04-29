#pragma once
#include <string>

#include "PhaseLockedClock.h"

namespace dex {
class ConfigReader {
   public:
    // All default values are defined here
    static const std::string defaultServer;
    static const unsigned int defaultPort = 4444;
    static const unsigned int defaultTimeout = 1000;
    static const unsigned int defaultPhasePanic = 5000;
    static const unsigned int defaultUpdatePanic = 5000000;

    // Default ctor initializes default values
    explicit inline ConfigReader()
        : server{std::string{"localhost"}}
        , port{defaultPort}
        , timeout{defaultTimeout}
        , phasePanic{defaultPhasePanic}
        , updatePanic{defaultUpdatePanic}
        , path{""} {};

    ~ConfigReader() = default;

    // Delete copy ctor as clocks with the same config only lead to trouble and
    // are not useful
    ConfigReader(const ConfigReader&) = delete;

    std::string server;
    unsigned int port;
    unsigned int timeout;
    unsigned int phasePanic;
    unsigned int updatePanic;

    // Prints the currently held values to stdout
    void print();

    // Read values from given file, only returns true if the config could be
    // successfully read **and** parsed
    bool readFrom(std::string);

    // Use the values in the reader to get a clock
    PhaseLockedClock* buildClock();

   private:
    std::string path;
};
}  // namespace dex
