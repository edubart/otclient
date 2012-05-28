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
    m_cacheReadPos = 0;
}

FileStream::~FileStream()
{
    close();
}

void FileStream::cache()
{
    if(!m_fileHandle)
        logTraceError("no file handle to cache");

    // cache entire file into cache buffer
    m_cacheReadPos = PHYSFS_tell(m_fileHandle);
    PHYSFS_seek(m_fileHandle, 0);
    int size = PHYSFS_fileLength(m_fileHandle);
    m_cacheBuffer.resize(size);
    int res = PHYSFS_read(m_fileHandle, &m_cacheBuffer[0], size, 1);
    if(res == -1)
        logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());

    PHYSFS_close(m_fileHandle);
    m_fileHandle = nullptr;
}

bool FileStream::close()
{
    if(m_fileHandle) {
        if(PHYSFS_isInit() && PHYSFS_close(m_fileHandle) == 0)
            logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());

        m_fileHandle = nullptr;
        return true;
    } else {
        m_cacheBuffer.clear();
        m_cacheReadPos = 0;
        return true;
    }
}

bool FileStream::flush()
{
    if(!m_fileHandle)
        return false;

    if(PHYSFS_flush(m_fileHandle) == 0) {
        logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
        return false;
    }
    return true;
}

int FileStream::read(void *buffer, int size, int nmemb)
{
    if(m_fileHandle) {
        int res = PHYSFS_read(m_fileHandle, buffer, size, nmemb);
        if(res == -1) {
            logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
            return 0;
        }
        return res;
    } else {
        uint maxReadPos = m_cacheBuffer.size()-1;
        int writePos = 0;
        uint8 *outBuffer = (uint8*)buffer;
        for(int i=0;i<nmemb;++i) {
            if(m_cacheReadPos+size > maxReadPos)
                return i;

            for(int j=0;j<size;++j)
                outBuffer[writePos++] = m_cacheBuffer[m_cacheReadPos++];
        }
        return nmemb;
    }
}

bool FileStream::write(void *buffer, int count)
{
    if(!m_fileHandle)
        return false;

    if(PHYSFS_write(m_fileHandle, buffer, 1, count) != count) {
        logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
        return false;
    }

    return true;
}

bool FileStream::seek(int pos)
{
    if(m_fileHandle) {
        if(PHYSFS_seek(m_fileHandle, pos) == 0) {
            logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
            return false;
        }
    } else {
        if(pos > (int)m_cacheBuffer.size() || pos < 0) {
            logTraceError("operation failed on '%s': seek pos cannot be greater than file length", m_name);
            return false;
        }
        m_cacheReadPos = pos;
    }
    return true;
}

int FileStream::size()
{
    if(m_fileHandle) 
        return PHYSFS_fileLength(m_fileHandle);
    else
        return m_cacheBuffer.size();
}

int FileStream::tell()
{
    if(m_fileHandle)
        return PHYSFS_tell(m_fileHandle);
    else
        return m_cacheReadPos;
}

uint8 FileStream::getU8()
{
    uint8 v = 0;
    if(m_fileHandle) {
        if(PHYSFS_read(m_fileHandle, &v, 1, 1) != 1)
            logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
    } else {
        if(m_cacheReadPos+1 > m_cacheBuffer.size()) {
            logTraceError("operation failed on '%s': reached file eof", m_name);
            return 0;
        }

        v = m_cacheBuffer[m_cacheReadPos];
        m_cacheReadPos += 1;
    }
    return v;
}

uint16 FileStream::getU16()
{
    uint16 v = 0;
    if(m_fileHandle) {
        if(PHYSFS_readULE16(m_fileHandle, &v) == 0)
            logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
    } else {
        if(m_cacheReadPos+2 > m_cacheBuffer.size()) {
            logTraceError("operation failed on '%s': reached file eof", m_name);
            return 0;
        }

        v = stdext::readLE16(&m_cacheBuffer[m_cacheReadPos]);
        m_cacheReadPos += 2;
    }
    return v;
}

uint32 FileStream::getU32()
{
    uint32 v = 0;
    if(m_fileHandle) {
        if(PHYSFS_readULE32(m_fileHandle, &v) == 0)
            logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
    } else {
        if(m_cacheReadPos+4 > m_cacheBuffer.size()) {
            logTraceError("operation failed on '%s': reached file eof", m_name);
            return 0;
        }

        v = stdext::readLE32(&m_cacheBuffer[m_cacheReadPos]);
        m_cacheReadPos += 4;
    }
    return v;
}

uint64 FileStream::getU64()
{
    uint64 v = 0;
    if(m_fileHandle) {
        if(PHYSFS_readULE64(m_fileHandle, (PHYSFS_uint64*)&v) == 0)
            logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
    } else {
        if(m_cacheReadPos+8 > m_cacheBuffer.size()) {
            logTraceError("operation failed on '%s': reached file eof", m_name);
            return 0;
        }

        v = stdext::readLE64(&m_cacheBuffer[m_cacheReadPos]);
        m_cacheReadPos += 8;
    }
    return v;
}

std::string FileStream::getString()
{
    std::string str;
    int len = getU16();
    if(len > 0 && len < 8192) {
        char buffer[8192];
        if(m_fileHandle) {
            if(PHYSFS_read(m_fileHandle, buffer, 1, len) == 0)
                logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
            else
                str = std::string(buffer, len);
        } else {
            if(m_cacheReadPos+len > m_cacheBuffer.size()) {
                logTraceError("operation failed on '%s': reached file eof", m_name);
                return 0;
            }

            str = std::string((char*)&m_cacheBuffer[m_cacheReadPos], len);
            m_cacheReadPos += len;
        }
    } else {
        logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
    }
    return str;
}

void FileStream::addU8(uint8 v)
{
    if(PHYSFS_write(m_fileHandle, &v, 1, 1) != 1)
        logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
}

void FileStream::addU16(uint8 v)
{
    if(PHYSFS_writeULE16(m_fileHandle, v) == 0)
        logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
}

void FileStream::addU32(uint8 v)
{
    if(PHYSFS_writeULE32(m_fileHandle, v) == 0)
        logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
}

void FileStream::addU64(uint8 v)
{
    if(PHYSFS_writeULE64(m_fileHandle, v) == 0)
        logTraceError("operation failed on '%s': %s", m_name, PHYSFS_getLastError());
}
