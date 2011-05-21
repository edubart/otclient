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


#include <prerequisites.h>
#include <core/resources.h>
#include <core/platform.h>

#include <physfs.h>

Resources g_resources;

void Resources::init(const char *argv0)
{
    PHYSFS_init(argv0);

    // try to find data directory
    std::list<std::string> searchPaths;
    std::string dir;
    std::string baseDir = PHYSFS_getBaseDir();

    std::list<std::string> possibleDirs;
    possibleDirs.push_back("data");
    possibleDirs.push_back(baseDir + "data");
    possibleDirs.push_back(baseDir + "../data");
    possibleDirs.push_back(baseDir + "../share/otclient/data");
    possibleDirs.push_back("");

    bool found = false;
    foreach(dir, possibleDirs) {
        if(g_resources.addToSearchPath(dir)) {
            flogInfo("Using data directory: %s", dir.c_str());
            found = true;
            break;
        }
    }
    if(!found)
        logFatal("ERROR: could not find data directory");

    // setup write directory
    dir = Platform::getAppUserDir();
    if(g_resources.setWriteDir(dir))
        g_resources.addToSearchPath(dir);
    else
        logError("ERROR: could not setup write directory");
}

void Resources::terminate()
{
    PHYSFS_deinit();
}

bool Resources::setWriteDir(const std::string& path)
{
    if(!PHYSFS_setWriteDir(path.c_str()))
        return false;
    return true;
}

bool Resources::addToSearchPath(const std::string& path, bool insertInFront /*= true*/)
{
    if(!PHYSFS_addToSearchPath(path.c_str(), insertInFront ? 0 : 1))
        return false;
    return true;
}

void Resources::addPackagesToSearchPath(const std::string &packagesDirectory, const std::string &packageExtension, bool append)
{
    auto files = listDirectoryFiles(resolvePath(packagesDirectory));
    foreach(const std::string& file, files) {
        if(boost::ends_with(file, packageExtension))
            addToSearchPath(packagesDirectory + "/" + file, !append);
    }
}

bool Resources::fileExists(const std::string& fileName)
{
    return (PHYSFS_exists(resolvePath(fileName).c_str()) && !PHYSFS_isDirectory(resolvePath(fileName).c_str()));
}

bool Resources::directoryExists(const std::string& directoryName)
{
    return (PHYSFS_exists(resolvePath(directoryName).c_str()) && PHYSFS_isDirectory(resolvePath(directoryName).c_str()));
}

bool Resources::loadFile(const std::string& fileName, std::iostream& out)
{
    std::string fullPath = resolvePath(fileName);
    out.clear(std::ios::goodbit);
    PHYSFS_file *file = PHYSFS_openRead(fullPath.c_str());
    if(!file) {
        flogError("ERROR: Failed to load file \"%s\": %s", fullPath.c_str() % PHYSFS_getLastError());
        out.clear(std::ios::failbit);
        return false;
    } else {
        int fileSize = PHYSFS_fileLength(file);
        if(fileSize > 0) {
            char *buffer = new char[fileSize];
            PHYSFS_read(file, (void*)buffer, 1, fileSize);
            out.write(buffer, fileSize);
            delete[] buffer;
        } else
            out.clear(std::ios::eofbit);
        PHYSFS_close(file);
        out.seekg(0, std::ios::beg);
        return true;
    }
}

bool Resources::saveFile(const std::string &fileName, const uchar *data, uint size)
{
    PHYSFS_file *file = PHYSFS_openWrite(resolvePath(fileName).c_str());
    if(!file) {
        flogError("ERROR: Failed to save file \"%s\": %s", fileName.c_str() % PHYSFS_getLastError());
        return false;
    }

    PHYSFS_write(file, (void*)data, size, 1);
    PHYSFS_close(file);
    return true;
}

bool Resources::saveFile(const std::string &fileName, std::istream& in)
{
    std::streampos oldPos = in.tellg();
    in.seekg(0, std::ios::end);
    std::streampos size = in.tellg();
    in.seekg(0, std::ios::beg);
    char *buffer = new char[size];
    in.read(buffer, size);
    bool ret = saveFile(fileName, (const uchar*)buffer, size);
    delete[] buffer;
    in.seekg(oldPos, std::ios::beg);
    return ret;
}

bool Resources::deleteFile(const std::string& fileName)
{
    return PHYSFS_delete(resolvePath(fileName).c_str()) != 0;
}

std::list<std::string> Resources::listDirectoryFiles(const std::string& directoryPath)
{
    std::list<std::string> files;
    char **rc = PHYSFS_enumerateFiles(resolvePath(directoryPath).c_str());

    for(char **i = rc; *i != NULL; i++)
        files.push_back(*i);

    PHYSFS_freeList(rc);
    return files;
}


void Resources::pushCurrentPath(const std::string &currentPath)
{
    m_currentPaths.push(currentPath);
}

void Resources::popCurrentPath()
{
    m_currentPaths.pop();
}

std::string Resources::resolvePath(const std::string& path)
{
    std::string fullPath;
    if(boost::starts_with(path, "/"))
        fullPath = path.substr(1);
    else {
        if(m_currentPaths.size() > 0) {
            std::string currentPath = m_currentPaths.top();
            if(currentPath.length() > 0)
                fullPath += currentPath + "/";
        }
        fullPath += path;
    }
    return fullPath;
}

