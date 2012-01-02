/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include "declarations.h"
#include <boost/asio.hpp>

class Connection : public std::enable_shared_from_this<Connection>, boost::noncopyable
{
    typedef std::function<void(boost::system::error_code&)> ErrorCallback;
    typedef std::function<void(uint8*, uint16)> RecvCallback;

    enum {
        READ_TIMEOUT = 30,
        WRITE_TIMEOUT = 30,
        RECV_BUFFER_SIZE = 65536
    };

public:
    Connection();

    static void init();
    static void poll();
    static void terminate();

    void connect(const std::string& host, uint16 port, const SimpleCallback& connectCallback);
    void close();

    void write(uint8* buffer, uint16 size);
    void read(uint16 bytes, const RecvCallback& callback);
    void read_some(const RecvCallback& callback);

    void setErrorCallback(const ErrorCallback& errorCallback) { m_errorCallback = errorCallback; }

    bool isConnecting() const { return m_connecting; }
    bool isConnected() const { return m_connected; }

protected:
    void onResolve(const boost::system::error_code& error, asio::ip::tcp::resolver::iterator endpointIterator);
    void onConnect(const boost::system::error_code& error);
    void onWrite(const boost::system::error_code& error, size_t);
    void onRecv(const boost::system::error_code& error, size_t recvSize);
    void onTimeout(const boost::system::error_code& error);
    void handleError(const boost::system::error_code& error);

    SimpleCallback m_connectCallback;
    ErrorCallback m_errorCallback;
    RecvCallback m_recvCallback;

    asio::deadline_timer m_readTimer;
    asio::deadline_timer m_writeTimer;
    asio::ip::tcp::resolver m_resolver;
    asio::ip::tcp::socket m_socket;

    uint8 m_recvBuffer[RECV_BUFFER_SIZE];
    bool m_connected;
    bool m_connecting;

    friend class Server;
};

#endif
