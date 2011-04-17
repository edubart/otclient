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

#include "connection.h"

static boost::asio::io_service ioService;

Connection::Connection() :
    m_socket(ioService),
    m_resolver(ioService),
    m_connecting(false),
    m_connected(false),
    m_port(0)
{
    logTrace();
}

Connection::~Connection()
{
    logTrace();
}

void Connection::poll()
{
    ioService.poll();
    ioService.reset();
}

void Connection::close()
{
    logTrace();
    if(m_connecting) {
        m_resolver.cancel();
        m_socket.cancel();
        m_connecting = false;
        m_connected = false;
        closeSocket();
    }
}

bool Connection::connect(const std::string& ip, uint16 port, const Callback& callback)
{
    logTrace();

    if(m_connecting) {
        logTraceError("already connecting.");
        return false;
    }

    if(m_connected) {
        logTraceError("already connected.");
        return false;
    }

    m_connectCallback = callback;
    m_connecting = true;
    m_ip = ip;
    m_port = port;

    boost::asio::ip::tcp::resolver::query query(ip, convertType<std::string, uint16>(port));
    m_resolver.async_resolve(query, boost::bind(&Connection::onResolveDns, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::iterator));
    return true;
}

void Connection::onResolveDns(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt)
{
    logTrace();
    if(error) {
        handleError(error);
        return;
    }

    //lets connect
    m_socket.async_connect(*endpointIt, boost::bind(&Connection::onConnect, shared_from_this(), boost::asio::placeholders::error));
}

void Connection::onConnect(const boost::system::error_code& error)
{
    logTrace();
    if(error) {
        handleError(error);
        return;
    }
    m_connected = true;
    m_connectCallback();
}

void Connection::handleError(const boost::system::error_code& error)
{
    logTrace();

    close();
    m_errorCallback(error);
}

void Connection::closeSocket()
{
    logTrace();

    boost::system::error_code error;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
    m_socket.close(error);
}

void Connection::send(const NetworkMessage& networkMessage, const ConnectionCallback& onSend)
{
    logTrace();

    boost::asio::async_write(m_socket,
        boost::asio::buffer(networkMessage.getBuffer(), NetworkMessage::header_length),
        boost::bind(&Connection::onSendHeader, shared_from_this(), networkMessage, onSend, boost::asio::placeholders::error));
}

void Connection::recv(const RecvCallback& onRecv)
{
    logTrace();

    static NetworkMessage networkMessage;
    boost::asio::async_read(m_socket,
        boost::asio::buffer(networkMessage.getBuffer(), NetworkMessage::header_length),
        boost::bind(&Connection::onRecvHeader, shared_from_this(), networkMessage, onRecv, boost::asio::placeholders::error));
}

void Connection::onRecvHeader(const NetworkMessage& networkMessage, const RecvCallback& onRecv, const boost::system::error_code& error)
{
    logTrace();

    if(error) {
        handleError(error);
        return;
    }

    boost::asio::async_read(m_socket,
        boost::asio::buffer(networkMessage.getBodyBuffer(), networkMessage.getMessageLength()),
        boost::bind(&Connection::onRecvBody, shared_from_this(), networkMessage, onRecv, boost::asio::placeholders::error));
}

void Connection::onRecvBody(const NetworkMessage& networkMessage, const RecvCallback& onRecv, const boost::system::error_code& error)
{
    logTrace();

    if(error){
        handleError(error);
        return;
    }

    onRecv(networkMessage);
}

void Connection::onSendHeader(const NetworkMessage& networkMessage, const ConnectionCallback& onSend, const boost::system::error_code& error)
{
    logTrace();

    if(error){
        handleError(error);
        return;
    }

    boost::asio::async_write(m_socket,
        boost::asio::buffer(networkMessage.getBodyBuffer(), networkMessage.getMessageLength()),
        boost::bind(&Connection::onSendBody, shared_from_this(), networkMessage, onSend, boost::asio::placeholders::error));
}

void Connection::onSendBody(const NetworkMessage& networkMessage, const ConnectionCallback& onSend, const boost::system::error_code& error)
{
    logTrace();

    if(error) {
        handleError(error);
        return;
    }

    onSend();
}
