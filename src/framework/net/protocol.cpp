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


#include <net/protocol.h>

#include <boost/bind.hpp>

Protocol::Protocol() :
        m_connection(new Connection)
{
    m_connection->setErrorCallback(boost::bind(&Protocol::onError, this, _1));
    m_connection->setRecvCallback(boost::bind(&Protocol::onRecv, this, _1));
    m_xteaEncryptionEnabled = false;
}

void Protocol::connect(const std::string& host, uint16 port, const boost::function<void()>& callback)
{
    m_connection->connect(host, port, callback);
}

void Protocol::send(OutputMessage *outputMessage)
{
    // Encrypt
    if(m_xteaEncryptionEnabled)
        xteaEncrypt(outputMessage);

    // Set checksum
    uint32 checksum = getAdlerChecksum(outputMessage->getBuffer() + OutputMessage::DATA_POS, outputMessage->getMessageSize());
    outputMessage->setWritePos(OutputMessage::CHECKSUM_POS);
    outputMessage->addU32(checksum);

    // Set size
    uint16 messageSize = outputMessage->getMessageSize();
    outputMessage->setWritePos(OutputMessage::HEADER_POS);
    outputMessage->addU16(messageSize);

    // Send
    m_connection->send(outputMessage);
}

void Protocol::onRecv(InputMessage *inputMessage)
{
    uint32 checksum = getAdlerChecksum(inputMessage->getBuffer() + InputMessage::DATA_POS, inputMessage->getMessageSize() - InputMessage::CHECKSUM_LENGTH);
    if(inputMessage->getU32() != checksum) {
        // error
        error("Checksum is invalid.");
        return;
    }

    if(m_xteaEncryptionEnabled)
        xteaDecrypt(inputMessage);
}

void Protocol::onError(const boost::system::error_code& err)
{
    error("PROTOCOL ERROR: ", err.message());

    // invalid hostname
    // connection timeouted

    // displays a dialog, finish protocol
}

bool Protocol::xteaDecrypt(InputMessage *inputMessage)
{
    // FIXME: this function has not been tested yet
    uint16 messageSize = inputMessage->getMessageSize() - InputMessage::CHECKSUM_LENGTH;
    if(messageSize % 8 != 0) {
        //LOG_TRACE_DEBUG("not valid encrypted message size")
        return false;
    }

    uint32 *buffer = (uint32*)(inputMessage->getBuffer() + InputMessage::DATA_POS);
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

    int tmp = inputMessage->getU16();
    if(tmp > inputMessage->getMessageSize() - 4) {
        //LOG_TRACE_DEBUG("not valid unencrypted message size")
        return false;
    }

    inputMessage->setMessageSize(tmp + InputMessage::UNENCRYPTED_DATA_POS);
    return true;
}

void Protocol::xteaEncrypt(OutputMessage *outputMessage)
{
    uint16 messageLength = outputMessage->getMessageSize();

    //add bytes until reach 8 multiple
    if((messageLength % 8) != 0) {
        uint16 n = 8 - (messageLength % 8);
        outputMessage->addPaddingBytes(n);
        messageLength += n;
    }

    int readPos = 0;
    uint32 *buffer = (uint32*)outputMessage->getBuffer() + OutputMessage::DATA_POS;
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

uint32 Protocol::getAdlerChecksum(uint8 *buffer, uint16 size)
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

