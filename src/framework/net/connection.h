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

#include "../prerequisites.h"

#include <boost/asio.hpp>

#include "networkmessage.h"

class TestState;
class Protocol;
class Connections;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    typedef std::function<void(const boost::system::error_code&)> ConnectionCallback;
    typedef std::shared_ptr<Connection> ConnectionPtr;

private:
    Connection(boost::asio::io_service& ioService);

    bool connect(const std::string& ip, uint16 port, ConnectionCallback onConnect);
    void stop();
    void send(NetworkMessagePtr networkMessage, ConnectionCallback onSend);

    bool isConnecting() const { return m_connecting; }
    bool isConnected() const { return m_connected; }

    boost::asio::ip::tcp::socket& getSocket() { return m_socket; }

private:
    static void onSendHeader(ConnectionPtr connection, NetworkMessagePtr networkMessage, ConnectionCallback onSend, const boost::system::error_code& error);
    static void onSendBody(ConnectionPtr connection, NetworkMessagePtr networkMessage, ConnectionCallback onSend, const boost::system::error_code& error);

private:
    void onResolveDns(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt);
    void onConnect(const boost::system::error_code& error);

private:
    void closeSocket();

private:
    void handleError(const boost::system::error_code& error);

    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::resolver m_resolver;

    bool m_connecting;
    bool m_connected;

    std::string m_ip;
    uint16_t m_port;

    ConnectionCallback m_connectCallback;

    friend class Protocol;
    friend class Connections;
};

typedef std::shared_ptr<Connection> ConnectionPtr;

#endif //CONNECTION_h
