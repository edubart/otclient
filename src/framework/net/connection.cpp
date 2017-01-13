/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#include <framework/core/application.h>
#include <framework/core/eventdispatcher.h>
#include <boost/asio.hpp>

asio::io_service g_ioService;
std::list<std::shared_ptr<asio::streambuf>> Connection::m_outputStreams;

Connection::Connection() :
        m_readTimer(g_ioService),
        m_writeTimer(g_ioService),
        m_delayedWriteTimer(g_ioService),
        m_resolver(g_ioService),
        m_socket(g_ioService)
{
    m_connected = false;
    m_connecting = false;
}

Connection::~Connection()
{
#ifndef NDEBUG
    assert(!g_app.isTerminated());
#endif
    close();
}

void Connection::poll()
{
    // reset must always be called prior to poll
    g_ioService.reset();
    g_ioService.poll();
}

void Connection::terminate()
{
    g_ioService.stop();
    m_outputStreams.clear();
}

void Connection::close()
{
    if(!m_connected && !m_connecting)
        return;

    // flush send data before disconnecting on clean connections
    if(m_connected && !m_error && m_outputStream)
        internal_write();

    m_connecting = false;
    m_connected = false;
    m_connectCallback = nullptr;
    m_errorCallback = nullptr;
    m_recvCallback = nullptr;

    m_resolver.cancel();
    m_readTimer.cancel();
    m_writeTimer.cancel();
    m_delayedWriteTimer.cancel();

    if(m_socket.is_open()) {
        boost::system::error_code ec;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        m_socket.close();
    }
}

void Connection::connect(const std::string& host, uint16 port, const std::function<void()>& connectCallback)
{
    m_connected = false;
    m_connecting = true;
    m_error.clear();
    m_connectCallback = connectCallback;

    asio::ip::tcp::resolver::query query(host, stdext::unsafe_cast<std::string>(port));
    m_resolver.async_resolve(query, std::bind(&Connection::onResolve, asConnection(), std::placeholders::_1, std::placeholders::_2));

    m_readTimer.cancel();
    m_readTimer.expires_from_now(boost::posix_time::seconds(READ_TIMEOUT));
    m_readTimer.async_wait(std::bind(&Connection::onTimeout, asConnection(), std::placeholders::_1));
}

void Connection::internal_connect(asio::ip::basic_resolver<asio::ip::tcp>::iterator endpointIterator)
{
    m_socket.async_connect(*endpointIterator, std::bind(&Connection::onConnect, asConnection(), std::placeholders::_1));

    m_readTimer.cancel();
    m_readTimer.expires_from_now(boost::posix_time::seconds(READ_TIMEOUT));
    m_readTimer.async_wait(std::bind(&Connection::onTimeout, asConnection(), std::placeholders::_1));
}

void Connection::write(uint8* buffer, size_t size)
{
    if(!m_connected)
        return;

    // we can't send the data right away, otherwise we could create tcp congestion
    if(!m_outputStream) {
        if(!m_outputStreams.empty()) {
            m_outputStream = m_outputStreams.front();
            m_outputStreams.pop_front();
        } else
            m_outputStream = std::shared_ptr<asio::streambuf>(new asio::streambuf);

        m_delayedWriteTimer.cancel();
        m_delayedWriteTimer.expires_from_now(boost::posix_time::milliseconds(0));
        m_delayedWriteTimer.async_wait(std::bind(&Connection::onCanWrite, asConnection(), std::placeholders::_1));
    }

    std::ostream os(m_outputStream.get());
    os.write((const char*)buffer, size);
    os.flush();
}

void Connection::internal_write()
{
    if(!m_connected)
        return;

    std::shared_ptr<asio::streambuf> outputStream = m_outputStream;
    m_outputStream = nullptr;

    asio::async_write(m_socket,
                      *outputStream,
                      std::bind(&Connection::onWrite, asConnection(), std::placeholders::_1, std::placeholders::_2, outputStream));

    m_writeTimer.cancel();
    m_writeTimer.expires_from_now(boost::posix_time::seconds(WRITE_TIMEOUT));
    m_writeTimer.async_wait(std::bind(&Connection::onTimeout, asConnection(), std::placeholders::_1));
}

