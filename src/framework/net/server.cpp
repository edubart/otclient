/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "server.h"
#include "connection.h"

extern asio::io_service g_ioService;

Server::Server(int port)
    : m_acceptor(g_ioService, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
{
}

ServerPtr Server::create(int port)
{
    try {
        Server *server = new Server(port);
        return ServerPtr(server);
    }
    catch(const std::exception& e) {
        g_logger.error(stdext::format("Failed to initialize server: %s", e.what()));
        return ServerPtr();
    }
}

void Server::close()
{
    m_isOpen = false;
    m_acceptor.cancel();
    m_acceptor.close();
}

void Server::acceptNext()
{
    ConnectionPtr connection = ConnectionPtr(new Connection);
    connection->m_connecting = true;
    auto self = static_self_cast<Server>();
    m_acceptor.async_accept(connection->m_socket, [=](const boost::system::error_code& error) {
        if(!error) {
            connection->m_connected = true;
            connection->m_connecting = false;
        }
        self->callLuaField("onAccept", connection, error.message(), error.value());
    });
}
