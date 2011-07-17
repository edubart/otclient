#ifndef RESOURCES_H
#define RESOURCES_H

#include <global.h>
#include <stack>

class Resources
{
public:
    Resources() { }

    void init(const char *argv0);
    void terminate();

    /// Set output files dir
    bool setWriteDir(const std::string &path);

    /// Add an package or directory to the search path
    bool addToSearchPath(const std::string& path, bool insertInFront = true);
    /// Add all packages from a directory
    void addPackagesToSearchPath(const std::string &packagesDirectory, const std::string &packageExtension, bool append);

    bool fileExists(const std::string& fileName);
    bool directoryExists(const std::string& directoryName);

    bool loadFile(const std::string& fileName, std::iostream& out);

    bool saveFile(const std::string& fileName, const uchar *data, uint size);
    bool saveFile(const std::string& fileName, const std::string& data) { return saveFile(fileName, (const uchar*)data.c_str(), data.size()); }
    bool saveFile(const std::string& fileName, std::istream& in);

    bool deleteFile(const std::string& fileName);

    std::list<std::string> listDirectoryFiles(const std::string& directoryPath = "");

    void pushCurrentPath(const std::string &currentPath);
    void popCurrentPath();
    std::string resolvePath(const std::string& path);

private:
    std::stack<std::string> m_currentPaths;
};

extern Resources g_resources;

#endif // RESOURCES_H
