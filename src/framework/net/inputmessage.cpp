#include "inputmessage.h"

InputMessage::InputMessage()
{
    reset();
}

void InputMessage::reset()
{
    m_readPos = 0;
    m_messageSize = 2;
}

uint8 InputMessage::getU8()
{
    assert(canRead(1));
    return m_buffer[m_readPos++];
}

uint16 InputMessage::getU16()
{
    assert(canRead(2));
    uint16 v = *(uint16_t*)(m_buffer + m_readPos);
    m_readPos += 2;
    return v;
}

uint32 InputMessage::getU32()
{
    assert(canRead(4));
    uint32 v = *(uint32*)(m_buffer + m_readPos);
    m_readPos += 4;
    return v;
}

uint64 InputMessage::getU64()
{
    assert(canRead(8));
    uint64 v = *(uint64*)(m_buffer + m_readPos);
    m_readPos += 8;
    return v;
}

std::string InputMessage::getString()
{
    uint16 stringLength = getU16();
    assert(canRead(stringLength));
    char* v = (char*)(m_buffer + m_readPos);
    m_readPos += stringLength;
    return std::string(v, stringLength);
}

bool InputMessage::canRead(int bytes)
{
    if((m_readPos + bytes > m_messageSize) || (m_readPos + bytes > BUFFER_MAXSIZE))
        return false;
    return true;
}
