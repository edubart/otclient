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

#include "resourcemanager.h"
#include "filestream.h"

#include <framework/application.h>
#include <framework/luascript/luainterface.h>

#include <physfs.h>

ResourceManager g_resources;

void ResourceManager::init(const char *argv0)
{
    PHYSFS_init(argv0);
}

void ResourceManager::terminate()
{
    PHYSFS_deinit();
}

bool ResourceManager::setupWriteDir(const std::string& appWriteDirName)
{
    std::string userDir = PHYSFS_getUserDir();
    std::string dirName = Fw::mkstr(".", appWriteDirName);
    std::string writeDir = userDir + dirName;
    if(!PHYSFS_setWriteDir(writeDir.c_str())) {
        if(!PHYSFS_setWriteDir(userDir.c_str()))
            return false;
        if(!PHYSFS_mkdir(dirName.c_str())) {
            PHYSFS_setWriteDir(NULL);
            return false;
        }
        if(!PHYSFS_setWriteDir(writeDir.c_str()))
            return false;
    }
    addToSearchPath(writeDir);
    return true;
}

bool ResourceManager::addToSearchPath(const std::string& path, bool insertInFront)
{
    if(!PHYSFS_addToSearchPath(path.c_str(), insertInFront ? 0 : 1))
        return false;
    return true;
}

void ResourceManager::searchAndAddPackages(const std::string& packagesDir, const std::string& packageExt, bool append)
{
    auto files = listDirectoryFiles(resolvePath(packagesDir));
    for(const std::string& file : files) {
        if(boost::ends_with(file, packageExt))
            addToSearchPath(packagesDir + "/" + file, !append);
    }
}

bool ResourceManager::fileExists(const std::string& fileName)
{
    return (PHYSFS_exists(resolvePath(fileName).c_str()) && !PHYSFS_isDirectory(resolvePath(fileName).c_str()));
}

bool ResourceManager::directoryExists(const std::string& directoryName)
{
    return (PHYSFS_isDirectory(resolvePath(directoryName).c_str()));
}

void ResourceManager::loadFile(const std::string& fileName, std::iostream& out)
{
    std::string fullPath = resolvePath(fileName);
    out.clear(std::ios::goodbit);
    PHYSFS_file* file = PHYSFS_openRead(fullPath.c_str());
    if(!file) {
        out.clear(std::ios::failbit);
        Fw::throwException("failed to load file '", fullPath.c_str(), "': ", PHYSFS_getLastError());
    } else {
        int fileSize = PHYSFS_fileLength(file);
        if(fileSize > 0) {
            std::vector<char> buffer(fileSize);
            PHYSFS_read(file, (void*)&buffer[0], 1, fileSize);
            out.write(&buffer[0], fileSize);
        } else
            out.clear(std::ios::eofbit);
        PHYSFS_close(file);
        out.seekg(0, std::ios::beg);
    }
}

std::string ResourceManager::loadFile(const std::string& fileName)
{
    std::stringstream fin;
    loadFile(fileName, fin);
    return fin.str();
}

bool ResourceManager::saveFile(const std::string& fileName, const uchar* data, uint size)
{
    PHYSFS_file* file = PHYSFS_openWrite(fileName.c_str());
    if(!file)
    {
        logError(PHYSFS_getLastError());
        return false;
    }

    PHYSFS_write(file, (void*)data, size, 1);
    PHYSFS_close(file);
    return true;
}

bool ResourceManager::saveFile(const std::string& fileName, std::iostream& in)
{
    std::streampos oldPos = in.tellg();
    in.seekg(0, std::ios::end);
    std::streampos size = in.tellg();
    in.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    in.read(&buffer[0], size);
    bool ret = saveFile(fileName, (const uchar*)&buffer[0], size);
    in.seekg(oldPos, std::ios::beg);
    return ret;
}

bool ResourceManager::saveFile(const std::string& fileName, const std::string& data)
{
    return saveFile(fileName, (const uchar*)data.c_str(), data.size());
}

FileStreamPtr ResourceManager::openFile(const std::string& fileName)
{
    std::string fullPath = resolvePath(fileName);
    PHYSFS_File* file = PHYSFS_openRead(fullPath.c_str());
    if(!file) {
        logTraceError("unable to open file '", fullPath, "': ", PHYSFS_getLastError());
        return nullptr;
    }
    return FileStreamPtr(new FileStream(fullPath, file));
}

FileStreamPtr ResourceManager::appendFile(const std::string& fileName)
{
    PHYSFS_File* file = PHYSFS_openAppend(fileName.c_str());
    if(!file) {
        logTraceError("failed to append file '", fileName, "': ", PHYSFS_getLastError());
        return nullptr;
    }
    return FileStreamPtr(new FileStream(fileName, file));
}

FileStreamPtr ResourceManager::createFile(const std::string& fileName)
{
    PHYSFS_File* file = PHYSFS_openWrite(fileName.c_str());
    if(!file) {
        logTraceError("failed to create file '", fileName, "': ", PHYSFS_getLastError());
        return nullptr;
    }
    return FileStreamPtr(new FileStream(fileName, file));
}

bool ResourceManager::deleteFile(const std::string& fileName)
{
    return PHYSFS_delete(resolvePath(fileName).c_str()) != 0;
}

std::list<std::string> ResourceManager::listDirectoryFiles(const std::string& directoryPath)
{
    std::list<std::string> files;
    auto rc = PHYSFS_enumerateFiles(resolvePath(directoryPath).c_str());

    for(int i = 0; rc[i] != NULL; i++)
        files.push_back(rc[i]);

    PHYSFS_freeList(rc);
    return files;
}

std::string ResourceManager::resolvePath(const std::string& path)
{
    std::string fullPath;
    if(boost::starts_with(path, "/"))
        fullPath = path;
    else {
        std::string scriptPath = "/" + g_lua.getCurrentSourcePath();
        if(!scriptPath.empty())
            fullPath += scriptPath + "/";
        fullPath += path;
    }
    if(!(boost::starts_with(fullPath, "/")))
        logTraceWarning("the following file path is not fully resolved: ", path);
    boost::replace_all(fullPath, "//", "/");
    return fullPath;
}

std::string ResourceManager::getBaseDir()
{
    return PHYSFS_getBaseDir();
}

