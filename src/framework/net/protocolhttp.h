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

#ifndef PROTOCOLHTTP_H
#define PROTOCOLHTTP_H

#include "declarations.h"
#include "connection.h"

#include <framework/luaengine/luaobject.h>

// @bindclass
class ProtocolHttp : public LuaObject
{
public:
    ProtocolHttp();
    virtual ~ProtocolHttp();

    void connect(const std::string& host, uint16 port);
    void disconnect();

    void send(const std::string &message);
    void recv();

    ProtocolHttpPtr asProtocolHttp() { return static_self_cast<ProtocolHttp>(); }

protected:
    void onConnect();
    void onRecv(uint8* buffer, uint16 size);
    void onError(const boost::system::error_code& err);

private:
    ConnectionPtr m_connection;
};

#endif
