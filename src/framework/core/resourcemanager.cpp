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

#include <filesystem>
#include <ranges>

#include "resourcemanager.h"
#include "filestream.h"

#include <framework/core/application.h>
#include <framework/luaengine/luainterface.h>
#include <framework/platform/platform.h>

#include <physfs.h>

ResourceManager g_resources;

void ResourceManager::init(const char* argv0)
{
    PHYSFS_init(argv0);
    PHYSFS_permitSymbolicLinks(1);
}

void ResourceManager::terminate()
{
    PHYSFS_deinit();
}

bool ResourceManager::discoverWorkDir(const std::string_view existentFile)
{
    // search for modules directory
    std::string possiblePaths[] = { g_platform.getCurrentDir(),
                                    g_resources.getBaseDir(),
                                    g_resources.getBaseDir() + "../",
                                    g_resources.getBaseDir() + "../share/" + g_app.getCompactName() + "/" };

    bool found = false;
    for (const std::string& dir : possiblePaths) {
        if (!PHYSFS_mount(dir.data(), nullptr, 0))
            continue;

        if (PHYSFS_exists(existentFile.data())) {
            g_logger.debug(stdext::format("Found work dir at '%s'", dir));
            m_workDir = dir;
            found = true;
            break;
        }
        PHYSFS_unmount(dir.data());
    }

    return found;
}

