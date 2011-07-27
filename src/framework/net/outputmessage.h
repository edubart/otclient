#ifndef OUTPUTMESSAGE_H
#define OUTPUTMESSAGE_H

#include "netdeclarations.h"

class OutputMessage
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
    void setWritePos(uint16 writePos) { m_writePos = writePos; }

private:
    bool canWrite(int bytes);

    uint16 m_writePos;
    uint16 m_messageSize;
    uint8 m_buffer[BUFFER_MAXSIZE];
};

#endif
