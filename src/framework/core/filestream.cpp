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
#include <framework/application.h>

#include <physfs.h>

FileStream::FileStream(const std::string& name, PHYSFS_File *fileHandle)
{
    m_name = name;
    m_pos = 0;
    m_fileHandle = fileHandle;
}

FileStream::~FileStream()
{
    assert(!g_app.isTermianted());
    close();
}

void FileStream::cache()
{
    if(!m_fileHandle)
        return;

    // cache entire file into data buffer
    m_pos = PHYSFS_tell(m_fileHandle);
    PHYSFS_seek(m_fileHandle, 0);
    int size = PHYSFS_fileLength(m_fileHandle);
    m_data.resize(size);
    if(PHYSFS_read(m_fileHandle, &m_data[0], size, 1) == -1)
        throwError("unable to read file data", true);

    close();
}

void FileStream::close()
{
    if(m_fileHandle) {
        if(!PHYSFS_close(m_fileHandle))
            throwError("close failed", true);
        m_fileHandle = nullptr;
    } else {
        m_data.clear();
        m_pos = 0;
    }
}

void FileStream::flush()
{
    if(m_fileHandle && PHYSFS_flush(m_fileHandle) == 0)
        throwError("flush failed", true);
}

int FileStream::read(void *buffer, uint32 size, uint32 nmemb)
{
    if(m_fileHandle) {
        int res = PHYSFS_read(m_fileHandle, buffer, size, nmemb);
        if(res == -1)
            throwError("read failed", true);
        return res;
    } else {
        uint maxReadPos = m_data.size()-1;
        int writePos = 0;
        uint8 *outBuffer = (uint8*)buffer;
        for(uint i=0;i<nmemb;++i) {
            if(m_pos+size > maxReadPos)
                return i;

            for(uint j=0;j<size;++j)
                outBuffer[writePos++] = m_data[m_pos++];
        }
        return nmemb;
    }
}

void FileStream::write(const void *buffer, uint32 count)
{
    if(PHYSFS_write(m_fileHandle, buffer, 1, count) != count)
        throwError("write failed", true);
}

void FileStream::seek(uint32 pos)
{
    if(m_fileHandle) {
        if(!PHYSFS_seek(m_fileHandle, pos))
            throwError("seek failed", true);
    } else {
        if(pos > m_data.size())
            throwError("seek failed");
        m_pos = pos;
    }
}

void FileStream::skip(uint len)
{
    seek(tell() + len);
}

int FileStream::size()
{
    if(m_fileHandle)
        return PHYSFS_fileLength(m_fileHandle);
    else
        return m_data.size();
}

int FileStream::tell()
{
    if(m_fileHandle)
        return PHYSFS_tell(m_fileHandle);
    else
        return m_pos;
}

uint8 FileStream::getU8()
{
    uint8 v = 0;
    if(m_fileHandle) {
        if(PHYSFS_read(m_fileHandle, &v, 1, 1) != 1)
            throwError("read failed", true);
    } else {
        if(m_pos+1 > m_data.size())
            throwError("read failed");

        v = m_data[m_pos];
        m_pos += 1;
    }
    return v;
}

uint16 FileStream::getU16()
{
    uint16 v = 0;
    if(m_fileHandle) {
        if(PHYSFS_readULE16(m_fileHandle, &v) == 0)
            throwError("read failed", true);
    } else {
        if(m_pos+2 > m_data.size())
            throwError("read failed");

        v = stdext::readLE16(&m_data[m_pos]);
        m_pos += 2;
    }
    return v;
}

uint32 FileStream::getU32()
{
    uint32 v = 0;
    if(m_fileHandle) {
        if(PHYSFS_readULE32(m_fileHandle, &v) == 0)
            throwError("read failed", true);
    } else {
        if(m_pos+4 > m_data.size())
            throwError("read failed");

        v = stdext::readLE32(&m_data[m_pos]);
        m_pos += 4;
    }
    return v;
}

uint64 FileStream::getU64()
{
    uint64 v = 0;
    if(m_fileHandle) {
        if(PHYSFS_readULE64(m_fileHandle, (PHYSFS_uint64*)&v) == 0)
            throwError("read failed", true);
    } else {
        if(m_pos+8 > m_data.size())
            throwError("read failed");
        v = stdext::readLE64(&m_data[m_pos]);
        m_pos += 8;
    }
    return v;
}

std::string FileStream::getString()
{
    std::string str;
    uint16 len = getU16();
    if(len > 0 && len < 8192) {
        char buffer[8192];
        if(m_fileHandle) {
            if(PHYSFS_read(m_fileHandle, buffer, 1, len) == 0)
                throwError("read failed", true);
            else
                str = std::string(buffer, len);
        } else {
            if(m_pos+len > m_data.size()) {
                throwError("read failed");
                return 0;
            }

            str = std::string((char*)&m_data[m_pos], len);
            m_pos += len;
        }
    } else if(len != 0)
        throwError("read failed because string is too big");
    return str;
}

void FileStream::addU8(uint8 v)
{
    if(PHYSFS_write(m_fileHandle, &v, 1, 1) != 1)
        throwError("write failed", true);
}

void FileStream::addU16(uint16 v)
{
    if(PHYSFS_writeULE16(m_fileHandle, v) == 0)
        throwError("write failed", true);
}

void FileStream::addU32(uint32 v)
{
    if(PHYSFS_writeULE32(m_fileHandle, v) == 0)
        throwError("write failed", true);
}

void FileStream::addU64(uint64 v)
{
    if(PHYSFS_writeULE64(m_fileHandle, v) == 0)
        throwError("write failed", true);
}

void FileStream::addString(const std::string& v)
{
    addU16(v.length());
    write(v.c_str(), v.length());
}

void FileStream::throwError(const std::string& message, bool physfsError)
{
    std::string completeMessage = stdext::format("in file '%s': %s", m_name, message);
    if(physfsError)
        completeMessage += std::string(": ") + PHYSFS_getLastError();
    stdext::throw_exception(completeMessage);
}
