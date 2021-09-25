#pragma once
#include <string>

#include "ClockClient.h"
#include "PhaseLockedClock.h"

namespace dex {
class ConfigReader {
   public:
    static const auto defaultPort = 4444u;
    static const auto defaultTimeout = 1000u;
    static const auto defaultPhasePanic = 5000u;
    static const auto defaultUpdatePanic = 5000000u;

    explicit inline ConfigReader()
        : server{"localhost"}
        , port{defaultPort}
        , timeout{defaultTimeout}
        , phasePanic{defaultPhasePanic}
        , updatePanic{defaultUpdatePanic}
        , client_(nullptr){};

    // Don't let this be called before destructing the PhaseLockedClock that uses client_.
    ~ConfigReader()
    {
        delete client_;
    }

    // Another clock with the same config would be dangerous, not useful.
    ConfigReader(const ConfigReader&) = delete;
    ConfigReader& operator=(const ConfigReader&) = delete;

    std::string server;
    unsigned port;
    unsigned timeout;
    unsigned phasePanic;
    unsigned updatePanic;

    // Print the current values to stdout.
    void print();

    // Read values from a config file.
    // Returns true iff the config was read *and* parsed.
    bool readFrom(const char*);

    // Make a clock from the reader's values.
    // The caller owns the returned pointer (which cannot be nullptr),
    // and is responsible for deleting it.
    PhaseLockedClock* buildClock();

   private:
    ClockClient* client_;
};
}  // namespace dex
