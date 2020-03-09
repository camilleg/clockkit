
//----------------------------------------------------------------------------//
#ifndef DEX_EXCEPTIONS_CPP
#define DEX_EXCEPTIONS_CPP
//----------------------------------------------------------------------------//
#include "Exceptions.h"
#include <iostream>
//----------------------------------------------------------------------------//
using namespace std;
namespace dex {
//----------------------------------------------------------------------------//

Exception::Exception(string message)
	: message_(message)
{
	cout << "EXCEPTION: " << message << endl;
}

Exception::~Exception() {}

string Exception::getMessage()
{ return message_; }

ClockException::ClockException(string message)
	: Exception(message) {}



//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_EXCEPTIONS_CPP
//----------------------------------------------------------------------------//
