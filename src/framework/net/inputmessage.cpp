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


#include <net/inputmessage.h>

InputMessage::InputMessage()
{
    reset();
}

void InputMessage::reset()
{
    m_readPos = 0;
    m_messageSize = 0;
}

uint8 InputMessage::getU8()
{
    if(!canRead(1))
        return 0;

    return m_buffer[m_readPos++];
}

uint16 InputMessage::getU16()
{
    if(!canRead(2))
        return 0;

    uint16 v = *(uint16_t*)(m_buffer + m_readPos);
    m_readPos += 2;
    return v;
}

uint32 InputMessage::getU32()
{
    if(!canRead(4))
        return 0;

    uint32 v = *(uint32*)(m_buffer + m_readPos);
    m_readPos += 4;
    return v;
}

uint64 InputMessage::getU64()
{
    if(!canRead(8))
        return 0;

    uint64 v = *(uint64*)(m_buffer + m_readPos);
    m_readPos += 8;
    return v;
}

std::string InputMessage::getString()
{
    uint16 stringLength = getU16();
    if(!canRead(stringLength))
        return std::string();

    char* v = (char*)(m_buffer + m_readPos);
    m_readPos += stringLength;
    return std::string(v, stringLength);
}

bool InputMessage::canRead(int bytes)
{
    return (m_readPos + bytes <= m_messageSize) && (m_readPos + bytes <= BUFFER_MAXSIZE);
}
