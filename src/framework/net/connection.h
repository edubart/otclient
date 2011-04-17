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

#include "prerequisites.h"
#include "networkmessage.h"
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
    Connection();
    ~Connection();

    bool connect(const std::string& ip, uint16 port, const Callback& callback);
    void close();

    void setOnError(const ErrorCallback& callback) { m_errorCallback = callback; }

    void recv(const RecvCallback& onSend);
    void send(const NetworkMessage& networkMessage, const ConnectionCallback& onRecv);

    bool isConnecting() const { return m_connecting; }
    bool isConnected() const { return m_connected; }

    static void poll();

private:
    void onSendHeader(const NetworkMessage& networkMessage, const ConnectionCallback& onSend, const boost::system::error_code& error);
    void onSendBody(const NetworkMessage& networkMessage, const ConnectionCallback& onSend, const boost::system::error_code& error);

    void onRecvHeader(const NetworkMessage& networkMessage, const RecvCallback& onRecv, const boost::system::error_code& error);
    void onRecvBody(const NetworkMessage& networkMessage, const RecvCallback& onRecv, const boost::system::error_code& error);

    void onResolveDns(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt);
    void onConnect(const boost::system::error_code& error);

    void closeSocket();
    void handleError(const boost::system::error_code& error);

    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::resolver m_resolver;

    bool m_connecting;
    bool m_connected;

    std::string m_ip;
    uint16_t m_port;

    Callback m_connectCallback;
    ErrorCallback m_errorCallback;
};

#endif //CONNECTION_h
