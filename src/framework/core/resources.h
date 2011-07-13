/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


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
