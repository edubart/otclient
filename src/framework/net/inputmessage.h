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

#ifndef INPUTMESSAGE_H
#define INPUTMESSAGE_H

#include "declarations.h"
#include "networkexception.h"

class InputMessage
{
public:
    enum {
        BUFFER_MAXSIZE = 16384,
        MAX_HEADER_SIZE = 8,
    };

    InputMessage();

    void reset();

    void fillBuffer(uint8 *buffer, uint16 size);
    uint16 readSize() { return getU16(); }
    bool readChecksum();

    void setHeaderSize(uint16 size);
    void setMessageSize(uint16 size) { m_messageSize = size; }

    void skipBytes(uint16 bytes) { m_readPos += bytes; }

    uint8 getU8(bool peek = false);
    uint16 getU16(bool peek = false);
    uint32 getU32(bool peek = false);
    uint64 getU64(bool peek = false);
    std::string getString();

    uint8* getReadBuffer() { return m_buffer + m_readPos; }
    uint8* getHeaderBuffer() { return m_buffer + m_headerPos; }
    uint8* getDataBuffer() { return m_buffer + MAX_HEADER_SIZE; }
    uint16 getHeaderSize() { return (MAX_HEADER_SIZE - m_headerPos); }
    uint16 getMessageSize() { return m_messageSize; }
    int getReadSize() { return m_readPos - m_headerPos; }
    int getUnreadSize() { return m_messageSize - (m_readPos - m_headerPos); }

    bool eof() { return (m_readPos - m_headerPos) >= m_messageSize; }

private:
    bool canRead(int bytes);
    void checkRead(int bytes);

    uint16 m_headerPos;
    uint16 m_readPos;
    uint16 m_messageSize;
    uint8 m_buffer[BUFFER_MAXSIZE];
};

#endif
