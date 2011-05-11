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

void Connection::connect(const std::string& host, uint16 port, const SimpleCallback& callback)
{
    m_connectCallback = callback;
    m_connectionState = CONNECTION_STATE_RESOLVING;

    boost::asio::ip::tcp::resolver::query query(host, convert_cast<std::string>(port));
    m_resolver.async_resolve(query, boost::bind(&Connection::onResolve, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));

    m_timer.expires_from_now(boost::posix_time::seconds(2));
    m_timer.async_wait(boost::bind(&Connection::onTimeout, this, boost::asio::placeholders::error));
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
        g_dispatcher.addTask(boost::bind(m_errorCallback, error));
        return;
    }

    m_socket.async_connect(*endpointIterator, boost::bind(&Connection::onConnect, this, boost::asio::placeholders::error));

    m_timer.expires_from_now(boost::posix_time::seconds(2));
    m_timer.async_wait(boost::bind(&Connection::onTimeout, this, boost::asio::placeholders::error));
}

void Connection::onConnect(const boost::system::error_code& error)
{
    logTrace();

    m_timer.cancel();

    if(error) {
        g_dispatcher.addTask(boost::bind(m_errorCallback, error));
        return;
    }

    g_dispatcher.addTask(m_connectCallback);
}
