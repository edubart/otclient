/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#include "protocol.h"
#include "connection.h"
#include <framework/core/application.h>
#include <random>

Protocol::Protocol()
{
    m_xteaEncryptionEnabled = false;
    m_checksumEnabled = false;
    m_inputMessage = InputMessagePtr(new InputMessage);
}

Protocol::~Protocol()
{
#ifndef NDEBUG
    assert(!g_app.isTerminated());
#endif
    disconnect();
}

void Protocol::connect(const std::string& host, uint16 port)
{
    m_connection = ConnectionPtr(new Connection);
    m_connection->setErrorCallback(std::bind(&Protocol::onError, asProtocol(), std::placeholders::_1));
    m_connection->connect(host, port, std::bind(&Protocol::onConnect, asProtocol()));
}

void Protocol::disconnect()
{
    if(m_connection) {
        m_connection->close();
        m_connection.reset();
    }
}

bool Protocol::isConnected()
{
    if(m_connection && m_connection->isConnected())
        return true;
    return false;
}

bool Protocol::isConnecting()
{
    if(m_connection && m_connection->isConnecting())
        return true;
    return false;
}

void Protocol::send(const OutputMessagePtr& outputMessage)
{
    // encrypt
    if(m_xteaEncryptionEnabled)
        xteaEncrypt(outputMessage);

    // write checksum
    if(m_checksumEnabled)
        outputMessage->writeChecksum();

    // write message size
    outputMessage->writeMessageSize();

    // send
    if(m_connection)
        m_connection->write(outputMessage->getHeaderBuffer(), outputMessage->getMessageSize());

    // reset message to allow reuse
    outputMessage->reset();
}

void Protocol::recv()
{
    m_inputMessage->reset();

    // first update message header size
    int headerSize = 2; // 2 bytes for message size
    if(m_checksumEnabled)
        headerSize += 4; // 4 bytes for checksum
    if(m_xteaEncryptionEnabled)
        headerSize += 2; // 2 bytes for XTEA encrypted message size
    m_inputMessage->setHeaderSize(headerSize);

    // read the first 2 bytes which contain the message size
    if(m_connection)
        m_connection->read(2, std::bind(&Protocol::internalRecvHeader, asProtocol(), std::placeholders::_1,  std::placeholders::_2));
}

void Protocol::internalRecvHeader(uint8* buffer, uint16 size)
{
    // read message size
    m_inputMessage->fillBuffer(buffer, size);
    uint16 remainingSize = m_inputMessage->readSize();

    // read remaining message data
    if(m_connection)
        m_connection->read(remainingSize, std::bind(&Protocol::internalRecvData, asProtocol(), std::placeholders::_1,  std::placeholders::_2));
}

void Protocol::internalRecvData(uint8* buffer, uint16 size)
{
    // process data only if really connected
    if(!isConnected()) {
        g_logger.traceError("received data while disconnected");
        return;
    }

    m_inputMessage->fillBuffer(buffer, size);

    if(m_checksumEnabled && !m_inputMessage->readChecksum()) {
        g_logger.traceError("got a network message with invalid checksum");
        return;
    }

    if(m_xteaEncryptionEnabled) {
        if(!xteaDecrypt(m_inputMessage)) {
            g_logger.traceError("failed to decrypt message");
            return;
        }
    }
    onRecv(m_inputMessage);
}

void Protocol::generateXteaKey()
{
    std::mt19937 eng(std::time(NULL));
    std::uniform_int_distribution<uint32> unif(0, 0xFFFFFFFF);
    m_xteaKey[0] = unif(eng);
    m_xteaKey[1] = unif(eng);
    m_xteaKey[2] = unif(eng);
    m_xteaKey[3] = unif(eng);
}

void Protocol::setXteaKey(uint32 a, uint32 b, uint32 c, uint32 d)
{
    m_xteaKey[0] = a;
    m_xteaKey[1] = b;
    m_xteaKey[2] = c;
    m_xteaKey[3] = d;
}

std::vector<uint32> Protocol::getXteaKey()
{
    std::vector<uint32> xteaKey;
    xteaKey.resize(4);
    for(int i = 0; i < 4; ++i)
        xteaKey[i] = m_xteaKey[i];
    return xteaKey;
}

bool Protocol::xteaDecrypt(const InputMessagePtr& inputMessage)
{
    uint16 encryptedSize = inputMessage->getUnreadSize();
    if(encryptedSize % 8 != 0) {
        g_logger.traceError("invalid encrypted network message");
        return false;
    }

    uint32 *buffer = (uint32*)(inputMessage->getReadBuffer());
    int readPos = 0;

    while(readPos < encryptedSize/4) {
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

    uint16 decryptedSize = inputMessage->getU16() + 2;
    int sizeDelta = decryptedSize - encryptedSize;
    if(sizeDelta > 0 || -sizeDelta > encryptedSize) {
        g_logger.traceError("invalid decrypted network message");
        return false;
    }

    inputMessage->setMessageSize(inputMessage->getMessageSize() + sizeDelta);
    return true;
}

void Protocol::xteaEncrypt(const OutputMessagePtr& outputMessage)
{
    outputMessage->writeMessageSize();
    uint16 encryptedSize = outputMessage->getMessageSize();

    //add bytes until reach 8 multiple
    if((encryptedSize % 8) != 0) {
        uint16 n = 8 - (encryptedSize % 8);
        outputMessage->addPaddingBytes(n);
        encryptedSize += n;
    }

    int readPos = 0;
    uint32 *buffer = (uint32*)(outputMessage->getDataBuffer() - 2);
    while(readPos < encryptedSize / 4) {
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

void Protocol::onConnect()
{
    callLuaField("onConnect");
}

void Protocol::onRecv(const InputMessagePtr& inputMessage)
{
    callLuaField("onRecv", inputMessage);
}

void Protocol::onError(const boost::system::error_code& err)
{
    callLuaField("onError", err.message(), err.value());
    disconnect();
}
