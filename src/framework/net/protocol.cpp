#include "protocol.h"
#include "connection.h"

Protocol::Protocol() :
        m_connection(new Connection)
{
    m_connection->setErrorCallback(std::bind(&Protocol::onError, this, std::placeholders::_1));
    m_xteaEncryptionEnabled = false;
    m_checksumEnabled = true;
}

void Protocol::connect(const std::string& host, uint16 port)
{
    m_connection->connect(host, port, std::bind(&Protocol::onConnect, asProtocol()));
}

void Protocol::send(OutputMessage& outputMessage)
{
    // Encrypt
    if(m_xteaEncryptionEnabled)
        xteaEncrypt(outputMessage);

    // Set checksum
    uint32 checksum = getAdlerChecksum(outputMessage.getBuffer() + OutputMessage::DATA_POS, outputMessage.getMessageSize());
    outputMessage.setWritePos(OutputMessage::CHECKSUM_POS);
    outputMessage.addU32(checksum);

    // Set size
    uint16 messageSize = outputMessage.getMessageSize();
    outputMessage.setWritePos(OutputMessage::HEADER_POS);
    outputMessage.addU16(messageSize);

    // Send
    m_connection->send(outputMessage.getBuffer(), outputMessage.getMessageSize());
}

void Protocol::recv()
{
    m_inputMessage.reset();

    m_connection->recv(InputMessage::HEADER_LENGTH, 2, std::bind(&Protocol::internalRecvHeader, asProtocol(), std::placeholders::_1, std::placeholders::_2));
}

void Protocol::internalRecvHeader(uint8* buffer, uint16 size)
{
    memcpy(m_inputMessage.getBuffer() + InputMessage::HEADER_POS, buffer, size);

    uint16 dataSize = m_inputMessage.getU16();
    m_inputMessage.setMessageSize(dataSize);

    m_connection->recv(dataSize, 5, std::bind(&Protocol::internalRecvData, asProtocol(), std::placeholders::_1, std::placeholders::_2));
}

void Protocol::internalRecvData(uint8* buffer, uint16 size)
{
    memcpy(m_inputMessage.getBuffer() + InputMessage::CHECKSUM_POS, buffer, size);

    if(m_checksumEnabled) {
        uint32 checksum = getAdlerChecksum(m_inputMessage.getBuffer() + InputMessage::DATA_POS, m_inputMessage.getMessageSize() - InputMessage::CHECKSUM_LENGTH);
        if(m_inputMessage.getU32() != checksum) {
            // error
            logError("Checksum is invalid.");
            return;
        }
    }

    if(m_xteaEncryptionEnabled)
        xteaDecrypt(m_inputMessage);

    onRecv(m_inputMessage);
}

void Protocol::onError(const boost::system::error_code& err)
{
    std::stringstream message;

    logError("PROTOCOL ERROR: ", err.message());
    message << "Boost error: " << err.message();

    // invalid hostname
    // connection timeouted

    // displays a dialog, finish protocol

    callField("onError", message.str());
}

bool Protocol::xteaDecrypt(InputMessage& inputMessage)
{
    // FIXME: this function has not been tested yet
    uint16 messageSize = inputMessage.getMessageSize() - InputMessage::CHECKSUM_LENGTH;
    if(messageSize % 8 != 0) {
        logDebug("not valid encrypted message size");
        return false;
    }

    uint32 *buffer = (uint32*)(inputMessage.getBuffer() + InputMessage::DATA_POS);
    int readPos = 0;

    while(readPos < messageSize/4) {
        uint32 v0 = buffer[readPos], v1 = buffer[readPos + 1];
        uint32 delta = 0x61C88647;
        uint32 sum = 0xC6EF3720;

        for(int32 i = 0; i < 32; i++) {
            v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + m_xteaKey[sum>>11 & 3]);
            sum += delta;
            v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + m_xteaKey[sum & 3]);
        }
        buffer[readPos] = v0; buffer[readPos + 1] = v1;
        readPos = readPos + 2;
    }

    int tmp = inputMessage.getU16();
    if(tmp > inputMessage.getMessageSize() - 4) {
        logDebug("not valid unencrypted message size");
        return false;
    }

    inputMessage.setMessageSize(tmp + InputMessage::UNENCRYPTED_DATA_POS);
    return true;
}

void Protocol::xteaEncrypt(OutputMessage& outputMessage)
{
    uint16 messageLength = outputMessage.getMessageSize();

    //add bytes until reach 8 multiple
    if((messageLength % 8) != 0) {
        uint16 n = 8 - (messageLength % 8);
        outputMessage.addPaddingBytes(n);
        messageLength += n;
    }

    int readPos = 0;
    uint32 *buffer = (uint32*)outputMessage.getBuffer() + OutputMessage::DATA_POS;
    while(readPos < messageLength / 4) {
        uint32 v0 = buffer[readPos], v1 = buffer[readPos + 1];
        uint32 delta = 0x61C88647;
        uint32 sum = 0;

        for(int32 i = 0; i < 32; i++) {
            v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + m_xteaKey[sum & 3]);
            sum -= delta;
            v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + m_xteaKey[sum>>11 & 3]);
        }
        buffer[readPos] = v0; buffer[readPos + 1] = v1;
        readPos = readPos + 2;
    }
}

uint32 Protocol::getAdlerChecksum(uint8* buffer, uint16 size)
{
    uint32 a = 1, b = 0;
    while (size > 0) {
        size_t tlen = size > 5552 ? 5552 : size;
        size -= tlen;
        do {
            a += *buffer++;
            b += a;
        } while (--tlen);

        a %= 65521;
        b %= 65521;
    }

    return (b << 16) | a;
}
