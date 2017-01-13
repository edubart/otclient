/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#include "declarations.h"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

// @bindsingleton g_resources
class ResourceManager
{
public:
    // @dontbind
    void init(const char *argv0);
    // @dontbind
    void terminate();

    bool discoverWorkDir(const std::string& existentFile);
    bool setupUserWriteDir(const std::string& appWriteDirName);
    bool setWriteDir(const std::string& writeDir, bool create = false);

    bool addSearchPath(const std::string& path, bool pushFront = false);
    bool removeSearchPath(const std::string& path);
    void searchAndAddPackages(const std::string& packagesDir, const std::string& packagesExt);

    bool fileExists(const std::string& fileName);
    bool directoryExists(const std::string& directoryName);

    // @dontbind
    void readFileStream(const std::string& fileName, std::iostream& out);
    std::string readFileContents(const std::string& fileName);
    // @dontbind
    bool writeFileBuffer(const std::string& fileName, const uchar* data, uint size);
    bool writeFileContents(const std::string& fileName, const std::string& data);
    // @dontbind
    bool writeFileStream(const std::string& fileName, std::iostream& in);

    FileStreamPtr openFile(const std::string& fileName);
    FileStreamPtr appendFile(const std::string& fileName);
    FileStreamPtr createFile(const std::string& fileName);
    bool deleteFile(const std::string& fileName);

    bool makeDir(const std::string directory);
    std::list<std::string> listDirectoryFiles(const std::string& directoryPath = "");
    std::vector<std::string> getDirectoryFiles(const std::string& path, bool filenameOnly, bool recursive);

    std::string resolvePath(const std::string& path);
    std::string getRealDir(const std::string& path);
    std::string getRealPath(const std::string& path);
    std::string getBaseDir();
    std::string getUserDir();
    std::string getWriteDir() { return m_writeDir; }
    std::string getWorkDir() { return m_workDir; }
    std::deque<std::string> getSearchPaths() { return m_searchPaths; }

    std::string guessFilePath(const std::string& filename, const std::string& type);
    bool isFileType(const std::string& filename, const std::string& type);
    ticks_t getFileTime(const std::string& filename);

protected:
    std::vector<std::string> discoverPath(const fs::path& path, bool filenameOnly, bool recursive);

private:
    std::string m_workDir;
    std::string m_writeDir;
    std::deque<std::string> m_searchPaths;
};

extern ResourceManager g_resources;

#endif
