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

#include <core/dispatcher.h>
#include <net/connection.h>

static boost::asio::io_service ioService;

Connection::Connection() :
        m_connectionState(CONNECTION_STATE_IDLE),
        m_timer(ioService),
        m_resolver(ioService),
        m_socket(ioService)
{
}

void Connection::poll()
{
    ioService.poll();
    ioService.reset();
}

void Connection::connect(const std::string& host, uint16 port, const SimpleCallback& connectCallback)
{
    m_connectCallback = connectCallback;
    m_connectionState = CONNECTION_STATE_RESOLVING;

    boost::asio::ip::tcp::resolver::query query(host, convert_cast<std::string>(port));
    m_resolver.async_resolve(query, boost::bind(&Connection::onResolve, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::iterator));

    m_timer.expires_from_now(boost::posix_time::seconds(2));
    m_timer.async_wait(boost::bind(&Connection::onTimeout, shared_from_this(), boost::asio::placeholders::error));
}

void Connection::send(OutputMessage *outputMessage)
{
    boost::asio::async_write(m_socket,
                             boost::asio::buffer(outputMessage->getBuffer(), outputMessage->getMessageSize()),
                             boost::bind(&Connection::onSend, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    m_timer.expires_from_now(boost::posix_time::seconds(2));
    m_timer.async_wait(boost::bind(&Connection::onTimeout, shared_from_this(), boost::asio::placeholders::error));
}

void Connection::onTimeout(const boost::system::error_code& error)
{
    if(error != boost::asio::error::operation_aborted)
        g_dispatcher.addTask(boost::bind(m_errorCallback, error));
}

void Connection::onResolve(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIterator)
{
    logTrace();

    m_timer.cancel();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addTask(boost::bind(m_errorCallback, error));
        return;
    }

    m_socket.async_connect(*endpointIterator, boost::bind(&Connection::onConnect, shared_from_this(), boost::asio::placeholders::error));

    m_timer.expires_from_now(boost::posix_time::seconds(2));
    m_timer.async_wait(boost::bind(&Connection::onTimeout, shared_from_this(), boost::asio::placeholders::error));
}

void Connection::onConnect(const boost::system::error_code& error)
{
    logTrace();

    m_timer.cancel();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addTask(boost::bind(m_errorCallback, error));
        return;
    }

    g_dispatcher.addTask(m_connectCallback);

    // Start listening.
    InputMessage *inputMessage = new InputMessage;
    boost::asio::async_read(m_socket,
                            boost::asio::buffer(inputMessage->getBuffer(), InputMessage::HEADER_LENGTH),
                            boost::bind(&Connection::onRecvHeader, shared_from_this(), boost::asio::placeholders::error, inputMessage));
}

void Connection::onSend(const boost::system::error_code& error, size_t)
{
    logTrace();

    m_timer.cancel();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addTask(boost::bind(m_errorCallback, error));
        return;
    }
}

void Connection::onRecvHeader(const boost::system::error_code& error, InputMessage *inputMessage)
{
    logTrace();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addTask(boost::bind(m_errorCallback, error));
        return;
    }

    uint16 messageSize = inputMessage->getU16();
    inputMessage->setMessageSize(messageSize);

    boost::asio::async_read(m_socket,
                            boost::asio::buffer(inputMessage->getBuffer() + InputMessage::CHECKSUM_POS, messageSize),
                            boost::bind(&Connection::onRecvData, shared_from_this(), boost::asio::placeholders::error, inputMessage));
}

void Connection::onRecvData(const boost::system::error_code& error, InputMessage *inputMessage)
{
    logTrace();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addTask(boost::bind(m_errorCallback, error));
        return;
    }

    // call callback
    if(m_recvCallback)
        g_dispatcher.addTask(boost::bind(m_recvCallback, inputMessage));

    // FIXME:
    // TODO declare inside class? call onRecvHeader.
    // this needs a remake
    /*delete inputMessage;

    inputMessage = new InputMessage;
    boost::asio::async_read(m_socket,
                            boost::asio::buffer(inputMessage->getBuffer(), InputMessage::HEADER_LENGTH),
                            boost::bind(&Connection::onRecvHeader, shared_from_this(), boost::asio::placeholders::error, inputMessage));*/

}
