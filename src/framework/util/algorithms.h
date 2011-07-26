#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <algorithm>
#include <string>

inline std::string getPathDirectory(const std::string& sourcePath)
{
    std::string dir;
    std::size_t pos = sourcePath.find_last_of('/');
    if(pos != std::string::npos)
        dir = sourcePath.substr(0, pos);
    return dir;
}

#endif // ALGORITHMS_H
