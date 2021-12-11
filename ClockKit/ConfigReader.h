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

    explicit ConfigReader()
        : server{"127.0.0.1"}
        , port{defaultPort}
        , timeout{defaultTimeout}
        , phasePanic{defaultPhasePanic}
        , updatePanic{defaultUpdatePanic} {};

    ~ConfigReader()
    {
    }

    // Another clock with the same config would be dangerous, not useful.
    ConfigReader(const ConfigReader&) = delete;
    ConfigReader& operator=(const ConfigReader&) = delete;

    char server[1000];
    unsigned port;
    unsigned timeout;      // usec
    unsigned phasePanic;   // usec
    unsigned updatePanic;  // usec

    // Print the current values to stdout.
    void print();

    // Read values from a config file.
    // Returns true iff the config was read *and* parsed.
    bool readFrom(const char*);

    // Make a clock from the reader's values.
    // The caller owns both returned pointers (which cannot be null),
    // and is responsible for deleting them, first plc, then cli.
    //
    // If we'll never need a cli (a reference clock) with multiple plc's
    // (see test-standalone.cpp), then hide cli inside plc as a unique_ptr.
    std::pair<PhaseLockedClock*, ClockClient*> buildClock();
};
}  // namespace dex
