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
#include <net/outputmessage.h>
#include <net/rsa.h>
#include <script/scriptcontext.h>
#include <boost/bind.hpp>

ProtocolLogin::ProtocolLogin()
{

}

void ProtocolLogin::login(const std::string& accountName, const std::string& accountPassword)
{
    if(accountName.empty() || accountPassword.empty()) {
        // shows error dialog
        return;
    }

    m_accountName = accountName;
    m_accountPassword = accountPassword;

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

    std::string host = hosts[rand() % 10];
    //std::string host = "tecserver.zapto.org";
    uint16 port = 7171;

    connect(host, port, boost::bind(&ProtocolLogin::onConnect, this));
}

void ProtocolLogin::onConnect()
{
    trace();
    sendPacket();
}

void ProtocolLogin::sendPacket()
{
    OutputMessage oMsg;

    oMsg.addU8(0x01); // Protocol id
    oMsg.addU16(0x02);  // OS
    oMsg.addU16(874); // Client version

    oMsg.addU32(0x4DBAA20B); // Data Signature
    oMsg.addU32(0x4DAD1A32); // Sprite Signature
    oMsg.addU32(0x4DA2D2B5); // Picture Signature

    oMsg.addU8(0); // First RSA byte must be 0x00 // 1

    // Generete xtea key.
    m_xteaKey[0] = 432324;
    m_xteaKey[1] = 24324;
    m_xteaKey[2] = 423432;
    m_xteaKey[3] = 234324;

    // Add xtea key
    oMsg.addU32(m_xteaKey[0]); // 5
    oMsg.addU32(m_xteaKey[1]); // 9
    oMsg.addU32(m_xteaKey[2]); // 13
    oMsg.addU32(m_xteaKey[3]); // 17

    oMsg.addString(m_accountName); // Account Name // 19
    oMsg.addString(m_accountPassword); // Password // 21

    // Packet data must have since byte 0, start, 128 bytes
    oMsg.addPaddingBytes(128 - (21 + m_accountName.length() + m_accountPassword.length()));

    // Encrypt msg with RSA
    if(!Rsa::encrypt((char*)oMsg.getBuffer() + 6 + oMsg.getMessageSize() - 128, 128, CIPSOFT_PUBLIC_RSA))
        return;

    send(&oMsg);

    m_xteaEncryptionEnabled = true;
}

void ProtocolLogin::onRecv(InputMessage *inputMessage)
{
    Protocol::onRecv(inputMessage);

    while(!inputMessage->end()) {
        uint8 opt = inputMessage->getU8();
        debug("opt:",(uint)opt);
        switch(opt) {
        case 0x0A:
            parseError(inputMessage);
            break;
        case 0x14:
            parseMOTD(inputMessage);
            break;
        case 0x64:
            parseCharacterList(inputMessage);
            break;
        }
    }
}

void ProtocolLogin::parseError(InputMessage *inputMessage)
{
    std::string error = inputMessage->getString();
    g_lua.pushString(error);
    callScriptTableField("onError", 1);
}

void ProtocolLogin::parseMOTD(InputMessage *inputMessage)
{
    std::string motd = inputMessage->getString();
    g_lua.pushString(motd);
    callScriptTableField("onMotd", 1);
}

void ProtocolLogin::parseCharacterList(InputMessage *inputMessage)
{
    uint8 characters = inputMessage->getU8();
    for(int i = 0; i < characters; ++i) {
        std::string name = inputMessage->getString();
        std::string world = inputMessage->getString();
        uint32 ip = inputMessage->getU32();
        uint16 port = inputMessage->getU16();

        debug("character: ", name.c_str(), world.c_str(), ip, port);
    }
    uint16 premiumDays = inputMessage->getU16();
    debug("prem days: ", premiumDays);
}
