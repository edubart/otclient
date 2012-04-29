/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

class ResourceManager
{
public:
    void init(const char *argv0);
    void terminate();

    bool setupWriteDir(const std::string& appWriteDirName);

    bool addToSearchPath(const std::string& path, bool insertInFront = true);
    void searchAndAddPackages(const std::string& packagesDir, const std::string& packagesExt, bool append);

    bool fileExists(const std::string& fileName);
    bool directoryExists(const std::string& directoryName);

    void loadFile(const std::string& fileName, std::iostream& out);
    std::string loadFile(const std::string& fileName);
    bool saveFile(const std::string& fileName, const uchar* data, uint size);
    bool saveFile(const std::string& fileName, const std::string& data);
    bool saveFile(const std::string& fileName, std::iostream& in);

    FileStreamPtr openFile(const std::string& fileName);
    FileStreamPtr appendFile(const std::string& fileName);
    FileStreamPtr createFile(const std::string& fileName);
    bool deleteFile(const std::string& fileName);

    bool makeDir(const std::string directory);
    std::list<std::string> listDirectoryFiles(const std::string& directoryPath = "");

    std::string resolvePath(const std::string& path);
    std::string getBaseDir();
};

extern ResourceManager g_resources;

#endif
