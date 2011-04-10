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

#include <boost/bind.hpp>

Connection::Connection(boost::asio::io_service& ioService) 
    : m_socket(ioService), m_resolver(ioService)
{
    m_connected = false;
    m_connecting = false;
    m_port = 0;
}

void Connection::stop()
{
    if(m_connecting){
        m_resolver.cancel();
        m_socket.cancel();

        m_connecting = false;
    }
}

bool Connection::connect(const std::string& ip, uint16 port, ConnectionCallback onConnect)
{
    if(m_connecting){
        logError("Already is connecting.");
        return false;
    }

    if(m_connected){
        logError("Already is connected.");
        return false;
    }

    m_connectCallback = onConnect;
    m_connecting = true;
    m_ip = ip;
    m_port = port;

    //first resolve dns
    boost::asio::ip::tcp::resolver::query query(ip, convertType<std::string, uint16>(port));
    m_resolver.async_resolve(query, boost::bind(&Connection::onResolveDns, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));

    return true;
}

void Connection::onResolveDns(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt)
{
    if(error){
        m_connecting = false;
        m_errorCallback(error, __FUNCTION__);
        return;
    }

    //lets connect
    m_socket.async_connect(*endpointIt, boost::bind(&Connection::onConnect, this, boost::asio::placeholders::error));
}

void Connection::onConnect(const boost::system::error_code& error)
{
    if(error){
        m_connecting = false;
        m_errorCallback(error, __FUNCTION__);
        return;
    }

    m_connected = true;

    m_connectCallback();
}

void Connection::handleError(const boost::system::error_code& error)
{
    stop();

    if(isConnected()){
        closeSocket();
    }
}

void Connection::closeSocket()
{
    boost::system::error_code error;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);

    if(error) {
        logError("Connection::closeSocket(): %s", error.message().c_str());
    }

    m_socket.close(error);

    if(error) {
        logError("Connection::closeSocket(): %s", error.message().c_str());
    }
}

void Connection::send(NetworkMessagePtr networkMessage, ConnectionCallback onSend)
{
    boost::asio::async_write(m_socket,
        boost::asio::buffer(networkMessage->getBuffer(), NetworkMessage::header_length),
        boost::bind(&Connection::onSendHeader, shared_from_this(), networkMessage, onSend, boost::asio::placeholders::error));
}

void Connection::recv(RecvCallback onRecv)
{
    NetworkMessagePtr networkMessage(new NetworkMessage);

    boost::asio::async_read(m_socket,
        boost::asio::buffer(networkMessage->getBuffer(), NetworkMessage::header_length),
        boost::bind(&Connection::onRecvHeader, shared_from_this(), networkMessage, onRecv, boost::asio::placeholders::error));
}

void Connection::onRecvHeader(ConnectionPtr connection, NetworkMessagePtr networkMessage, RecvCallback onRecv, const boost::system::error_code& error)
{
    if(error){
        connection->handleError(error);
        connection->onError(error, __FUNCTION__);
        return;
    }

    boost::asio::async_read(connection->getSocket(),
        boost::asio::buffer(networkMessage->getBodyBuffer(), networkMessage->getMessageLength()),
        boost::bind(&Connection::onRecvBody, connection, networkMessage, onRecv, boost::asio::placeholders::error));
}

void Connection::onRecvBody(ConnectionPtr connection, NetworkMessagePtr networkMessage, RecvCallback onRecv, const boost::system::error_code& error)
{
    if(error){
        connection->handleError(error);
        connection->onError(error, __FUNCTION__);
        return;
    }

    onRecv(networkMessage);
}

void Connection::onSendHeader(ConnectionPtr connection, NetworkMessagePtr networkMessage, ConnectionCallback onSend, const boost::system::error_code& error)
{
    if(error){
        connection->handleError(error);
        connection->onError(error, __FUNCTION__);
        return;
    }

    boost::asio::async_write(connection->getSocket(),
        boost::asio::buffer(networkMessage->getBodyBuffer(), networkMessage->getMessageLength()),
        boost::bind(&Connection::onSendBody, connection, networkMessage, onSend, boost::asio::placeholders::error));
}

void Connection::onSendBody(ConnectionPtr connection, NetworkMessagePtr networkMessage, ConnectionCallback onSend, const boost::system::error_code& error)
{
    if(error){
        connection->handleError(error);
        connection->onError(error, __FUNCTION__);
        return;
    }

    onSend();
}