void Connection::read(uint16 bytes, const RecvCallback& callback)
{
    if(!m_connected)
        return;

    m_recvCallback = callback;

    asio::async_read(m_socket,
                     asio::buffer(m_inputStream.prepare(bytes)),
                     std::bind(&Connection::onRecv, asConnection(), std::placeholders::_1, std::placeholders::_2));

    m_readTimer.cancel();
    m_readTimer.expires_from_now(boost::posix_time::seconds(READ_TIMEOUT));
    m_readTimer.async_wait(std::bind(&Connection::onTimeout, asConnection(), std::placeholders::_1));
}

void Connection::read_until(const std::string& what, const RecvCallback& callback)
{
    if(!m_connected)
        return;

    m_recvCallback = callback;

    asio::async_read_until(m_socket,
                           m_inputStream,
                           what.c_str(),
                           std::bind(&Connection::onRecv, asConnection(), std::placeholders::_1, std::placeholders::_2));

    m_readTimer.cancel();
    m_readTimer.expires_from_now(boost::posix_time::seconds(READ_TIMEOUT));
    m_readTimer.async_wait(std::bind(&Connection::onTimeout, asConnection(), std::placeholders::_1));
}

void Connection::read_some(const RecvCallback& callback)
{
    if(!m_connected)
        return;

    m_recvCallback = callback;

    m_socket.async_read_some(asio::buffer(m_inputStream.prepare(RECV_BUFFER_SIZE)),
                             std::bind(&Connection::onRecv, asConnection(), std::placeholders::_1, std::placeholders::_2));

    m_readTimer.cancel();
    m_readTimer.expires_from_now(boost::posix_time::seconds(READ_TIMEOUT));
    m_readTimer.async_wait(std::bind(&Connection::onTimeout, asConnection(), std::placeholders::_1));
}

void Connection::onResolve(const boost::system::error_code& error, asio::ip::basic_resolver<asio::ip::tcp>::iterator endpointIterator)
{
    m_readTimer.cancel();

    if(error == asio::error::operation_aborted)
        return;

    if(!error)
        internal_connect(endpointIterator);
    else
        handleError(error);
}

void Connection::onConnect(const boost::system::error_code& error)
{
    m_readTimer.cancel();
    m_activityTimer.restart();

    if(error == asio::error::operation_aborted)
        return;

    if(!error) {
        m_connected = true;

        // disable nagle's algorithm, this make the game play smoother
        boost::asio::ip::tcp::no_delay option(true);
        m_socket.set_option(option);

        if(m_connectCallback)
            m_connectCallback();
    } else
        handleError(error);

    m_connecting = false;
}

void Connection::onCanWrite(const boost::system::error_code& error)
{
    m_delayedWriteTimer.cancel();

    if(error == asio::error::operation_aborted)
        return;

    if(m_connected)
        internal_write();
}

void Connection::onWrite(const boost::system::error_code& error, size_t writeSize, std::shared_ptr<asio::streambuf> outputStream)
{
    m_writeTimer.cancel();

    if(error == asio::error::operation_aborted)
        return;

    // free output stream and store for using it again later
    outputStream->consume(outputStream->size());
    m_outputStreams.push_back(outputStream);

    if(m_connected && error)
        handleError(error);
}

void Connection::onRecv(const boost::system::error_code& error, size_t recvSize)
{
    m_readTimer.cancel();
    m_activityTimer.restart();

    if(error == asio::error::operation_aborted)
        return;

    if(m_connected) {
        if(!error) {
            if(m_recvCallback) {
                const char* header = boost::asio::buffer_cast<const char*>(m_inputStream.data());
                m_recvCallback((uint8*)header, recvSize);
            }
        } else
            handleError(error);
    }

    if(!error)
        m_inputStream.consume(recvSize);
}

void Connection::onTimeout(const boost::system::error_code& error)
{
    if(error == asio::error::operation_aborted)
        return;

    handleError(asio::error::timed_out);
}

void Connection::handleError(const boost::system::error_code& error)
{
    if(error == asio::error::operation_aborted)
        return;

    m_error = error;
    if(m_errorCallback)
        m_errorCallback(error);
    if(m_connected || m_connecting)
        close();
}

int Connection::getIp()
{
    boost::system::error_code error;
    const boost::asio::ip::tcp::endpoint ip = m_socket.remote_endpoint(error);
    if(!error)
        return boost::asio::detail::socket_ops::host_to_network_long(ip.address().to_v4().to_ulong());

    g_logger.error("Getting remote ip");
    return 0;
}
