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

#ifndef FILESTREAM_H
#define FILESTREAM_H

#include "declarations.h"
#include <framework/luaengine/luaobject.h>
#include <framework/util/databuffer.h>

struct PHYSFS_File;

// @bindclass
class FileStream : public LuaObject
{
public:
    FileStream(const std::string& name, PHYSFS_File *fileHandle, bool writeable);
    ~FileStream();

    void cache();
    void close();
    void flush();
    void write(const void *buffer, uint count);
    int read(void *buffer, uint size, uint nmemb = 1);
    void seek(uint pos);
    void skip(uint len);
    uint size();
    uint tell();
    std::string name() { return m_name; }

    uint8 getU8();
    uint16 getU16();
    uint32 getU32();
    uint64 getU64();
    std::string getString();
    BinaryTreePtr getBinaryTree();

    void addU8(uint8 v);
    void addU16(uint16 v);
    void addU32(uint32 v);
    void addU64(uint64 v);
    void addString(const std::string& v);
    BinaryTreePtr makeTree();

    FileStreamPtr asFileStream() { return std::static_pointer_cast<FileStream>(shared_from_this()); }

private:
    void checkWrite();
    void throwError(const std::string& message, bool physfsError = false);

    std::string m_name;
    PHYSFS_File *m_fileHandle;
    uint m_pos;
    bool m_writeable;
    bool m_caching;

    DataBuffer<uint8_t> m_data;
};

#endif
