#include "util.h"

#include <cstdio>

std::string vformat(const char *format, va_list args)
{
    if(!format)
        return "";
    int result = -1, length = 256;
    char *buffer = 0;
    while(result == -1) {
        if(buffer)
            delete [] buffer;
        buffer = new char [length + 1];
        result = vsnprintf(buffer, length, format, args);
        length *= 2;
    }
    std::string s(buffer);
    delete [] buffer;
    return s;
}

std::string format(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::string s = vformat(format, args);
    va_end(args);
    return s;
}
