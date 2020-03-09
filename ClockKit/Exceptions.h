
//----------------------------------------------------------------------------//
#ifndef DEX_EXCEPTIONS_H
#define DEX_EXCEPTIONS_H
//----------------------------------------------------------------------------//
#include "Common.h"
#include <string>
//----------------------------------------------------------------------------//
using namespace std;
namespace dex {
//----------------------------------------------------------------------------//


/**
 * The base class for exceptions in the DEX package.
 * Each exception has a simple string message.
 */
class Exception
{
protected:

    /**
     * The message associated with this exception.
     */
	string message_;

public:

	/**
	 * Creates an exception with the given message
	 */
	Exception(string message);
	
    /**
     * Desturctor.
     */
    virtual ~Exception();
    
	/**
	 * @return the message supplied with the exception.
	 */
	virtual string getMessage();
};


/**
 * An Exception specific to the ClockKit classes in DEX.
 * Clocks may fail for all sorts of reasons.  A remote
 * clock may not be able to contact a server, or an
 * internal counter may have rolled over, violating montanicity.
 */
class ClockException : public Exception
{
public:

	/**
	 * Creates a ClockException with the given message.
	 */
	ClockException(string message);
};



//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_EXCEPTIONS_H
//----------------------------------------------------------------------------//

