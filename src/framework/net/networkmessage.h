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

#include <prerequisites.h>

class InputMessage
{
public:
    enum {
        INPUTMESSAGE_MAXSIZE = 16834,
        HEADER_LENGTH = 2
    };

    InputMessage() : m_messageSize(0), m_readPos(HEADER_LENGTH) { }
    ~InputMessage() { }

    inline void reset() {
        m_messageSize = 0;
        m_readPos = HEADER_LENGTH;
    }

    uint16_t decodeHeader() {
        return (int32_t)(m_buffer[0] | m_buffer[1] << 8);
    }

    uint8_t getByte() {
        return m_buffer[m_readPos++];
    }

    uint16_t getU16() {
        uint16_t v = *(uint16_t*)(m_buffer + m_readPos);
        m_readPos += 2;
        return v;
    }

    uint32_t getU32() {
        uint32_t v = *(uint32_t*)(m_buffer + m_readPos);
        m_readPos += 4;
        return v;
    }

    uint64_t getU64() {
        uint64_t v = *(uint64_t*)(m_buffer + m_readPos);
        m_readPos += 8;
        return v;
    }

    std::string getString() {
        uint16_t stringlen = getU16();
        if(stringlen >= (INPUTMESSAGE_MAXSIZE - m_readPos))
            return std::string();

        char* v = (char*)(m_buffer + m_readPos);
        m_readPos += stringlen;
        return std::string(v, stringlen);
    }

    void skipBytes(int count) { m_readPos += count; }

    int32_t getMessageLength() const {return m_messageSize; }
    void setMessageLength(int32_t newSize) { m_messageSize = newSize; }

    int32_t getReadPos() const { return m_readPos; }
    const char *getBuffer() const { return (char*)&m_buffer[0]; }

private:
    uint16_t m_messageSize;
    uint16_t m_readPos;
    uint8_t m_buffer[INPUTMESSAGE_MAXSIZE];
};

class OutputMessage
{
public:
    enum {
        OUTPUTMESSAGE_MAXSIZE = 1460
    };

    OutputMessage() : m_outputBufferStart(4), m_messageSize(0), m_writePos(4) { }
    ~OutputMessage() { }

    void reset() {
        m_messageSize = 0;
        m_writePos = 4;
        m_outputBufferStart = 4;
    }

    void addByte(uint8_t value)
    {
        if(!canAdd(1))
            return;
        m_buffer[m_writePos++] = value;
        m_messageSize++;
    }

    void addU16(uint16_t value)
    {
        if(!canAdd(2))
            return;
        *(uint16_t*)(m_buffer + m_writePos) = value;
        m_writePos += 2;
        m_messageSize += 2;
    }

    void addU32(uint32_t value)
    {
        if(!canAdd(4))
            return;
        *(uint32_t*)(m_buffer + m_writePos) = value;
        m_writePos += 4;
        m_messageSize += 4;
    }

    void addU64(uint64_t value)
    {
        if(!canAdd(8))
            return;
        *(uint64_t*)(m_buffer + m_writePos) = value;
        m_writePos += 8;
        m_messageSize += 8;
    }

    void addBytes(const char* bytes, uint32_t size)
    {
        if(!canAdd(size) || size > 8192)
            return;

        memcpy(m_buffer + m_writePos, bytes, size);
        m_writePos += size;
        m_messageSize += size;
    }

    void addPaddingBytes(uint32_t n)    {
        if(!canAdd(n))
            return;

        memset((void*)&m_buffer[m_writePos], 0x33, n);
        m_messageSize = m_messageSize + n;
    }

    void addString(const char* value)
    {
        uint32_t stringlen = (uint32_t)strlen(value);
        if(!canAdd(stringlen + 2) || stringlen > 8192)
            return;

        addU16(stringlen);
        strcpy((char*)(m_buffer + m_writePos), value);
        m_writePos += stringlen;
        m_messageSize += stringlen;
    }

    void addString(const std::string &value) {
        addString(value.c_str());
    }

    void writeMessageLength() {
        *(uint16_t*)(m_buffer + 2) = m_messageSize;
        m_messageSize += 2;
        m_outputBufferStart = 2;
    }

    void writeCryptoHeader() {
        *(uint16_t*)(m_buffer) = m_messageSize;
        m_messageSize += 2;
        m_outputBufferStart = 0;
    }

    int32_t getMessageLength() const { return m_messageSize; }
    void setMessageLength(int32_t newSize) { m_messageSize = newSize; }

    const char *getBuffer() const { return (char*)&m_buffer[0]; }
    const char *getOutputBuffer() const { return (char*)&m_buffer[m_outputBufferStart]; }

private:
    inline bool canAdd(int size) {
        return (size + m_writePos < OUTPUTMESSAGE_MAXSIZE);
    }

    uint16_t m_outputBufferStart;
    uint16_t m_messageSize;
    uint16_t m_writePos;
    uint8_t m_buffer[OUTPUTMESSAGE_MAXSIZE];
};

#endif //NETWORKMESSAGE_H 