#pragma once

#define KISSNET_NO_EXCEP
#include "kissnet.hpp"

#include "Timestamp.h"

#include <iostream>

namespace dex {

// Abstract base class.
// Clocks have frequency 1000000 Hz, except for VariableFrequencyClock.
class Clock {
   public:
    // This clock's value, when this returns (not when it was called).
    // The lag between those two may be large for ClockClient.
    // Not const because VFC's sets rolledOver_, and because
    // ClockClient's calls its getPhase which sets sequence_.
    virtual tp getValue() = 0;

    // The phase, or offset, between this (primary) clock and another clock.
    virtual dur getPhase(Clock& c)
    {
        return diff(getValue(), c.getValue());
    }

    virtual ~Clock() = default;

    // Cleanly kill a ClockServer or ClockClient.
    virtual void die(){};
};

// For parsing numbers from argv[] at the start of main(),
// more robustly than atoi() and atof().
// On error, simply exit(1).
inline int parseInt(const char* s)
{
    const auto saved = errno;
    errno = 0;
    char* tmp;
    const auto val = strtol(s, &tmp, 0);
    if (tmp == s || *tmp != '\0' || errno == ERANGE || errno == EINVAL) {
        std::cerr << "Failed to parse int from '" << s << "'.\n";
        exit(1);
    }
    if (errno == 0)
        errno = saved;
    return int(val);
}

inline double parseFloat(const char* s)
{
    const auto saved = errno;
    errno = 0;
    char* tmp;
    const auto val = strtod(s, &tmp);
    if (tmp == s || *tmp != '\0' || errno == ERANGE || errno == EINVAL) {
        std::cerr << "Failed to parse float from '" << s << "'.\n";
        exit(1);
    }
    if (errno == 0)
        errno = saved;
    return val;
}

// Handle some errors specially, instead of just aborting.
static inline void kissnet_error(const std::string& s, void*)
{
    if (s == "bind() failed\n")
        std::cerr << "bind() failed: bad port number, or another ckserver might be running on that port.\n";
    else
        std::cerr << s << "\n";
    exit(1);
}
static inline void kissnet_init()
{
    kissnet::error::abortOnFatalError = false;
    kissnet::error::callback = &kissnet_error;
}

}  // namespace dex
