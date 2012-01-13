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

#ifndef OUTPUTMESSAGE_H
#define OUTPUTMESSAGE_H

#include "declarations.h"
#include "networkexception.h"
#include <framework/luascript/luaobject.h>

typedef std::shared_ptr<OutputMessage> OutputMessagePtr;

class OutputMessage : public LuaObject
{
public:
    enum {
        BUFFER_MAXSIZE = 1024,
        HEADER_POS = 0,
        HEADER_LENGTH = 2,
        CHECKSUM_POS = 2,
        CHECKSUM_LENGTH = 4,
        DATA_POS = 6
    };

    OutputMessage();

    void reset();

    void addU8(uint8 value);
    void addU16(uint16 value);
    void addU32(uint32 value);
    void addU64(uint64 value);
    void addString(const char* value);
    void addString(const std::string &value);
    void addPaddingBytes(int bytes, uint8 byte = 0);

    uint8* getBuffer() { return m_buffer; }
    uint16 getMessageSize() { return m_messageSize; }
    void setMessageSize(uint16 messageSize) { m_messageSize = messageSize; }
    void setWritePos(uint16 writePos) { m_writePos = writePos; }

private:
    bool canWrite(int bytes);
    void checkWrite(int bytes);

    uint16 m_writePos;
    uint16 m_messageSize;
    uint8 m_buffer[BUFFER_MAXSIZE];
};

#endif
