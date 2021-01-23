#ifndef DEX_EXCEPTIONS_CPP
#define DEX_EXCEPTIONS_CPP

#include "Exceptions.h"
#include <iostream>

namespace dex {

Exception::Exception(string message): message_(message)
{
	cout << "EXCEPTION: " << message << endl;
}

Exception::~Exception() {}

string Exception::getMessage() { return message_; }

ClockException::ClockException(string message) : Exception(message) {}

}
#endif
