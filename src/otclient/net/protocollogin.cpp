/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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
#include <framework/net/outputmessage.h>
#include <framework/net/rsa.h>
#include <framework/luascript/luainterface.h>
#include <boost/bind.hpp>

// TODO just testing
#include "protocolgame.h"
#include <otclient/core/game.h>

ProtocolLogin::ProtocolLogin()
{
    enableChecksum();
}

void ProtocolLogin::login(const std::string& accountName, const std::string& accountPassword)
{
    if(accountName.empty() || accountPassword.empty()) {
        callLuaField("onError", "You must enter an account name and password.");
        return;
    }

    m_accountName = accountName;
    m_accountPassword = accountPassword;

    std::string host = "sv3.radbr.com";
    uint16 port = 7171;

    connect(host, port);
}

void ProtocolLogin::onConnect()
{
    sendLoginPacket();
}

void ProtocolLogin::onRecv(InputMessage& inputMessage)
{
    while(!inputMessage.eof()) {
        uint8 opt = inputMessage.getU8();
        switch(opt) {
        case 0x0A:
            parseError(inputMessage);
            break;
        case 0x14:
            parseMOTD(inputMessage);
            break;
        case 0x1e:
            callLuaField("onError", "Client needs update.");
            break;
        case 0x64:
            parseCharacterList(inputMessage);
            break;
        }
    }
    disconnect();
}

void ProtocolLogin::onError(const boost::system::error_code& error)
{
    callLuaField("onError", error.message());
}

void ProtocolLogin::sendLoginPacket()
{
    OutputMessage oMsg;

    oMsg.addU8(0x01); // protocol id
    oMsg.addU16(0x02); // os
    oMsg.addU16(862); // client version

    oMsg.addU32(0x4E12DAFF); // data signature
    oMsg.addU32(0x4E12DB27); // sprite signature
    oMsg.addU32(0x4E119CBF); // pic signature

    oMsg.addU8(0); // first RSA byte must be 0

    // xtea key
    generateXteaKey();
    oMsg.addU32(m_xteaKey[0]);
    oMsg.addU32(m_xteaKey[1]);
    oMsg.addU32(m_xteaKey[2]);
    oMsg.addU32(m_xteaKey[3]);

    oMsg.addString(m_accountName);
    oMsg.addString(m_accountPassword);

    // complete the 128 bytes for rsa encryption with zeros
    oMsg.addPaddingBytes(128 - (21 + m_accountName.length() + m_accountPassword.length()));

    if(!Rsa::encrypt((char*)oMsg.getBuffer() + 6 + oMsg.getMessageSize() - 128, 128, OTSERV_PUBLIC_RSA))
        return;

    send(oMsg);

    enableXteaEncryption();

    recv();
}

void ProtocolLogin::parseError(InputMessage& inputMessage)
{
    std::string error = inputMessage.getString();
    callLuaField("onError", error);
}

void ProtocolLogin::parseMOTD(InputMessage& inputMessage)
{
    std::string motd = inputMessage.getString();
    callLuaField("onMotd", motd);
}

void ProtocolLogin::parseCharacterList(InputMessage& inputMessage)
{
    typedef std::tuple<std::string, std::string, std::string, int> CharacterInfo;
    typedef std::vector<CharacterInfo> CharaterList;
    CharaterList charList;

    int numCharacters = inputMessage.getU8();
    for(int i = 0; i < numCharacters; ++i) {
        std::string name = inputMessage.getString();
        std::string world = inputMessage.getString();
        uint32 ip = inputMessage.getU32();
        uint16 port = inputMessage.getU16();
        charList.push_back(CharacterInfo(name, world, fw::ip2str(ip), port));
    }
    int premDays = inputMessage.getU16();

    callLuaField("onCharacterList", charList, premDays);
}
