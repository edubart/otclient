#ifndef RESOURCES_H
#define RESOURCES_H

#include "declarations.h"

class ResourceManager
{
public:
    void init(const char* argv0);
    void terminate();

    /// Set output files directory
    bool setWriteDir(const std::string& path);

    /// Add an package or directory to the search path
    bool addToSearchPath(const std::string& path, bool insertInFront = true);

    /// Search and packages from a directory to the search path
    void searchAndAddPackages(const std::string& packagesDir, const std::string& packagesExt, bool append);

    bool fileExists(const std::string& fileName);
    bool directoryExists(const std::string& directoryName);

    void loadFile(const std::string& fileName, std::iostream& out);
    std::string loadFile(const std::string& fileName);

    bool saveFile(const std::string& fileName, const uchar* data, uint size);
    bool saveFile(const std::string& fileName, const std::string& data);
    bool saveFile(const std::string& fileName, std::istream& in);

    bool deleteFile(const std::string& fileName);

    std::list<std::string> listDirectoryFiles(const std::string& directoryPath = "");

    std::string resolvePath(const std::string& path);
};

extern ResourceManager g_resources;

#endif
