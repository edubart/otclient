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

#include "connection.h"

#include <framework/core/eventdispatcher.h>
#include <boost/asio.hpp>

asio::io_service g_ioService;

Connection::Connection() :
        m_readTimer(g_ioService),
        m_writeTimer(g_ioService),
        m_resolver(g_ioService),
        m_socket(g_ioService)
{
    m_connected = false;
    m_connecting = false;
}

void Connection::poll()
{
    g_ioService.poll();
    g_ioService.reset();
}

void Connection::terminate()
{
    g_ioService.stop();
}

void Connection::connect(const std::string& host, uint16 port, const SimpleCallback& connectCallback)
{
    m_connected = false;
    m_connecting = true;
    m_connectCallback = connectCallback;

    asio::ip::tcp::resolver::query query(host, Fw::unsafeCast<std::string>(port));
    m_resolver.async_resolve(query, std::bind(&Connection::onResolve, shared_from_this(), _1, _2));

    m_readTimer.expires_from_now(boost::posix_time::seconds(READ_TIMEOUT));
    m_readTimer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), _1));
}

void Connection::close()
{
    if(!m_connected && !m_connecting)
        return;

    m_connecting = false;
    m_connected = false;
    m_connectCallback = nullptr;
    m_errorCallback = nullptr;
    m_recvCallback = nullptr;

    m_readTimer.cancel();
    m_writeTimer.cancel();

    if(m_socket.is_open()) {
        boost::system::error_code ec;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        m_socket.close();
    }
}

void Connection::write(uint8* buffer, uint16 size)
{
    m_writeTimer.cancel();

    if(!m_connected)
        return;

    asio::async_write(m_socket,
                      asio::buffer(buffer, size),
                      std::bind(&Connection::onWrite, shared_from_this(), _1, _2));

    m_writeTimer.expires_from_now(boost::posix_time::seconds(WRITE_TIMEOUT));
    m_writeTimer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), _1));
}

void Connection::read(uint16 bytes, const RecvCallback& callback)
{
    m_readTimer.cancel();

    if(!m_connected)
        return;

    m_recvCallback = callback;

    asio::async_read(m_socket,
                     asio::buffer(m_recvBuffer, bytes),
                     std::bind(&Connection::onRecv, shared_from_this(), _1, bytes));

    m_readTimer.expires_from_now(boost::posix_time::seconds(READ_TIMEOUT));
    m_readTimer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), _1));
}

void Connection::read_some(const RecvCallback& callback)
{
    m_readTimer.cancel();

    if(!m_connected)
        return;

    m_recvCallback = callback;

    m_socket.async_read_some(asio::buffer(m_recvBuffer, RECV_BUFFER_SIZE),
                             std::bind(&Connection::onRecv, shared_from_this(), _1, _2));

    m_readTimer.expires_from_now(boost::posix_time::seconds(READ_TIMEOUT));
    m_readTimer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), _1));
}

void Connection::onResolve(const boost::system::error_code& error, asio::ip::tcp::resolver::iterator endpointIterator)
{
    m_readTimer.cancel();

    if(!error) {
        m_socket.async_connect(*endpointIterator, std::bind(&Connection::onConnect, shared_from_this(), _1));

        m_readTimer.expires_from_now(boost::posix_time::seconds(READ_TIMEOUT));
        m_readTimer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), _1));
    } else
        handleError(error);
}

void Connection::onConnect(const boost::system::error_code& error)
{
    m_readTimer.cancel();

    if(!error) {
        m_connected = true;

        // disable nagle's algorithm
        boost::asio::ip::tcp::no_delay option(true);
        m_socket.set_option(option);

        if(m_connectCallback)
            g_dispatcher.addEvent(m_connectCallback);
    } else
        handleError(error);

    m_connecting = false;
}

void Connection::onWrite(const boost::system::error_code& error, size_t)
{
    m_writeTimer.cancel();

    if(!m_connected)
        return;

    if(error)
        handleError(error);
}

void Connection::onRecv(const boost::system::error_code& error, size_t recvSize)
{
    m_readTimer.cancel();

    if(!m_connected)
        return;

    if(!error) {
        if(m_recvCallback)
            g_dispatcher.addEvent(std::bind(m_recvCallback, m_recvBuffer, recvSize));
    } else
        handleError(error);
}

void Connection::onTimeout(const boost::system::error_code& error)
{
    if(error != asio::error::operation_aborted)
        handleError(asio::error::timed_out);
}

void Connection::handleError(const boost::system::error_code& error)
{
    if(error != asio::error::operation_aborted) {
        if(m_errorCallback)
            g_dispatcher.addEvent(std::bind(m_errorCallback, error));
        if(m_connected || m_connecting)
            close();
    }
}

