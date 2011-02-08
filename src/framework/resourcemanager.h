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


#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "prerequisites.h"

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    void init(const char *argv0);
    void terminate();

    /// Sets the write directory
    bool setWriteDir(const std::string &path);

    /// Adds a directory or zip archive to the search path
    bool addToSearchPath(const std::string& path, bool insertInFront = true);

    /// Checks whether the given file exists in the search path
    bool fileExists(const std::string& filePath);

    /// Searches for zip files and adds them to the search path
    void searchAndAddArchives(const std::string &path,
                              const std::string &ext,
                              const bool append);

    /** Load a file by allocating a buffer and filling it with the file contents
     * where fileSize will be set to the file size.
     * The returned buffer must be freed with delete[]. */
    unsigned char *loadFile(const std::string &fileName, unsigned int *fileSize);

    /// Loads a text file into a std::string
    std::string loadTextFile(const std::string &fileName);

    /// Save a file into write directory
    bool saveFile(const std::string &fileName, const unsigned char *data, unsigned int size);

    /// Save a text file into write directory
    bool saveTextFile(const std::string &fileName, std::string text);
};

extern ResourceManager g_resources;

#endif // RESOURCEMANAGER_H
