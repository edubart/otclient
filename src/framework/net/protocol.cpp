/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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
    m_connection->setErrorCallback([proto = asProtocol()] (auto error) {
        proto->onError(error);
    });
    m_connection->connect(host, port, [proto = asProtocol()] () {
        proto->onConnect();
    });
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
        m_connection->read(2, [proto = asProtocol()] (auto buffer, auto size) {
            proto->internalRecvHeader(buffer, size);
        });
}

void Protocol::internalRecvHeader(uint8* buffer, uint16 size)
{
    // read message size
    m_inputMessage->fillBuffer(buffer, size);
    uint16 remainingSize = m_inputMessage->readSize();

    // read remaining message data
    if(m_connection)
        m_connection->read(remainingSize, [proto = asProtocol()] (auto buffer, auto size) {
            proto->internalRecvData(buffer, size);
        });
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
    std::random_device rd;
    std::uniform_int_distribution<uint32> unif;
    std::generate(m_xteaKey.begin(), m_xteaKey.end(), [&]() { return unif(rd); });
}

namespace {

constexpr uint32_t delta = 0x9E3779B9;

template<typename Round>
void apply_rounds(uint8_t* data, size_t length, Round round)
{
    for (auto j = 0u; j < length; j += 8) {
        uint32_t left = data[j+0] | data[j+1] << 8u | data[j+2] << 16u | data[j+3] << 24u,
                right = data[j+4] | data[j+5] << 8u | data[j+6] << 16u | data[j+7] << 24u;

        round(left, right);

        data[j] = static_cast<uint8_t>(left);
        data[j+1] = static_cast<uint8_t>(left >> 8u);
        data[j+2] = static_cast<uint8_t>(left >> 16u);
        data[j+3] = static_cast<uint8_t>(left >> 24u);
        data[j+4] = static_cast<uint8_t>(right);
        data[j+5] = static_cast<uint8_t>(right >> 8u);
        data[j+6] = static_cast<uint8_t>(right >> 16u);
        data[j+7] = static_cast<uint8_t>(right >> 24u);
    }
}

}

bool Protocol::xteaDecrypt(const InputMessagePtr& inputMessage)
{
    uint16 encryptedSize = inputMessage->getUnreadSize();
    if(encryptedSize % 8 != 0) {
        g_logger.traceError("invalid encrypted network message");
        return false;
    }

    for (uint32_t i = 0, sum = delta << 5, next_sum = sum - delta; i < 32; ++i, sum = next_sum, next_sum -= delta) {
        apply_rounds(inputMessage->getReadBuffer(), encryptedSize, [&](uint32_t& left, uint32_t& right) {
            right -= ((left << 4 ^ left >> 5) + left) ^ (sum + m_xteaKey[(sum >> 11) & 3]);
            left -= ((right << 4 ^ right >> 5) + right) ^ (next_sum + m_xteaKey[next_sum & 3]);
        });
    };

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

    for (uint32_t i = 0, sum = 0, next_sum = sum + delta; i < 32; ++i, sum = next_sum, next_sum += delta) {
        apply_rounds(outputMessage->getDataBuffer() - 2, encryptedSize, [&](uint32_t& left, uint32_t& right) {
            left += ((right << 4 ^ right >> 5) + right) ^ (sum + m_xteaKey[sum & 3]);
            right += ((left << 4 ^ left >> 5) + left) ^ (next_sum + m_xteaKey[(next_sum >> 11) & 3]);
        });
    };
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
