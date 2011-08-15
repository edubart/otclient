#include <framework/net/outputmessage.h>

OutputMessage::OutputMessage()
{
    reset();
}

void OutputMessage::reset()
{
    m_writePos = DATA_POS;
    m_messageSize = 0;
}

void OutputMessage::addU8(uint8 value)
{
    assert(canWrite(1));
    m_buffer[m_writePos] = value;
    m_writePos += 1;
    m_messageSize += 1;
}

void OutputMessage::addU16(uint16 value)
{
    assert(canWrite(2));
    *(uint16_t*)(m_buffer + m_writePos) = value;
    m_writePos += 2;
    m_messageSize += 2;
}

void OutputMessage::addU32(uint32 value)
{
    assert(canWrite(4));
    *(uint32*)(m_buffer + m_writePos) = value;
    m_writePos += 4;
    m_messageSize += 4;
}

void OutputMessage::addU64(uint64 value)
{
    assert(canWrite(8));
    *(uint64*)(m_buffer + m_writePos) = value;
    m_writePos += 8;
    m_messageSize += 8;
}

void OutputMessage::addString(const char* value)
{
    size_t stringLength = strlen(value);
    assert(stringLength < 0xFFFF && canWrite(stringLength + 2));
    addU16(stringLength);
    strcpy((char*)(m_buffer + m_writePos), value);
    m_writePos += stringLength;
    m_messageSize += stringLength;
}

void OutputMessage::addString(const std::string &value)
{
    addString(value.c_str());
}

void OutputMessage::addPaddingBytes(int bytes, uint8 byte)
{
    assert(canWrite(bytes) && bytes >= 0);
    memset((void*)&m_buffer[m_writePos], byte, bytes);
    m_writePos += bytes;
    m_messageSize += bytes;
}

bool OutputMessage::canWrite(int bytes)
{
    if(m_writePos + bytes > BUFFER_MAXSIZE)
        return false;
    return true;
}
