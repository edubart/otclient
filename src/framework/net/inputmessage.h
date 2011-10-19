/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

class InputMessage
{
public:
    enum {
        BUFFER_MAXSIZE = 16384,
        HEADER_POS = 0,
        HEADER_LENGTH = 2,
        CHECKSUM_POS = 2,
        CHECKSUM_LENGTH = 4,
        DATA_POS = 6,
        UNENCRYPTED_DATA_POS = 8
    };

    InputMessage();

    void reset();

    uint8 getU8(bool peek = false);
    uint16 getU16(bool peek = false);
    uint32 getU32(bool peek = false);
    uint64 getU64(bool peek = false);
    std::string getString();

    void skipBytes(uint16 bytes) { m_readPos += bytes; }
    uint8* getBuffer() { return m_buffer; }
    uint16 getMessageSize() { return m_messageSize; }
    void setMessageSize(uint16 messageSize) { m_messageSize = messageSize; }
    bool eof() { return m_readPos >= m_messageSize; }

private:
    bool canRead(int bytes);

    uint16 m_readPos;
    uint16 m_messageSize;
    uint8 m_buffer[BUFFER_MAXSIZE];
};

#endif
