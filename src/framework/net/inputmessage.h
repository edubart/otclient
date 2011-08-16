#ifndef INPUTMESSAGE_H
#define INPUTMESSAGE_H

#include "declarations.h"

class InputMessage
{
public:
    enum {
        BUFFER_MAXSIZE = 4096,
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
    bool eof() { return m_readPos == m_messageSize; }

private:
    bool canRead(int bytes);

    uint16 m_readPos;
    uint16 m_messageSize;
    uint8 m_buffer[BUFFER_MAXSIZE];
};

#endif
