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

#include "protocolgame.h"
#include <framework/net/rsa.h>

void ProtocolGame::sendLoginPacket(uint32 timestamp, uint8 unknown)
{
    OutputMessage oMsg;

    oMsg.addU8(Otc::ClientEnterGame);
    oMsg.addU16(Otc::OsLinux);
    oMsg.addU16(Otc::ClientVersion);

    oMsg.addU8(0); // first RSA byte must be 0

    // xtea key
    generateXteaKey();
    oMsg.addU32(m_xteaKey[0]);
    oMsg.addU32(m_xteaKey[1]);
    oMsg.addU32(m_xteaKey[2]);
    oMsg.addU32(m_xteaKey[3]);

    oMsg.addU8(0); // is gm set?
    oMsg.addString(m_accountName);
    oMsg.addString(m_characterName);
    oMsg.addString(m_accountPassword);

    oMsg.addU32(timestamp);
    oMsg.addU8(unknown);

    // complete the 128 bytes for rsa encryption with zeros
    oMsg.addPaddingBytes(128 - (29 + m_accountName.length() + m_characterName.length() + m_accountPassword.length()));

    // encrypt with RSA
    if(!Rsa::encrypt((char*)oMsg.getBuffer() + 6 + oMsg.getMessageSize() - 128, 128, Otc::OtservPublicRSA))
        return;

    send(oMsg);

    enableXteaEncryption();
}

void ProtocolGame::sendLogout()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientQuitGame);
    send(oMsg);
}

void ProtocolGame::sendPing()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientPingBack);
    send(oMsg);
}

void ProtocolGame::sendWalkNorth()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientGoNorth);
    send(oMsg);
}

void ProtocolGame::sendWalkEast()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientGoEast);
    send(oMsg);
}

void ProtocolGame::sendWalkSouth()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientGoSouth);
    send(oMsg);
}

void ProtocolGame::sendWalkWest()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientGoWest);
    send(oMsg);
}

void ProtocolGame::sendWalkNorthEast()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientGoNorthEast);
    send(oMsg);
}

void ProtocolGame::sendWalkSouthEast()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientGoSouthEast);
    send(oMsg);
}

void ProtocolGame::sendWalkSouthWest()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientGoSouthWest);
    send(oMsg);
}

void ProtocolGame::sendWalkNorthWest()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientGoNorthWest);
    send(oMsg);
}

void ProtocolGame::sendTurnNorth()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientRotateNorth);
    send(oMsg);
}

void ProtocolGame::sendTurnEast()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientRotateEast);
    send(oMsg);
}

void ProtocolGame::sendTurnSouth()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientRotateSouth);
    send(oMsg);
}

void ProtocolGame::sendTurnWest()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientRotateWest);
    send(oMsg);
}

void ProtocolGame::sendUseItem(const Position& position, int itemId, int stackpos, int index)
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientUseObject);
    addPosition(oMsg, position);
    oMsg.addU16(itemId);
    oMsg.addU8(stackpos);
    oMsg.addU8(index);
    send(oMsg);
}

void ProtocolGame::sendTalk(int channelType, int channelId, const std::string& receiver, const std::string& message)
{
    if(message.length() > 255 || message.length() <= 0)
        return;

    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientTalk);

    assert(channelType >= 0);
    oMsg.addU8(channelType);

    switch(channelType) {
    case Otc::SpeakPrivate:
    case Otc::SpeakPrivateRed:
        oMsg.addString(receiver);
        break;
    case Otc::SpeakChannelYellow:
    case Otc::SpeakChannelRed:
        oMsg.addU16(channelId);
        break;
    }

    oMsg.addString(message);
    send(oMsg);
}

void ProtocolGame::sendGetOutfit()
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientGetOutfit);
    send(oMsg);
}

void ProtocolGame::sendAddVip(const std::string& name)
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientAddBuddy);
    oMsg.addString(name);
    send(oMsg);
}

void ProtocolGame::sendRemoveVip(int id)
{
    OutputMessage oMsg;
    oMsg.addU8(Otc::ClientRemoveBuddy);
    oMsg.addU32(id);
    send(oMsg);
}

void ProtocolGame::addPosition(OutputMessage& msg, const Position& position)
{
    msg.addU16(position.x);
    msg.addU16(position.y);
    msg.addU8(position.z);
}
