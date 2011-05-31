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

#include <net/inputmessage.h>
#include <net/outputmessage.h>
#include <prerequisites.h>
#include <boost/asio.hpp>

typedef boost::function<void(boost::system::error_code&)> ErrorCallback;
typedef boost::function<void(InputMessage*)> RecvCallback;

class Connection : public boost::enable_shared_from_this<Connection>, boost::noncopyable
{
public:
    Connection();

    static void poll();

    void connect(const std::string& host, uint16 port, const SimpleCallback& connectCallback);
    void send(OutputMessage *outputMessage);

    void setErrorCallback(const ErrorCallback& errorCallback) { m_errorCallback = errorCallback; }
    void setRecvCallback(const RecvCallback& recvCallback) { m_recvCallback = recvCallback; }

    void onTimeout(const boost::system::error_code& error);
    void onResolve(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIterator);
    void onConnect(const boost::system::error_code& error);
    void onSend(const boost::system::error_code& error, size_t);
    void onRecvHeader(const boost::system::error_code& error);
    void onRecvData(const boost::system::error_code& error);

    enum ConnectionState_t {
        CONNECTION_STATE_IDLE = 0,
        CONNECTION_STATE_RESOLVING,
        CONNECTION_STATE_CONNECTING,
        CONNECTION_STATE_CONNECTED
    };

private:
    ErrorCallback m_errorCallback;
    RecvCallback m_recvCallback;
    SimpleCallback m_connectCallback;
    ConnectionState_t m_connectionState;

    boost::asio::deadline_timer m_timer;
    boost::asio::ip::tcp::resolver m_resolver;
    boost::asio::ip::tcp::socket m_socket;
    InputMessage m_inputMessage;

};

typedef boost::shared_ptr<Connection> ConnectionPtr;

#endif
