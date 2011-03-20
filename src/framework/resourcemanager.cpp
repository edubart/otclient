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


#include "resourcemanager.h"

#include <physfs.h>

ResourceManager g_resources;

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::init(const char *argv0)
{
    PHYSFS_init(argv0);
}

void ResourceManager::terminate()
{
    PHYSFS_deinit();
}

bool ResourceManager::setWriteDir(const std::string& path)
{
    bool ret = (bool)PHYSFS_setWriteDir(path.c_str());

    if(!ret)
        error("Could not set the path \"%s\" as write directory, file write will not work.", path.c_str());
    return ret;
}

bool ResourceManager::addToSearchPath(const std::string& path, bool insertInFront)
{
    if(!PHYSFS_addToSearchPath(path.c_str(), insertInFront ? 0 : 1)) {
        error("Error while adding \"%s\" to resources search path: %s", path.c_str(), PHYSFS_getLastError());
        return false;
    }
    return true;
}

bool ResourceManager::fileExists(const std::string& filePath)
{
    return PHYSFS_exists(filePath.c_str());
}

unsigned char *ResourceManager::loadFile(const std::string& fileName, unsigned int *fileSize)
{
    PHYSFS_file *file = PHYSFS_openRead(fileName.c_str());
    if(!file) {
        error("Failed to load file \"%s\": %s", fileName.c_str(), PHYSFS_getLastError());
        *fileSize = 0;
        return NULL;
    }

    *fileSize = PHYSFS_fileLength(file);
    unsigned char *buffer = new unsigned char[*fileSize + 1];
    PHYSFS_read(file, (void*)buffer, 1, *fileSize);
    buffer[*fileSize] = 0;
    PHYSFS_close(file);
    return buffer;
}

std::string ResourceManager::loadTextFile(const std::string& fileName)
{
    std::string text;
    unsigned int fileSize;
    char *buffer = (char *)loadFile(fileName, &fileSize);
    if(buffer) {
        text.assign(buffer);
        delete[] buffer;
    }
    return text;
}

bool ResourceManager::saveFile(const std::string &fileName, const unsigned char *data, unsigned int size)
{
    PHYSFS_file *file = PHYSFS_openWrite(fileName.c_str());
    if(!file) {
        error("Failed to save file \"%s\": %s", fileName.c_str(), PHYSFS_getLastError());
        return false;
    }

    PHYSFS_write(file, (void*)data, size, 1);
    PHYSFS_close(file);
    return true;
}

bool ResourceManager::saveTextFile(const std::string &fileName, std::string text)
{
    return saveFile(fileName, (const unsigned char*)text.c_str(), text.size());
}
