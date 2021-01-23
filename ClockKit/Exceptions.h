#ifndef DEX_EXCEPTIONS_H
#define DEX_EXCEPTIONS_H

#include <string>

using namespace std;
namespace dex {

/**
 * The base class for exceptions in the DEX package.
 * Each exception has a simple string message.
 */
class Exception
{
protected:
  string message_;
public:
  Exception(string message);
  virtual ~Exception();
  virtual string getMessage();
};

/**
 * An Exception specific to the ClockKit classes in DEX.
 * Clocks may fail for all sorts of reasons.  A remote
 * clock may not be able to contact a server, or an
 * internal counter may have rolled over, violating monotonicity.
 */
class ClockException : public Exception
{
public:
  ClockException(string message);
};

}
#endif
