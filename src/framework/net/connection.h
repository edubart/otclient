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

#ifndef CONNECTION_H
#define CONNECTION_H

#include "../prerequisites.h"

#include <boost/asio.hpp>

class Connection
{
public:
    Connection(boost::asio::io_service& ioService);
    
    void connect(const std::string& ip, uint16 port);
    void stop();
    
    bool isConnecting() const { return m_connecting; } 
    bool isConnected() const { return m_connected; } 
    
    const boost::system::error_code& getLastError() const { return m_lastError; }
    
private:
    void onResolveDns(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    void onConnect(const boost::system::error_code& error);
   
private:
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::resolver m_resolver;
    boost::system::error_code m_lastError;
    
    bool m_connecting;
    bool m_connected;
    
    std::string m_ip;
    uint16_t m_port;
};

typedef std::shared_ptr<Connection> ConnectionPtr;

#endif //CONNECTION_h
