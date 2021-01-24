#include "Exceptions.h"
#include <iostream>

namespace dex {

Exception::Exception(std::string message)
    : message_(message)
{
    std::cout << "EXCEPTION: " << message << std::endl;
}

Exception::~Exception()
{
}

std::string Exception::getMessage()
{
    return message_;
}

ClockException::ClockException(std::string message)
    : Exception(message)
{
}

}  // namespace dex
