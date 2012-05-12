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

#include <otclient/net/protocolgame.h>
#include <otclient/core/game.h>
#include <otclient/core/player.h>
#include <otclient/core/item.h>
#include <otclient/core/localplayer.h>

void ProtocolGame::login(const std::string& accountName, const std::string& accountPassword, const std::string& host, uint16 port, const std::string& characterName)
{
    if(accountName.empty() || accountPassword.empty()) {
        callLuaField("onError", "You must enter an account name and password.");
        return;
    }

    m_accountName = accountName;
    m_accountPassword = accountPassword;
    m_characterName = characterName;

    connect(host, port);
}

void ProtocolGame::onConnect()
{
    // must create local player before parsing anything
    m_localPlayer = LocalPlayerPtr(new LocalPlayer);

#if PROTOCOL>=854
    enableChecksum();
#else
    sendLoginPacket(0, 0);
#endif

    recv();
}

void ProtocolGame::onRecv(InputMessage& inputMessage)
{
    parseMessage(inputMessage);
    recv();
}

void ProtocolGame::onError(const boost::system::error_code& error)
{
    g_game.processConnectionError(error);
}

