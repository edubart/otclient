#ifndef NETWORKEXCEPTION_H
#define NETWORKEXCEPTION_H

#include "declarations.h"

class NetworkException : public Exception
{
public:
    NetworkException(const std::string& what) : Exception(what) { }
};

#endif
