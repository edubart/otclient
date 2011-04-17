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


#include <prerequisites.h>
#include <util/rsa.h>
#include "protocollogin.h"

ProtocolLogin::ProtocolLogin()
{
    logTrace();
}

ProtocolLogin::~ProtocolLogin()
{
    logTrace();
}

void ProtocolLogin::login(const std::string& account, const std::string& password)
{
    logTrace();
    m_connection = ConnectionPtr(new Connection);
    m_connection->connect("www.google.com", 80, boost::bind(&ProtocolLogin::afterConnect, this));
}

void ProtocolLogin::afterConnect()
{
    logTrace();

}

void ProtocolLogin::onError(const boost::system::error_code& error, const std::string& msg)
{
    logTrace();
    logError("Connection error: %s", error.message().c_str());
}



