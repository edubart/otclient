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
#include <otclient/core/thingstype.h>
#include <otclient/core/spritemanager.h>

ProtocolLogin::ProtocolLogin()
{
#ifndef POKE
    enableChecksum();
#endif
}

void ProtocolLogin::login(const std::string& accountName, const std::string& accountPassword)
{
    if(accountName.empty() || accountPassword.empty()) {
        callLuaField("onError", "You must enter an account name and password.");
        return;
    }

    m_accountName = accountName;
    m_accountPassword = accountPassword;

    connect(Otc::Host, Otc::HostPort);
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
        case Otc::LoginServerError:
            parseError(inputMessage);
            break;
        case Otc::LoginServerMotd:
            parseMOTD(inputMessage);
            break;
        case Otc::LoginServerUpdateNeeded:
            callLuaField("onError", "Client needs update.");
            break;
        case Otc::LoginServerCharacterList:
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

#ifdef POKE
    oMsg.addU8(Otc::ClientEnterAccount);
    oMsg.addU16(Otc::OsPoke);
    oMsg.addU16(Otc::ClientVersion);

    oMsg.addU32(g_thingsType.getSignature()); // data signature
    oMsg.addU32(g_sprites.getSignature()); // sprite signature
    oMsg.addU32(Otc::PicSignature); // pic signature

    oMsg.addU8(0); // first RSA byte must be 0

    // xtea key
    generateXteaKey();
    oMsg.addU32(m_xteaKey[0]);
    oMsg.addU32(m_xteaKey[1]);
    oMsg.addU32(m_xteaKey[2]);
    oMsg.addU32(m_xteaKey[3]);

    oMsg.addU32(Fw::unsafeCast<int>(m_accountName));
    oMsg.addString(m_accountPassword);

    // complete the 128 bytes for rsa encryption with zeros
    oMsg.addPaddingBytes(128 - (23 + m_accountPassword.length()));
#else
    oMsg.addU8(Otc::ClientEnterAccount);
    oMsg.addU16(Otc::OsLinux);
    oMsg.addU16(Otc::ClientVersion);

    oMsg.addU32(g_thingsType.getSignature()); // data signature
    oMsg.addU32(g_sprites.getSignature()); // sprite signature
    oMsg.addU32(Otc::PicSignature); // pic signature

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
#endif

    if(!Rsa::encrypt((char*)oMsg.getBuffer() + InputMessage::DATA_POS + oMsg.getMessageSize() - 128, 128, Otc::OtservPublicRSA))
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
        charList.push_back(CharacterInfo(name, world, Fw::ip2str(ip), port));
    }
    int premDays = inputMessage.getU16();

    callLuaField("onCharacterList", charList, premDays);
}
