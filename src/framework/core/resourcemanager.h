/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#pragma once

#include "declarations.h"
#include <filesystem>

 // @bindsingleton g_resources
class ResourceManager
{
public:
    // @dontbind
    void init(const char* argv0);
    // @dontbind
    void terminate();

    bool discoverWorkDir(const std::string_view existentFile);
    bool setupUserWriteDir(const std::string_view appWriteDirName);
    bool setWriteDir(const std::string_view writeDir, bool create = false);

    bool addSearchPath(const std::string_view path, bool pushFront = false);
    bool removeSearchPath(const std::string_view path);
    void searchAndAddPackages(const std::string_view packagesDir, const std::string_view packageExt);

    bool fileExists(const std::string_view fileName);
    bool directoryExists(const std::string_view directoryName);

    // @dontbind
    void readFileStream(const std::string_view fileName, std::iostream& out);
    std::string readFileContents(const std::string_view fileName);
    // @dontbind
    bool writeFileBuffer(const std::string_view fileName, const uchar* data, uint size);
    bool writeFileContents(const std::string_view fileName, const std::string_view data);
    // @dontbind
    bool writeFileStream(const std::string_view fileName, std::iostream& in);

    FileStreamPtr openFile(const std::string_view fileName);
    FileStreamPtr appendFile(const std::string_view fileName);
    FileStreamPtr createFile(const std::string_view fileName);
    bool deleteFile(const std::string_view fileName);

    bool makeDir(const std::string_view directory);
    std::list<std::string> listDirectoryFiles(const std::string_view directoryPath = "");
    std::vector<std::string> getDirectoryFiles(const std::string_view path, bool filenameOnly, bool recursive);

    std::string resolvePath(const std::string_view path);
    std::string getRealDir(const std::string_view path);
    std::string getRealPath(const std::string_view path);
    std::string getBaseDir();
    std::string getUserDir();
    std::string getWriteDir() { return m_writeDir; }
    std::string getWorkDir() { return m_workDir; }
    std::deque<std::string> getSearchPaths() { return m_searchPaths; }

    std::string guessFilePath(const std::string_view filename, const std::string_view type);
    bool isFileType(const std::string_view filename, const std::string_view type);
    ticks_t getFileTime(const std::string_view filename);

    std::string encrypt(const std::string_view data, const std::string_view password);
    std::string decrypt(const std::string_view data);
    static uint8_t* decrypt(uint8_t* data, int32_t size);
    void runEncryption(const std::string_view password);
    void save_string_into_file(const std::string_view contents, const std::string_view name);

protected:
    std::vector<std::string> discoverPath(const std::filesystem::path& path, bool filenameOnly, bool recursive);

private:
    std::string m_workDir;
    std::string m_writeDir;
    std::deque<std::string> m_searchPaths;
};

extern ResourceManager g_resources;
