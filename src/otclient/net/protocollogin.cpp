/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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
#include <framework/luascript/luainterface.h>
#include <boost/bind.hpp>
#include <otclient/core/thingstype.h>
#include <otclient/core/spritemanager.h>
#include <otclient/core/game.h>

void ProtocolLogin::login(const std::string& host, int port, const std::string& accountName, const std::string& accountPassword)
{
    if(accountName.empty() || accountPassword.empty()) {
        callLuaField("onError", "You must enter an account name and password.");
        return;
    }

    m_accountName = accountName;
    m_accountPassword = accountPassword;

    connect(host, port);
}

void ProtocolLogin::onConnect()
{
    sendLoginPacket();
}

void ProtocolLogin::onRecv(const InputMessagePtr& msg)
{
    try {
        while(!msg->eof()) {
            int opcode = msg->getU8();
            switch(opcode) {
            case Proto::LoginServerError:
                parseError(msg);
                break;
            case Proto::LoginServerMotd:
                parseMOTD(msg);
                break;
            case Proto::LoginServerUpdateNeeded:
                callLuaField("onError", "Client needs update.");
                break;
            case Proto::LoginServerCharacterList:
                parseCharacterList(msg);
                break;
            default:
                stdext::throw_exception(stdext::format("unknown opcode %d", opcode));
                break;
            }
        }
    } catch(stdext::exception& e) {
        logTraceError(e.what());
    }
    disconnect();
}

void ProtocolLogin::onError(const boost::system::error_code& error)
{
    callLuaField("onError", error.message(), true);
    disconnect();
}

void ProtocolLogin::sendLoginPacket()
{
    OutputMessagePtr msg(new OutputMessage);

    msg->addU8(Proto::ClientEnterAccount);
    msg->addU16(Proto::ClientOs);
    msg->addU16(Proto::ClientVersion);

    msg->addU32(g_thingsType.getSignature()); // data signature
    msg->addU32(g_sprites.getSignature()); // sprite signature
    msg->addU32(Proto::PicSignature); // pic signature

    int paddingBytes = 128;
    msg->addU8(0); // first RSA byte must be 0
    paddingBytes -= 1;

    // xtea key
    generateXteaKey();

    msg->addU32(m_xteaKey[0]);
    msg->addU32(m_xteaKey[1]);
    msg->addU32(m_xteaKey[2]);
    msg->addU32(m_xteaKey[3]);
    paddingBytes -= 16;

    if(g_game.getFeature(Otc::GameProtocolChecksum))
        enableChecksum();

    if(g_game.getFeature(Otc::GameAccountNames)) {
        msg->addString(m_accountName);
        msg->addString(m_accountPassword);
        paddingBytes -= 4 + m_accountName.length() + m_accountPassword.length();
    } else {
        msg->addU32(stdext::from_string<uint32>(m_accountName));
        msg->addString(m_accountPassword);
        paddingBytes -= 6 + m_accountPassword.length();
    }

    msg->addPaddingBytes(paddingBytes); // complete the 128 bytes for rsa encryption with zeros
    msg->encryptRSA(128, Proto::RSA);

    send(msg);
    enableXteaEncryption();
    recv();
}

void ProtocolLogin::parseError(const InputMessagePtr& msg)
{
    std::string error = msg->getString();
    callLuaField("onError", error, false);
}

void ProtocolLogin::parseMOTD(const InputMessagePtr& msg)
{
    std::string motd = msg->getString();
    callLuaField("onMotd", motd);
}

void ProtocolLogin::parseCharacterList(const InputMessagePtr& msg)
{
    typedef std::tuple<std::string, std::string, std::string, int> CharacterInfo;
    typedef std::vector<CharacterInfo> CharaterList;
    CharaterList charList;

    int numCharacters = msg->getU8();
    for(int i = 0; i < numCharacters; ++i) {
        std::string name = msg->getString();
        std::string world = msg->getString();
        uint32 ip = msg->getU32();
        uint16 port = msg->getU16();
        charList.push_back(CharacterInfo(name, world, stdext::ip_to_string(ip), port));
    }
    int premDays = msg->getU16();

    callLuaField("onCharacterList", charList, premDays);
}
