#ifndef CONNECTION_H
#define CONNECTION_H

#include "declarations.h"
#include <boost/asio.hpp>

class Connection : public std::enable_shared_from_this<Connection>, boost::noncopyable
{
    typedef std::function<void(boost::system::error_code&)> ErrorCallback;
    typedef std::function<void(uint8*, uint16)> RecvCallback;
    typedef std::function<void()> ConnectCallback;

public:
    Connection();
    virtual ~Connection();

    static void poll();

    void connect(const std::string& host, uint16 port, const ConnectCallback& connectCallback);
    void disconnect();
    void send(uint8* buffer, uint16 size);
    void recv(uint16 bytes, uint32 timeout, const RecvCallback& callback);

    void setErrorCallback(const ErrorCallback& errorCallback) { m_errorCallback = errorCallback; }
    void setRecvCallback(const RecvCallback& recvCallback) { m_recvCallback = recvCallback; }

private:
    void onTimeout(const boost::system::error_code& error);
    void onResolve(const boost::system::error_code& error, asio::ip::tcp::resolver::iterator endpointIterator);
    void onConnect(const boost::system::error_code& error);
    void onSend(const boost::system::error_code& error, size_t);
    void onRecv(const boost::system::error_code& error);


    ErrorCallback m_errorCallback;
    ConnectCallback m_connectCallback;

    asio::deadline_timer m_timer;
    asio::ip::tcp::resolver m_resolver;
    asio::ip::tcp::socket m_socket;

    uint8 m_recvBuffer[65538];
    uint16 m_recvSize;
    RecvCallback m_recvCallback;
};

#endif
