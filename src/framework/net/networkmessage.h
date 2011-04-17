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

#ifndef NETWORKMESSAGE_H
#define NETWORKMESSAGE_H

#include "prerequisites.h"

class Rsa;

class NetworkMessage
{
public:
    enum {
        header_length = 2,
        NETWORKMESSAGE_MAXSIZE = 1500
    };

    enum {
        max_body_length = NETWORKMESSAGE_MAXSIZE - header_length
    };

    // constructor/destructor
    NetworkMessage() {
        reset();
    }

    // resets the internal buffer to an empty message
protected:
    void reset() {
        m_msgSize = 0;
        m_readPos = 2;
    }
public:
    // simply read functions for incoming message
    uint8 getByte();
    uint16 getU16();
    uint32 getU32();
    uint64 getU64();

    std::string getString();
    std::string getRaw();

    // skips count unknown/unused bytes in an incoming message
    void skipBytes(int count);

    // simply write functions for outgoing message
    void addByte(uint8 value);
    void addU16(uint16 value);
    void addU32(uint32 value);
    void addU64(uint64 value);
    void addBytes(const char* bytes, uint32_t size);
    void addPaddingBytes(uint32 n);
    void addString(const std::string &value);
    void addString(const char* value);
    int32 getMessageLength() const;

    void setMessageLength(int32 newSize);
    int32 getReadPos() const;
    int32 getHeaderSize();
    char* getBuffer() const;
    char* getBodyBuffer() const;

    void updateHeaderLength();

protected:
    inline bool canAdd(int size);

    int32 m_msgSize;
    int32 m_readPos;

    uint8 m_msgBuf[NETWORKMESSAGE_MAXSIZE];
};

#endif //NETWORKMESSAGE_H 