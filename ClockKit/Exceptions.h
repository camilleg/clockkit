#pragma once
#include <string>

namespace dex {

/**
 * The base class for exceptions in the DEX package.
 * Each exception has a simple string message.
 */
class Exception {
   protected:
     std::string message_;

   public:
    Exception(std::string message);
    virtual ~Exception();
    virtual std::string getMessage();
};

/**
 * An Exception specific to the ClockKit classes in DEX.
 * Clocks may fail for all sorts of reasons.  A remote
 * clock may not be able to contact a server, or an
 * internal counter may have rolled over, violating monotonicity.
 */
class ClockException : public Exception {
   public:
    ClockException(std::string message);
};

}  // namespace dex
