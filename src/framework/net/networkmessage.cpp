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

#include "networkmessage.h"

void NetworkMessage::updateHeaderLength()
{
    uint16 size = m_msgSize;
    memcpy(m_msgBuf, &size, 2);
}

bool NetworkMessage::canAdd(int size) {
    return (size + m_readPos < NETWORKMESSAGE_MAXSIZE - 16);
}

std::string NetworkMessage::getString()
{
    uint16 stringlen = getU16();
    if(stringlen >= (16384 - m_readPos))
        return std::string();

    char* v = (char*)(m_msgBuf + m_readPos);
    m_readPos += stringlen;
    return std::string(v, stringlen);
}

std::string NetworkMessage::getRaw()
{
    uint16 stringlen = m_msgSize - m_readPos;
    if(stringlen >= (16384 - m_readPos))
        return std::string();

    char* v = (char*)(m_msgBuf + m_readPos);
    m_readPos += stringlen;
    return std::string(v, stringlen);
}

void NetworkMessage::addString(const char* value)
{
    uint32 stringlen = (uint32)strlen(value);
    if(!canAdd(stringlen + 2) || stringlen > 8192)
        return;

    addU16(stringlen);
    strcpy((char*)(m_msgBuf + m_readPos), value);
    m_readPos += stringlen;
    m_msgSize += stringlen;
}

void NetworkMessage::addBytes(const char* bytes, uint32 size)
{
    if(!canAdd(size) || size > 8192)
        return;

    memcpy(m_msgBuf + m_readPos, bytes, size);
    m_readPos += size;
    m_msgSize += size;
}

void NetworkMessage::addPaddingBytes(uint32 n)
{
    if(!canAdd(n))
        return;

    memset((void*)&m_msgBuf[m_readPos], 0x33, n);
    m_msgSize = m_msgSize + n;
}

void NetworkMessage::skipBytes(int count) {
    m_readPos += count;
}

// simply write functions for outgoing message
void NetworkMessage::addByte(uint8 value) {
    if(!canAdd(1))
        return;

    m_msgBuf[m_readPos++] = value;
    m_msgSize++;
}

void NetworkMessage::addU16(uint16 value) {
    if(!canAdd(2))
        return;

    *(uint16*)(m_msgBuf + m_readPos) = value;
    m_readPos += 2;
    m_msgSize += 2;
}

void NetworkMessage::addU32(uint32 value) {
    if(!canAdd(4))
        return;

    *(uint32*)(m_msgBuf + m_readPos) = value;
    m_readPos += 4;
    m_msgSize += 4;
}

void NetworkMessage::addU64(uint64 value) {
    if(!canAdd(8))
        return;

    *(uint64*)(m_msgBuf + m_readPos) = value;
    m_readPos += 8;
    m_msgSize += 8;
}

void NetworkMessage::addString(const std::string &value) {
    addString(value.c_str());
}

int32 NetworkMessage::getMessageLength() const {
    return m_msgSize;
}

void NetworkMessage::setMessageLength(int32 newSize) {
    m_msgSize = newSize;
}

int32 NetworkMessage::getReadPos() const {
    return m_readPos;
}

uint8 NetworkMessage::getByte()
{
    return m_msgBuf[m_readPos++];
}

uint16 NetworkMessage::getU16()
{
    uint16 v = *(uint16*)(m_msgBuf + m_readPos);
    m_readPos += 2;
    return v;
}

uint32 NetworkMessage::getU32()
{
    uint32 v = *(uint32*)(m_msgBuf + m_readPos);
    m_readPos += 4;
    return v;
}

uint64 NetworkMessage::getU64()
{
    uint64 v = *(uint64*)(m_msgBuf + m_readPos);
    m_readPos += 8;
    return v;
}

char* NetworkMessage::getBuffer() {
    return (char*)&m_msgBuf[0];
}

char* NetworkMessage::getBodyBuffer() {
    m_readPos = 2;
    return (char*)&m_msgBuf[header_length];
}
