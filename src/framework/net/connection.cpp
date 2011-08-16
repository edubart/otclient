#include "connection.h"
#include "networkmanager.h"

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
    m_connectCallback = connectCallback;

    asio::ip::tcp::resolver::query query(host, fw::unsafe_cast<std::string>(port));
    m_resolver.async_resolve(query, std::bind(&Connection::onResolve, shared_from_this(), _1, _2));

    m_readTimer.expires_from_now(boost::posix_time::seconds(READ_TIMEOUT));
    m_readTimer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), _1));
}

void Connection::close()
{
    if(!m_connected)
        return;

    m_readTimer.cancel();
    m_writeTimer.cancel();

    if(m_socket.is_open()) {
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        m_socket.close();
    }

    m_connected = false;
}

void Connection::write(uint8* buffer, uint16 size)
{
    if(!m_connected)
        return;

    m_writeTimer.cancel();

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
    m_recvSize = bytes;

    asio::async_read(m_socket,
                     asio::buffer(m_recvBuffer, bytes),
                     std::bind(&Connection::onRecv, shared_from_this(), _1));

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
    m_connected = true;

    if(!error) {
        if(m_connectCallback)
            g_dispatcher.addEvent(m_connectCallback);
    } else
        handleError(error);
}

void Connection::onWrite(const boost::system::error_code& error, size_t)
{
    m_writeTimer.cancel();

    if(error)
        handleError(error);
}

void Connection::onRecv(const boost::system::error_code& error)
{
    m_readTimer.cancel();

    if(!error) {
        if(m_recvCallback)
            g_dispatcher.addEvent(std::bind(m_recvCallback, m_recvBuffer, m_recvSize));
    } else
        handleError(error);
}

void Connection::onTimeout(const boost::system::error_code& error)
{
    if(error != asio::error::operation_aborted)
        handleError(error);
}

void Connection::handleError(const boost::system::error_code& error)
{
    logTraceDebug(error.message());
    close();
    if(m_errorCallback)
        g_dispatcher.addEvent(std::bind(m_errorCallback, error));
}

