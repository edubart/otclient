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


#include "protocollogin.h"

ProtocolLogin::ProtocolLogin()
{

}

void ProtocolLogin::login(const std::string& accountName, const std::string& password)
{
    // return error if acc or password is empty or any other condition

    m_accountName = accountName;
    m_password = password;

    static const char hosts[][32] = {
        "login01.tibia.com",
        "login02.tibia.com",
        "login03.tibia.com",
        "login04.tibia.com",
        "login05.tibia.com",
        "tibia01.cipsoft.com",
        "tibia02.cipsoft.com",
        "tibia03.cipsoft.com",
        "tibia04.cipsoft.com",
        "tibia05.cipsoft.com"
    };

    const std::string host = hosts[rand() % 10];
    const uint16 port = 7171;

    connect(host, port, boost::bind(&ProtocolLogin::onConnect, this));
}

void ProtocolLogin::onConnect()
{
    logTrace();
    sendPacket();
}

void ProtocolLogin::sendPacket()
{

}
