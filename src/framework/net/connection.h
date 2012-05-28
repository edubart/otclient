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
#include <framework/core/timer.h>
#include <framework/core/declarations.h>

class Connection : public std::enable_shared_from_this<Connection>, boost::noncopyable
{
    typedef std::function<void(const boost::system::error_code&)> ErrorCallback;
    typedef std::function<void(uint8*, uint16)> RecvCallback;

    enum {
        READ_TIMEOUT = 30,
        WRITE_TIMEOUT = 30,
        SEND_INTERVAL = 1,
        SEND_BUFFER_SIZE = 65536,
        RECV_BUFFER_SIZE = 65536
    };

public:
    Connection();
    ~Connection();

    static void poll();
    static void terminate();

    void connect(const std::string& host, uint16 port, const std::function<void()>& connectCallback);
    void close();

    void write(uint8* buffer, uint16 size);
    void read(uint16 bytes, const RecvCallback& callback);
    void read_some(const RecvCallback& callback);

    void setErrorCallback(const ErrorCallback& errorCallback) { m_errorCallback = errorCallback; }

    boost::system::error_code getError() const { return m_error; }
    bool isConnecting() const { return m_connecting; }
    bool isConnected() const { return m_connected; }

protected:
    void onConnect(const boost::system::error_code& error);
    void onWrite(const boost::system::error_code& error, size_t);
    void onRecv(const boost::system::error_code& error, size_t recvSize);
    void onTimeout(const boost::system::error_code& error);
    void handleError(const boost::system::error_code& error);

    std::function<void()> m_connectCallback;
    ErrorCallback m_errorCallback;
    RecvCallback m_recvCallback;

    asio::deadline_timer m_readTimer;
    asio::deadline_timer m_writeTimer;
    asio::ip::tcp::resolver m_resolver;
    asio::ip::tcp::socket m_socket;

    uint8 m_sendBuffer[SEND_BUFFER_SIZE];
    uint8 m_recvBuffer[RECV_BUFFER_SIZE];
    bool m_connected;
    bool m_connecting;
    boost::system::error_code m_error;
    int m_sendBufferSize;
    Timer m_sendTimer;
    ScheduledEventPtr m_sendEvent;

    friend class Server;
};

#endif
