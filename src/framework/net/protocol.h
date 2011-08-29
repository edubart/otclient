/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#include <framework/luascript/luaobject.h>

class Protocol : public LuaObject
{
public:
    Protocol();

    void connect(const std::string& host, uint16 port);
    void disconnect();

    bool isConnected();
    bool isConnecting();

    void send(OutputMessage& outputMessage);
    void recv();

    void internalRecvHeader(uint8* buffer, uint16 size);
    void internalRecvData(uint8* buffer, uint16 size);

    virtual void onConnect() = 0;
    virtual void onRecv(InputMessage& inputMessage) = 0;
    virtual void onError(const boost::system::error_code& err) = 0;

    ProtocolPtr asProtocol() { return std::static_pointer_cast<Protocol>(shared_from_this()); }

protected:
    void enableChecksum() { m_checksumEnabled = true; }
    void enableXteaEncryption() { m_xteaEncryptionEnabled = true; }
    void generateXteaKey();

    uint32 m_xteaKey[4];
    InputMessage m_inputMessage;

private:
    bool xteaDecrypt(InputMessage& inputMessage);
    void xteaEncrypt(OutputMessage& outputMessage);
    uint32 getAdlerChecksum(uint8* buffer, uint16 size);

    bool m_checksumEnabled;
    bool m_xteaEncryptionEnabled;
    ConnectionPtr m_connection;
};

#endif
