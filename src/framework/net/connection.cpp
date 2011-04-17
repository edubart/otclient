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


#include <prerequisites.h>
#include <core/dispatcher.h>
#include <net/connection.h>

static boost::asio::io_service ioService;

Connection::Connection() :
    m_socket(ioService),
    m_resolver(ioService),
    m_writeError(false),
    m_readError(false),
    m_readTimer(ioService),
    m_writeTimer(ioService),
    m_state(STATE_CLOSED)
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
    ioService.post(boost::bind(&Connection::internalCloseConnection, shared_from_this()));
}

void Connection::internalCloseConnection()
{
    if(m_state != STATE_CLOSED) {
        m_pendingRead = 0;
        m_pendingWrite = 0;

        m_resolver.cancel();
        m_readTimer.cancel();
        m_writeTimer.cancel();

        g_dispatcher.addTask(m_closeCallback);

        if(m_socket.is_open()) {
            boost::system::error_code error;
            m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);

            if(error) {
                if(error == boost::asio::error::not_connected) {
                    //Transport endpoint is not connected.
                } else {
                    logError("shutdown socket error = %s", error.message());
                }
            }

            m_socket->close(error);

            if(error) {
                logError("close socket error = %s", error.message());
            }
        }
        m_state = STATE_CLOSED;
    }
}

bool Connection::connect(const std::string& host, uint16 port, const Callback& callback)
{
    logTrace();

    if(m_state != STATE_CLOSED) {
        logTraceError("connection not closed");
        return false;
    }

    m_connectCallback = callback;
    boost::asio::ip::tcp::resolver::query query(ip, convertType<std::string>(port));
    m_resolver.async_resolve(query, boost::bind(&Connection::onResolveDns, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::iterator));
    return true;
}

void Connection::onResolveDns(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt)
{
    logTrace();
    if(error) {
        handleError(error);
        return;
    } else {

    m_socket.async_connect(*endpointIt, boost::bind(&Connection::onConnect, shared_from_this(), boost::asio::placeholders::error));
}

void Connection::onConnect(const boost::system::error_code& error)
{
    logTrace();
    if(error) {
        handleError(error);
        return;
    }

    m_state = STATE_OPEN;

    if(m_connectCallback)
        g_dispatcher.addTask(m_connectCallback);

    recvNext();
}

void Connection::recvNext()
{
    logTrace();
    ++m_pendingRead;
    m_readTimer.expires_from_now(boost::posix_time::seconds(READ_TIMEOUT));
    m_readTimer.async_wait(boost::bind(&Connection::handleReadTimeout,
                                        boost::weak_ptr<Connection>(shared_from_this()),
                                        boost::asio::placeholders::error));

    static InputMessage inputMessage;
    boost::asio::async_read(*m_socket,
        boost::asio::buffer(inputMessage->getBuffer(), InputMessage::HEADER_LENGTH),
        boost::bind(&Connection::parseHeader, shared_from_this(), inputMessage, boost::asio::placeholders::error));
}

void Connection::parseHeader(const InputMessage& inputMessage, const boost::system::error_code& error)
{
    logTrace();

    --m_pendingRead;
    m_readTimer.cancel();

    if(error && !handleReadError(error))
        return;

    uint16_t size = inputMessage->decodeHeader();
    if(size <= 0 || size + 2 > InputMessage::INPUTMESSAGE_MAXSIZE) {
        internalCloseConnection();
        return;
    }

    try {
        ++m_pendingRead;
        m_readTimer.expires_from_now(boost::posix_time::seconds(Connection::read_timeout));
        m_readTimer.async_wait(boost::bind(&Connection::handleReadTimeout, boost::weak_ptr<Connection>(shared_from_this()),
            boost::asio::placeholders::error));

        inputMessage->setMessageLength(size + InputMessage::HEADER_LENGTH);
        boost::asio::async_read(*m_socket, boost::asio::buffer(inputMessage->getBuffer() + InputMessage::HEADER_LENGTH, size),
            boost::bind(&Connection::parsePacket, shared_from_this(), inputMessage, boost::asio::placeholders::error));
    } catch(boost::system::system_error& e) {
        logError("async read error = " << e.what());
        internalCloseConnection();
    }
}

void Connection::parsePacket(const InputMessage& inputMessage, const boost::system::error_code& error)
{
    logTrace();

    --m_pendingRead;
    m_readTimer.cancel();

    if(error && !handleReadError(error))
        return;

    //g_dispatcher.addTask(boost);
}

void Connection::handleError(const boost::system::error_code& error)
{
    logTrace();

    internalCloseConnection();
    m_errorCallback(error);
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
