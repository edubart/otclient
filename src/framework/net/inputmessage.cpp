/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#include "inputmessage.h"
#include <framework/util/crypt.h>

InputMessage::InputMessage()
{
    reset();
}

void InputMessage::reset()
{
    m_messageSize = 0;
    m_readPos = MAX_HEADER_SIZE;
    m_headerPos = MAX_HEADER_SIZE;
}

void InputMessage::setBuffer(const std::string& buffer)
{
    const int len = buffer.size();
    reset();
    checkWrite(len);
    memcpy(m_buffer + m_readPos, buffer.c_str(), len);
    m_readPos += len;
    m_messageSize += len;
}

uint8 InputMessage::getU8()
{
    checkRead(1);
    const uint8 v = m_buffer[m_readPos];
    m_readPos += 1;
    return v;
}

uint16 InputMessage::getU16()
{
    checkRead(2);
    const uint16 v = stdext::readULE16(m_buffer + m_readPos);
    m_readPos += 2;
    return v;
}

uint32 InputMessage::getU32()
{
    checkRead(4);
    const uint32 v = stdext::readULE32(m_buffer + m_readPos);
    m_readPos += 4;
    return v;
}

uint64 InputMessage::getU64()
{
    checkRead(8);
    const uint64 v = stdext::readULE64(m_buffer + m_readPos);
    m_readPos += 8;
    return v;
}

int64 InputMessage::get64()
{
    checkRead(8);
    const int64 v = stdext::readSLE64(m_buffer + m_readPos);
    m_readPos += 8;
    return v;
}

std::string InputMessage::getString()
{
    const uint16 stringLength = getU16();
    checkRead(stringLength);
    const auto* const v = (char*)(m_buffer + m_readPos);
    m_readPos += stringLength;
    return std::string(v, stringLength);
}

double InputMessage::getDouble()
{
    const uint8 precision = getU8();
    const int32 v = getU32() - INT_MAX;
    return (v / std::pow(10.f, precision));
}

bool InputMessage::decryptRsa(int size)
{
    checkRead(size);
    g_crypt.rsaDecrypt(static_cast<unsigned char*>(m_buffer) + m_readPos, size);
    return (getU8() == 0x00);
}

void InputMessage::fillBuffer(uint8* buffer, uint16 size)
{
    checkWrite(m_readPos + size);
    memcpy(m_buffer + m_readPos, buffer, size);
    m_messageSize += size;
}

void InputMessage::setHeaderSize(uint16 size)
{
    assert(MAX_HEADER_SIZE - size >= 0);
    m_headerPos = MAX_HEADER_SIZE - size;
    m_readPos = m_headerPos;
}

bool InputMessage::readChecksum()
{
    const uint32 receivedCheck = getU32();
    const uint32 checksum = stdext::adler32(m_buffer + m_readPos, getUnreadSize());
    return receivedCheck == checksum;
}

bool InputMessage::canRead(int bytes)
{
    if ((m_readPos - m_headerPos + bytes > m_messageSize) || (m_readPos + bytes > BUFFER_MAXSIZE))
        return false;
    return true;
}
void InputMessage::checkRead(int bytes)
{
    if (!canRead(bytes))
        throw stdext::exception("InputMessage eof reached");
}

void InputMessage::checkWrite(int bytes)
{
    if (bytes > BUFFER_MAXSIZE)
        throw stdext::exception("InputMessage max buffer size reached");
}
