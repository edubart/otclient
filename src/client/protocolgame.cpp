/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#include "protocolgame.h"
#include "framework/net/inputmessage.h"
#include "game.h"

void ProtocolGame::login(const std::string_view accountName, const std::string_view accountPassword, const std::string_view host, uint16_t port,
                         const std::string_view characterName, const std::string_view authenticatorToken, const std::string_view sessionKey)
{
    m_accountName = accountName;
    m_accountPassword = accountPassword;
    m_authenticatorToken = authenticatorToken;
    m_sessionKey = sessionKey;
    m_characterName = characterName;

    connect(host, port);
}

void ProtocolGame::onConnect()
{
    m_firstRecv = true;
    Protocol::onConnect();

    m_localPlayer = g_game.getLocalPlayer();

    if (g_game.getFeature(Otc::GameProtocolChecksum))
        enableChecksum();

    if (!g_game.getFeature(Otc::GameChallengeOnLogin))
        sendLoginPacket(0, 0);

    recv();
}

void ProtocolGame::onRecv(const InputMessagePtr& inputMessage)
{
    if (m_firstRecv) {
        m_firstRecv = false;

        if (g_game.getFeature(Otc::GameMessageSizeCheck)) {
            const int size = inputMessage->getU16();
            if (size != inputMessage->getUnreadSize()) {
                g_logger.traceError("invalid message size");
                return;
            }
        }
    }

    parseMessage(inputMessage);
    recv();
}

void ProtocolGame::onError(const std::error_code& error)
{
    g_game.processConnectionError(error);
    disconnect();
}
