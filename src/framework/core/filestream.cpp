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

#include "filestream.h"

#include <physfs.h>

FileStream::FileStream(const std::string& name, PHYSFS_File *fileHandle)
{
    m_name = name;
    m_fileHandle = fileHandle;
}

FileStream::~FileStream()
{
    close();
}

void FileStream::close()
{
    if(m_fileHandle) {
        if(PHYSFS_isInit() && PHYSFS_close(m_fileHandle) == 0)
            logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());

        m_fileHandle = nullptr;
    }
}

void FileStream::flush()
{
    if(PHYSFS_flush(m_fileHandle) == 0)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
}

void FileStream::read(void *buffer, uint count)
{
    if(PHYSFS_read(m_fileHandle, buffer, 1, count) != count)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
}

void FileStream::write(void *buffer, uint count)
{
    if(PHYSFS_write(m_fileHandle, buffer, 1, count) != count)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
}

void FileStream::seek(uint pos)
{
    if(PHYSFS_seek(m_fileHandle, pos) == 0)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
}

int FileStream::size()
{
    return PHYSFS_fileLength(m_fileHandle);
}

uint8 FileStream::getU8()
{
    uint8 v = 0;
    if(PHYSFS_read(m_fileHandle, &v, 1, 1) != 1)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
    return v;
}

uint16 FileStream::getU16()
{
    uint16 v = 0;
    if(PHYSFS_readULE16(m_fileHandle, &v) == 0)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
    return v;
}

uint32 FileStream::getU32()
{
    uint32 v = 0;
    if(PHYSFS_readULE32(m_fileHandle, &v) == 0)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
    return v;
}

uint64 FileStream::getU64()
{
    uint64 v = 0;
    if(PHYSFS_readULE64(m_fileHandle, (PHYSFS_uint64*)&v) == 0)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
    return v;
}

void FileStream::addU8(uint8 v)
{
    if(PHYSFS_write(m_fileHandle, &v, 1, 1) != 1)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
}

void FileStream::addU16(uint8 v)
{
    if(PHYSFS_writeULE16(m_fileHandle, v) == 0)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
}

void FileStream::addU32(uint8 v)
{
    if(PHYSFS_writeULE32(m_fileHandle, v) == 0)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
}

void FileStream::addU64(uint8 v)
{
    if(PHYSFS_writeULE64(m_fileHandle, v) == 0)
        logTraceError("operation failed on '", m_name, "': ", PHYSFS_getLastError());
}
