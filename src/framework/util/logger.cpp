#include "logger.h"

#include <cstdlib>

void Logger::log(LogLevel level, const std::string& message, std::string prettyFunction)
{
    if(!prettyFunction.empty()) {
        prettyFunction = prettyFunction.substr(0, prettyFunction.find_first_of('('));
        if(prettyFunction.find_last_of(' ') != std::string::npos)
            prettyFunction = prettyFunction.substr(prettyFunction.find_last_of(' ') + 1);
        if(!prettyFunction.empty())
            std::cout << "[" << prettyFunction << "] ";
    }

    std::cout << message << std::endl;

    if(level == LogFatal)
        exit(-1);
}