bool ResourceManager::setupUserWriteDir(const std::string_view appWriteDirName)
{
    const auto& userDir = getUserDir();
    std::string dirName;
#ifndef WIN32
    dirName = stdext::format(".%s", appWriteDirName);
#else
    dirName = appWriteDirName;
#endif
    const auto& writeDir = userDir + dirName;

    if (!PHYSFS_setWriteDir(writeDir.data())) {
        if (!PHYSFS_setWriteDir(userDir.data()) || !PHYSFS_mkdir(dirName.data())) {
            g_logger.error(stdext::format("Unable to create write directory '%s': %s", writeDir, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
            return false;
        }
    }
    return setWriteDir(writeDir);
}

bool ResourceManager::setWriteDir(const std::string_view writeDir, bool)
{
    if (!PHYSFS_setWriteDir(writeDir.data())) {
        g_logger.error(stdext::format("Unable to set write directory '%s': %s", writeDir, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
        return false;
    }

    if (!m_writeDir.empty())
        removeSearchPath(m_writeDir);

    m_writeDir = writeDir;

    if (!addSearchPath(writeDir))
        g_logger.error(stdext::format("Unable to add write '%s' directory to search path", writeDir));

    return true;
}

bool ResourceManager::addSearchPath(const std::string_view path, bool pushFront)
{
    std::string savePath{ path };
    if (!PHYSFS_mount(path.data(), nullptr, pushFront ? 0 : 1)) {
        bool found = false;
        for (const std::string& searchPath : m_searchPaths) {
            std::string newPath = searchPath + savePath;
            if (PHYSFS_mount(newPath.data(), nullptr, pushFront ? 0 : 1)) {
                savePath = newPath;
                found = true;
                break;
            }
        }

        if (!found) {
            //g_logger.error(stdext::format("Could not add '%s' to directory search path. Reason %s", path, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
            return false;
        }
    }
    if (pushFront)
        m_searchPaths.push_front(savePath);
    else
        m_searchPaths.push_back(savePath);
    return true;
}

bool ResourceManager::removeSearchPath(const std::string_view path)
{
    if (!PHYSFS_unmount(path.data()))
        return false;
    const auto it = std::find(m_searchPaths.begin(), m_searchPaths.end(), path);
    assert(it != m_searchPaths.end());
    m_searchPaths.erase(it);
    return true;
}

void ResourceManager::searchAndAddPackages(const std::string_view packagesDir, const std::string_view packageExt)
{
    auto files = listDirectoryFiles(packagesDir);
    for (const auto& file : files | std::views::reverse) {
        if (!file.ends_with(packageExt))
            continue;
        std::string package = getRealDir(packagesDir) + "/" + file;
        if (!addSearchPath(package, true))
            g_logger.error(stdext::format("Unable to read package '%s': %s", package, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
    }
}

bool ResourceManager::fileExists(const std::string_view fileName)
{
    return (PHYSFS_exists(resolvePath(fileName).data()) && !directoryExists(fileName));
}

bool ResourceManager::directoryExists(const std::string_view directoryName)
{
    PHYSFS_Stat stat = {};
    if (!PHYSFS_stat(resolvePath(directoryName).data(), &stat)) {
        return false;
    }

    return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
}

void ResourceManager::readFileStream(const std::string_view fileName, std::iostream& out)
{
    const auto& buffer = readFileContents(fileName);
    if (buffer.length() == 0) {
        out.clear(std::ios::eofbit);
        return;
    }
    out.clear(std::ios::goodbit);
    out.write(&buffer[0], buffer.length());
    out.seekg(0, std::ios::beg);
}

std::string ResourceManager::readFileContents(const std::string_view fileName)
{
    const auto& fullPath = resolvePath(fileName);

    PHYSFS_File* file = PHYSFS_openRead(fullPath.data());
    if (!file)
        stdext::throw_exception(stdext::format("unable to open file '%s': %s", fullPath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));

    const int fileSize = PHYSFS_fileLength(file);
    std::string buffer(fileSize, 0);
    PHYSFS_readBytes(file, &buffer[0], fileSize);
    PHYSFS_close(file);

#if ENABLE_ENCRYPTION == 1
    buffer = decrypt(buffer);
#endif

    return buffer;
}

bool ResourceManager::writeFileBuffer(const std::string_view fileName, const uchar* data, uint size)
{
    PHYSFS_file* file = PHYSFS_openWrite(fileName.data());
    if (!file) {
        g_logger.error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        return false;
    }

    PHYSFS_writeBytes(file, data, size);
    PHYSFS_close(file);
    return true;
}

bool ResourceManager::writeFileStream(const std::string_view fileName, std::iostream& in)
{
    const std::streampos oldPos = in.tellg();
    in.seekg(0, std::ios::end);
    const std::streampos size = in.tellg();
    in.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    in.read(&buffer[0], size);
    const bool ret = writeFileBuffer(fileName, (const uchar*)&buffer[0], size);
    in.seekg(oldPos, std::ios::beg);
    return ret;
}

bool ResourceManager::writeFileContents(const std::string_view fileName, const std::string_view data)
{
#if ENABLE_ENCRYPTION == 1
    return writeFileBuffer(fileName, (const uchar*)encrypt(data, std::string_view{ ENCRYPTION_PASSWORD }).data(), data.size());
#else
    return writeFileBuffer(fileName, (const uchar*)data.data(), data.size());
#endif
}

FileStreamPtr ResourceManager::openFile(const std::string_view fileName)
{
    const auto& fullPath = resolvePath(fileName.data());

    PHYSFS_File* file = PHYSFS_openRead(fullPath.data());
    if (!file)
        stdext::throw_exception(stdext::format("unable to open file '%s': %s", fullPath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
    return { new FileStream(fullPath, file, false) };
}

FileStreamPtr ResourceManager::appendFile(const std::string_view fileName)
{
    PHYSFS_File* file = PHYSFS_openAppend(fileName.data());
    if (!file)
        stdext::throw_exception(stdext::format("failed to append file '%s': %s", fileName, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
    return { new FileStream(fileName, file, true) };
}

FileStreamPtr ResourceManager::createFile(const std::string_view fileName)
{
    PHYSFS_File* file = PHYSFS_openWrite(fileName.data());
    if (!file)
        stdext::throw_exception(stdext::format("failed to create file '%s': %s", fileName, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
    return { new FileStream(fileName, file, true) };
}

bool ResourceManager::deleteFile(const std::string_view fileName)
{
    return PHYSFS_delete(resolvePath(fileName).data()) != 0;
}

bool ResourceManager::makeDir(const std::string_view directory)
{
    return PHYSFS_mkdir(directory.data());
}

std::list<std::string> ResourceManager::listDirectoryFiles(const std::string_view directoryPath)
{
    std::list<std::string> files;
    auto* const rc = PHYSFS_enumerateFiles(resolvePath(directoryPath).data());

    for (int i = 0; rc[i] != nullptr; ++i)
        files.emplace_back(rc[i]);

    PHYSFS_freeList(rc);
    return files;
}

std::vector<std::string> ResourceManager::getDirectoryFiles(const std::string_view path, bool filenameOnly, bool recursive)
{
    if (!std::filesystem::exists(path))
        return {};

    const std::filesystem::path p(path);
    return discoverPath(p, filenameOnly, recursive);
}

std::vector<std::string> ResourceManager::discoverPath(const std::filesystem::path& path, bool filenameOnly, bool recursive)
{
    std::vector<std::string> files;

    /* Before doing anything, we have to add this directory to search path,
     * this is needed so it works correctly when one wants to open a file.  */
    addSearchPath(path.generic_string(), true);
    for (std::filesystem::directory_iterator it(path), end; it != end; ++it) {
        if (std::filesystem::is_directory(it->path().generic_string()) && recursive) {
            std::vector<std::string> subfiles = discoverPath(it->path(), filenameOnly, recursive);
            files.insert(files.end(), subfiles.begin(), subfiles.end());
        } else {
            if (filenameOnly)
                files.push_back(it->path().filename().string());
            else
                files.push_back(it->path().generic_string() + "/" + it->path().filename().string());
        }
    }

    return files;
}

std::string ResourceManager::resolvePath(const std::string_view path)
{
    std::string fullPath;
    if (path.starts_with("/"))
        fullPath = path;
    else {
        const auto& scriptPath = "/" + g_lua.getCurrentSourcePath();
        if (!scriptPath.empty())
            fullPath += scriptPath + "/";
        fullPath += path;
    }
    if (!(fullPath.starts_with("/")))
        g_logger.traceWarning(stdext::format("the following file path is not fully resolved: %s", path));

    stdext::replace_all(fullPath, "//", "/");
    return fullPath;
}

std::string ResourceManager::getRealDir(const std::string_view path)
{
    std::string dir;
    const char* cdir = PHYSFS_getRealDir(resolvePath(path).data());
    if (cdir)
        dir = cdir;
    return dir;
}

std::string ResourceManager::getRealPath(const std::string_view path)
{
    return getRealDir(path) + "/" + path.data();
}

std::string ResourceManager::getBaseDir()
{
    return PHYSFS_getBaseDir();
}

std::string ResourceManager::getUserDir()
{
    return PHYSFS_getPrefDir("otclient", "otclient");
}

std::string ResourceManager::guessFilePath(const std::string_view filename, const std::string_view type)
{
    std::string fn{ filename.data() };
    if (isFileType(filename, type))
        return fn;

    return fn + "."s + type.data();
}

bool ResourceManager::isFileType(const std::string_view filename, const std::string_view type)
{
    if (filename.ends_with("."s + type.data()))
        return true;
    return false;
}

ticks_t ResourceManager::getFileTime(const std::string_view filename)
{
    return g_platform.getFileModificationTime(getRealPath(filename));
}

std::string ResourceManager::encrypt(const std::string_view data, const std::string_view password)
{
    const int len = data.length(),
        plen = password.length();

    std::ostringstream ss;
    int j = 0;
    for (int i = -1; ++i < len;) {
        int ct = data[i];
        if (i % 2) {
            ct = ct - password[j] + i;
        } else {
            ct = ct + password[j] - i;
        }
        ss << static_cast<char>(ct);
        ++j;

        if (j >= plen)
            j = 0;
    }

    return ss.str();
}
std::string ResourceManager::decrypt(const std::string_view data)
{
    const auto password = std::string_view{ ENCRYPTION_PASSWORD };

    const int len = data.length(),
        plen = password.length();

    std::ostringstream ss;
    int j = 0;
    for (int i = -1; ++i < len;) {
        int ct = data[i];
        if (i % 2) {
            ct = ct + password[j] - i;
        } else {
            ct = ct - password[j] + i;
        }
        ss << static_cast<char>(ct);
        ++j;

        if (j >= plen)
            j = 0;
    }

    return ss.str();
}

uint8_t* ResourceManager::decrypt(uint8_t* data, int32_t size)
{
    const auto password = std::string_view{ ENCRYPTION_PASSWORD };
    const int plen = password.length();

    auto* const new_Data = new uint8_t[size];

    int j = 0;
    for (int i = -1; ++i < size;) {
        const int ct = data[i];
        if (i % 2) {
            new_Data[i] = ct + password[j] - i;
        } else {
            new_Data[i] = ct - password[j] + i;
        }
        data[i] = new_Data[i];
        ++j;

        if (j >= plen)
            j = 0;
    }

    return nullptr;
}

void ResourceManager::runEncryption(const std::string_view password)
{
    std::vector<std::string> excludedExtensions = { ".rar",".ogg",".xml",".dll",".exe", ".log",".otb" };
    for (const auto& entry : std::filesystem::recursive_directory_iterator("./")) {
        std::string ext = entry.path().extension().string();
        if (std::find(excludedExtensions.begin(), excludedExtensions.end(), ext) != excludedExtensions.end())
            continue;

        std::ifstream ifs(entry.path().string(), std::ios_base::binary);
        std::string data((std::istreambuf_iterator(ifs)), std::istreambuf_iterator<char>());
        ifs.close();
        data = encrypt(data, password);
        save_string_into_file(data, entry.path().string());
    }
}

void ResourceManager::save_string_into_file(const std::string_view contents, const std::string_view name)
{
    std::ofstream datFile;
    datFile.open(name.data(), std::ofstream::binary | std::ofstream::trunc | std::ofstream::out);
    datFile.write(contents.data(), contents.size());
    datFile.close();
}
