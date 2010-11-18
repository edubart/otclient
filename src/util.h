#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <cstdarg>

std::string vformat(const char *format, va_list args);
std::string format(const char *format, ...);

#endif