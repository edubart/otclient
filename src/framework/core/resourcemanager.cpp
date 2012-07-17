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

#include <framework/core/application.h>
#include <framework/luaengine/luainterface.h>

#include <physfs.h>
#include <boost/filesystem.hpp>

ResourceManager g_resources;

void ResourceManager::init(const char *argv0)
{
    PHYSFS_init(argv0);
}

void ResourceManager::terminate()
{
    PHYSFS_deinit();
}

void ResourceManager::discoverWorkDir(const std::string& appName, const std::string& existentFile)
{
    // search for modules directory
    std::string sep = PHYSFS_getDirSeparator();
    std::string possiblePaths[] = { boost::filesystem::current_path().generic_string() + sep,
                                    g_resources.getBaseDir() + ".." + sep,
                                    g_resources.getBaseDir() + ".." + sep + "share" + sep + appName + sep,
                                    g_resources.getBaseDir() + appName + sep };
    bool found = false;
    for(const std::string& dir : possiblePaths) {
        // try to directory to modules path to see if it exists
        std::ifstream fin(dir + existentFile);
        if(fin) {
            g_logger.debug(stdext::format("Found work dir at '%s'", dir.c_str()));
            m_workDir = dir;
            found = true;
            break;
        }
    }

    if(!found)
        g_logger.fatal(stdext::format("Unable to find %s, the application cannot be initialized.", existentFile));
}

bool ResourceManager::setupUserWriteDir(const std::string& appWriteDirName)
{
    std::string userDir = PHYSFS_getUserDir();
    std::string dirName;
#ifndef WIN32
    dirName = stdext::format(".%s", appWriteDirName);
#else
    dirName = appWriteDirName;
#endif
    std::string writeDir = userDir + dirName;
    return setWriteDir(writeDir);
}

bool ResourceManager::setWriteDir(const std::string& writeDir, bool create)
{
    boost::filesystem::create_directory(writeDir);

    if(!PHYSFS_setWriteDir(writeDir.c_str())) {
        g_logger.error(stdext::format("Unable to set write directory '%s': %s", writeDir, PHYSFS_getLastError()));
        return false;
    }

    if(!m_writeDir.empty())
        removeSearchPath(m_writeDir);

    m_writeDir = writeDir;

    if(!addSearchPath(writeDir))
        g_logger.error(stdext::format("Unable to add write directory '%s' to search path"));

    return true;
}

bool ResourceManager::addSearchPath(const std::string& path, bool pushFront)
{
    if(!PHYSFS_addToSearchPath(path.c_str(), pushFront ? 0 : 1))
        return false;
    if(pushFront)
        m_searchPaths.push_front(path);
    else
        m_searchPaths.push_back(path);
    m_hasSearchPath = true;
    return true;
}

bool ResourceManager::removeSearchPath(const std::string& path)
{
    if(!PHYSFS_removeFromSearchPath(path.c_str()))
        return false;
    auto it = std::find(m_searchPaths.begin(), m_searchPaths.end(), path);
    assert(it != m_searchPaths.end());
    m_searchPaths.erase(it);
    return true;
}

void ResourceManager::searchAndAddPackages(const std::string& packagesDir, const std::string& packageExt)
{
    auto files = listDirectoryFiles(packagesDir);
    for(auto it = files.rbegin(); it != files.rend(); ++it) {
        const std::string& file = *it;
        if(!boost::ends_with(file, packageExt))
            continue;
        std::string package = getRealDir(packagesDir) + "/" + file;
        if(!addSearchPath(package, true))
            g_logger.error(stdext::format("Unable to read package '%s': %s", package, PHYSFS_getLastError()));
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
    out.clear(std::ios::goodbit);
    if(m_hasSearchPath) {
        std::string fullPath = resolvePath(fileName);
        PHYSFS_file* file = PHYSFS_openRead(fullPath.c_str());
        if(!file) {
            out.clear(std::ios::failbit);
            stdext::throw_exception(stdext::format("failed to load file '%s': %s", fullPath.c_str(), PHYSFS_getLastError()));
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
    } else {
        std::ifstream fin(fileName);
        if(!fin) {
            out.clear(std::ios::failbit);
            stdext::throw_exception(stdext::format("failed to load file '%s': %s", fileName.c_str(), PHYSFS_getLastError()));
        } else {
            out << fin.rdbuf();
        }
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
    if(!file) {
        g_logger.error(PHYSFS_getLastError());
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
    if(!file)
        stdext::throw_exception(stdext::format("unable to open file '%s': %s", fullPath, PHYSFS_getLastError()));
    return FileStreamPtr(new FileStream(fullPath, file, false));
}

FileStreamPtr ResourceManager::appendFile(const std::string& fileName)
{
    PHYSFS_File* file = PHYSFS_openAppend(fileName.c_str());
    if(!file)
        stdext::throw_exception(stdext::format("failed to append file '%s': %s", fileName, PHYSFS_getLastError()));
    return FileStreamPtr(new FileStream(fileName, file, true));
}

FileStreamPtr ResourceManager::createFile(const std::string& fileName)
{
    PHYSFS_File* file = PHYSFS_openWrite(fileName.c_str());
    if(!file)
        stdext::throw_exception(stdext::format("failed to create file '%s': %s", fileName, PHYSFS_getLastError()));
    return FileStreamPtr(new FileStream(fileName, file, true));
}

bool ResourceManager::deleteFile(const std::string& fileName)
{
    return PHYSFS_delete(resolvePath(fileName).c_str()) != 0;
}

bool ResourceManager::makeDir(const std::string directory)
{
    return PHYSFS_mkdir(directory.c_str());
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
        g_logger.traceWarning(stdext::format("the following file path is not fully resolved: %s", path));
    boost::replace_all(fullPath, "//", "/");
    return fullPath;
}

std::string ResourceManager::getRealDir(const std::string& path)
{
    std::string dir;
    const char *cdir = PHYSFS_getRealDir(resolvePath(path).c_str());
    if(cdir)
        dir = cdir;
    return dir;
}

std::string ResourceManager::getBaseDir()
{
    return PHYSFS_getBaseDir();
}

