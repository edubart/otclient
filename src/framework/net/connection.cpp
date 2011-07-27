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

void Connection::connect(const std::string& host, uint16 port, const std::function<void()>& connectCallback)
{
    m_connectCallback = connectCallback;

    boost::asio::ip::tcp::resolver::query query(host, convert<std::string>(port));
    m_resolver.async_resolve(query, std::bind(&Connection::onResolve, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

    m_timer.expires_from_now(boost::posix_time::seconds(2));
    m_timer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), std::placeholders::_1));
}

void Connection::send(uint8 *buffer, uint16 size)
{
    boost::asio::async_write(m_socket,
                             boost::asio::buffer(buffer, size),
                             std::bind(&Connection::onSend, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

    m_timer.expires_from_now(boost::posix_time::seconds(2));
    m_timer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), std::placeholders::_1));
}

void Connection::recv(uint16 bytes, uint32 timeout, const RecvCallback& callback)
{
    m_recvCallback = callback;
    m_recvSize = bytes;

    boost::asio::async_read(m_socket,
                            boost::asio::buffer(m_recvBuffer, bytes),
                            std::bind(&Connection::onRecv, shared_from_this(), std::placeholders::_1));

    if(timeout > 0) {
        m_timer.expires_from_now(boost::posix_time::seconds(timeout));
        m_timer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), std::placeholders::_1));
    }
}

void Connection::onTimeout(const boost::system::error_code& error)
{
    if(error != boost::asio::error::operation_aborted)
        g_dispatcher.addTask(std::bind(m_errorCallback, error));
}

void Connection::onResolve(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIterator)
{
    m_timer.cancel();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addTask(std::bind(m_errorCallback, error));
        return;
    }

    m_socket.async_connect(*endpointIterator, std::bind(&Connection::onConnect, shared_from_this(), std::placeholders::_1));

    m_timer.expires_from_now(boost::posix_time::seconds(2));
    m_timer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), std::placeholders::_1));
}

void Connection::onConnect(const boost::system::error_code& error)
{
    m_timer.cancel();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addTask(std::bind(m_errorCallback, error));
        return;
    }

    if(m_connectCallback)
        g_dispatcher.addTask(m_connectCallback);
}

void Connection::onSend(const boost::system::error_code& error, size_t)
{
    m_timer.cancel();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addTask(std::bind(m_errorCallback, error));
        return;
    }
}

void Connection::onRecv(const boost::system::error_code& error)
{
    m_timer.cancel();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addTask(std::bind(m_errorCallback, error));
        return;
    }

    if(m_recvCallback)
        g_dispatcher.addTask(std::bind(m_recvCallback, m_recvBuffer, m_recvSize));
}
