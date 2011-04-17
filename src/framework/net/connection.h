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


#ifndef CONNECTION_H
#define CONNECTION_H

#include <prerequisites.h>
#include <net/networkmessage.h>

#include <boost/asio.hpp>

class Protocol;
class Connection;
typedef boost::shared_ptr<Connection> ConnectionPtr;

typedef boost::function<void()> ConnectionCallback;
typedef boost::function<void(const NetworkMessage&)> RecvCallback;
typedef boost::function<void(const boost::system::error_code&)> ErrorCallback;

class Connection : public boost::enable_shared_from_this<Connection>
{
public:
    enum {
        WRITE_TIMEOUT = 10,
        READ_TIMEOUT = 10
    };

    enum EConnectionState {
        STATE_CONNECTING,
        STATE_OPEN,
        STATE_CLOSED
    }

    Connection();
    ~Connection();

    bool connect(const std::string& host, uint16 port, const Callback& callback);
    void close();

    void setOnError(const ErrorCallback& callback) { m_errorCallback = callback; }
    void setOnRecv(const RecvCallback& callback) { m_recvCallback = callback; }

    void send(const OutputMessage& networkMessage);

    bool isConnecting() const { return m_state == STATE_CONNECTING; }
    bool isConnected() const { return m_state == STATE_OPEN; }

    static void poll();

private:
    void onResolveDns(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt);
    void onConnect(const boost::system::error_code& error);

    void recvNext();

    void onRecvBody(const NetworkMessage& networkMessage, const RecvCallback& onRecv, const boost::system::error_code& error);

    void onSendHeader(const NetworkMessage& networkMessage, const ConnectionCallback& onSend, const boost::system::error_code& error);
    void onSendBody(const NetworkMessage& networkMessage, const ConnectionCallback& onSend, const boost::system::error_code& error);

    void onRecvHeader(const NetworkMessage& networkMessage, const RecvCallback& onRecv, const boost::system::error_code& error);

    void handleError(const boost::system::error_code& error);
    void internalCloseConnection();

    boost::asio::ip::tcp::resolver m_resolver;
    boost::asio::ip::tcp::socket m_socket;

    int32_t m_pendingWrite;
    int32_t m_pendingRead;
    bool m_writeError;
    bool m_readError;
    boost::asio::deadline_timer m_readTimer;
    boost::asio::deadline_timer m_writeTimer;

    EConnectionState m_state;

    Callback m_connectCallback;
    Callback m_closeCallback;
    ErrorCallback m_errorCallback;
    RecvCallback m_recvCallback;
};

#endif //CONNECTION_h
