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

#include "connection.h"

#include <boost/bind.hpp>

Connection::Connection(boost::asio::io_service& ioService) 
    : m_socket(ioService), m_resolver(ioService)
{
    m_connected = false;
    m_connecting = false;
    m_port = 0;
}

void Connection::stop()
{
    if(m_connecting){
        m_resolver.cancel();
        m_socket.cancel();
        
        m_connecting = false;
    }
}

void Connection::connect(const std::string& ip, uint16 port)
{
    if(m_connecting){
        logError("Already is connecting.");
        return;
    }

    m_connecting = true;
    m_ip = ip;
    m_port = port;
    
    logDebug("connecting...");
    
    //first resolve dns
    boost::asio::ip::tcp::resolver::query query(ip, "80");
    m_resolver.async_resolve(query, boost::bind(&Connection::onResolveDns, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
}

void Connection::onResolveDns(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
    logDebug("resolving dns..");
    if(error){
        m_connecting = false;
        m_lastError = error;
        return;
    }

    //lets connect    
    m_socket.async_connect(*endpoint_iterator, boost::bind(&Connection::onConnect, this, boost::asio::placeholders::error));
}

void Connection::onConnect(const boost::system::error_code& error)
{
    if(error){
        m_connecting = false;
        m_lastError = error;
        return;
    }    
    
    m_connected = true;
    
    logInfo("Connected on %s.", m_ip.c_str());
}
