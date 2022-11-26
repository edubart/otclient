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

#include "protocolhttp.h"
#include "connection.h"
#include <framework/core/application.h>

ProtocolHttp::ProtocolHttp()
{
}

ProtocolHttp::~ProtocolHttp()
{
#ifndef NDEBUG
    assert(!g_app.isTerminated());
#endif
    disconnect();
}

void ProtocolHttp::connect(const std::string& host, uint16 port)
{
    m_connection = ConnectionPtr(new Connection);
    m_connection->setErrorCallback([proto = asProtocolHttp()] (auto error) {
        proto->onError(error);
    });
    m_connection->connect(host, port, [proto = asProtocolHttp()] () {
        proto->onConnect();
    });
}

void ProtocolHttp::disconnect()
{
    if(m_connection) {
        m_connection->close();
        m_connection.reset();
    }
}

void ProtocolHttp::send(const std::string& message)
{
    if(m_connection)
        m_connection->write((uint8*)message.c_str(), message.length());
}

void ProtocolHttp::recv()
{
    if(m_connection)
        m_connection->read_until("\r\n\r\n", [proto = asProtocolHttp()] (auto buffer, auto size) {
            proto->onRecv(buffer, size);
        });
}

void ProtocolHttp::onConnect()
{
    callLuaField("onConnect");
}

void ProtocolHttp::onRecv(uint8* buffer, uint16 size)
{
    std::string string = std::string((char*)buffer, (size_t)size);
    callLuaField("onRecv", string);
}

void ProtocolHttp::onError(const boost::system::error_code& err)
{
    callLuaField("onError", err.message(), err.value());
    disconnect();
}
