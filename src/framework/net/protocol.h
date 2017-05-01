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

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "declarations.h"
#include "inputmessage.h"
#include "outputmessage.h"
#include "connection.h"

#include <framework/luaengine/luaobject.h>

// @bindclass
class Protocol : public LuaObject
{
public:
    Protocol();
    virtual ~Protocol();

    void connect(const std::string& host, uint16 port);
    void disconnect();

    bool isConnected();
    bool isConnecting();
    ticks_t getElapsedTicksSinceLastRead() { return m_connection ? m_connection->getElapsedTicksSinceLastRead() : -1; }

    ConnectionPtr getConnection() { return m_connection; }
    void setConnection(const ConnectionPtr& connection) { m_connection = connection; }

    void generateXteaKey();
    void setXteaKey(uint32 a, uint32 b, uint32 c, uint32 d);
    std::vector<uint32> getXteaKey();
    void enableXteaEncryption() { m_xteaEncryptionEnabled = true; }

    void enableChecksum() { m_checksumEnabled = true; }

    virtual void send(const OutputMessagePtr& outputMessage);
    virtual void recv();

    ProtocolPtr asProtocol() { return static_self_cast<Protocol>(); }

protected:
    virtual void onConnect();
    virtual void onRecv(const InputMessagePtr& inputMessage);
    virtual void onError(const boost::system::error_code& err);

    uint32 m_xteaKey[4];

private:
    void internalRecvHeader(uint8* buffer, uint16 size);
    void internalRecvData(uint8* buffer, uint16 size);

    bool xteaDecrypt(const InputMessagePtr& inputMessage);
    void xteaEncrypt(const OutputMessagePtr& outputMessage);

    bool m_checksumEnabled;
    bool m_xteaEncryptionEnabled;
    ConnectionPtr m_connection;
    InputMessagePtr m_inputMessage;
};

#endif
