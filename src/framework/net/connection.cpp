#include "connection.h"

#include <core/eventdispatcher.h>

static asio::io_service ioService;

Connection::Connection() :
        m_timer(ioService),
        m_resolver(ioService),
        m_socket(ioService)
{
}

Connection::~Connection()
{
    disconnect();
}

void Connection::poll()
{
    ioService.poll();
    ioService.reset();
}

void Connection::connect(const std::string& host, uint16 port, const ConnectCallback& connectCallback)
{
    m_connectCallback = connectCallback;

    asio::ip::tcp::resolver::query query(host, aux::unsafe_cast<std::string>(port));
    m_resolver.async_resolve(query, std::bind(&Connection::onResolve, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

    m_timer.expires_from_now(boost::posix_time::seconds(2));
    m_timer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), std::placeholders::_1));
}

void Connection::disconnect()
{
    m_socket.close();
}

void Connection::send(uint8* buffer, uint16 size)
{
    asio::async_write(m_socket,
                             asio::buffer(buffer, size),
                             std::bind(&Connection::onSend, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

    m_timer.expires_from_now(boost::posix_time::seconds(2));
    m_timer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), std::placeholders::_1));
}

void Connection::recv(uint16 bytes, uint32 timeout, const RecvCallback& callback)
{
    m_recvCallback = callback;
    m_recvSize = bytes;

    asio::async_read(m_socket,
                            asio::buffer(m_recvBuffer, bytes),
                            std::bind(&Connection::onRecv, shared_from_this(), std::placeholders::_1));

    if(timeout > 0) {
        m_timer.expires_from_now(boost::posix_time::seconds(timeout));
        m_timer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), std::placeholders::_1));
    }
}

void Connection::onTimeout(const boost::system::error_code& error)
{
    if(error != asio::error::operation_aborted)
        g_dispatcher.addEvent(std::bind(m_errorCallback, error));
}

void Connection::onResolve(const boost::system::error_code& error, asio::ip::tcp::resolver::iterator endpointIterator)
{
    m_timer.cancel();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addEvent(std::bind(m_errorCallback, error));
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
            g_dispatcher.addEvent(std::bind(m_errorCallback, error));
        return;
    }

    if(m_connectCallback)
        g_dispatcher.addEvent(m_connectCallback);
}

void Connection::onSend(const boost::system::error_code& error, size_t)
{
    m_timer.cancel();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addEvent(std::bind(m_errorCallback, error));
        return;
    }
}

void Connection::onRecv(const boost::system::error_code& error)
{
    m_timer.cancel();

    if(error) {
        if(m_errorCallback)
            g_dispatcher.addEvent(std::bind(m_errorCallback, error));
        return;
    }

    if(m_recvCallback)
        g_dispatcher.addEvent(std::bind(m_recvCallback, m_recvBuffer, m_recvSize));
}
