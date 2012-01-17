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

#include "protocolgame.h"
#include <framework/net/rsa.h>

void ProtocolGame::sendLoginPacket(uint timestamp, uint8 unknown)
{
    OutputMessage oMsg;

    oMsg.addU8(Proto::ClientEnterGame);
    oMsg.addU16(Proto::OsLinux);
    oMsg.addU16(Proto::ClientVersion);

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
    Rsa::encrypt((char*)oMsg.getBuffer() + 6 + oMsg.getMessageSize() - 128, 128, Proto::RSA);

    send(oMsg);

    enableXteaEncryption();
}

void ProtocolGame::sendLogout()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientQuitGame);
    send(oMsg);
}

void ProtocolGame::sendPing()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientPingBack);
    send(oMsg);
}

// autowalk

void ProtocolGame::sendWalkNorth()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGoNorth);
    send(oMsg);
}

void ProtocolGame::sendWalkEast()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGoEast);
    send(oMsg);
}

void ProtocolGame::sendWalkSouth()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGoSouth);
    send(oMsg);
}

void ProtocolGame::sendWalkWest()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGoWest);
    send(oMsg);
}

void ProtocolGame::sendStopAutowalk()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientStop);
    send(oMsg);
}

void ProtocolGame::sendWalkNorthEast()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGoNorthEast);
    send(oMsg);
}

void ProtocolGame::sendWalkSouthEast()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGoSouthEast);
    send(oMsg);
}

void ProtocolGame::sendWalkSouthWest()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGoSouthWest);
    send(oMsg);
}

void ProtocolGame::sendWalkNorthWest()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGoNorthWest);
    send(oMsg);
}

void ProtocolGame::sendTurnNorth()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientRotateNorth);
    send(oMsg);
}

void ProtocolGame::sendTurnEast()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientRotateEast);
    send(oMsg);
}

void ProtocolGame::sendTurnSouth()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientRotateSouth);
    send(oMsg);
}

void ProtocolGame::sendTurnWest()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientRotateWest);
    send(oMsg);
}

void ProtocolGame::sendThrow(const Position& fromPos, int thingId, int stackpos, const Position& toPos, int count)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientMoveObject);
    addPosition(oMsg, fromPos);
    oMsg.addU16(thingId);
    oMsg.addU8(stackpos);
    addPosition(oMsg, toPos);
    oMsg.addU8(count);
    send(oMsg);
}

void ProtocolGame::sendLookInShop(int thingId, int count)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientInspectNpcTrade);
    oMsg.addU16(thingId);
    oMsg.addU8(count);
    send(oMsg);
}

void ProtocolGame::sendPlayerPurchase(int thingId, int count, int amount, bool ignoreCapacity, bool buyWithBackpack)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientBuyObject);
    oMsg.addU16(thingId);
    oMsg.addU8(count);
    oMsg.addU8(amount);
    oMsg.addU8(ignoreCapacity ? 0x01 : 0x00);
    oMsg.addU8(buyWithBackpack ? 0x01 : 0x00);
    send(oMsg);
}

void ProtocolGame::sendPlayerSale(int thingId, int count, int amount, bool ignoreEquipped)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientSellObject);
    oMsg.addU16(thingId);
    oMsg.addU8(count);
    oMsg.addU8(amount);
    oMsg.addU8(ignoreEquipped ? 0x01 : 0x00);
    send(oMsg);
}

void ProtocolGame::sendCloseShop()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientCloseNpcTrade);
    send(oMsg);
}

void ProtocolGame::sendRequestTrade(const Position& pos, int thingId, int stackpos, uint playerId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientTradeObject);
    addPosition(oMsg, pos);
    oMsg.addU16(thingId);
    oMsg.addU8(stackpos);
    oMsg.addU32(playerId);
    send(oMsg);
}

void ProtocolGame::sendLookInTrade(bool counterOffer, int index)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientInspectTrade);
    oMsg.addU8(counterOffer ? 0x01 : 0x00);
    oMsg.addU8(index);
    send(oMsg);
}

void ProtocolGame::sendAcceptTrade()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientAcceptTrade);
    send(oMsg);
}

void ProtocolGame::sendRejectTrade()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientRejectTrade);
    send(oMsg);
}

void ProtocolGame::sendUseItem(const Position& position, int itemId, int stackpos, int index)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientUseObject);
    addPosition(oMsg, position);
    oMsg.addU16(itemId);
    oMsg.addU8(stackpos);
    oMsg.addU8(index);
    send(oMsg);
}

void ProtocolGame::sendUseItemEx(const Position& fromPos, int fromThingId, int fromStackpos, const Position& toPos, int toThingId, int toStackpos)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientUseTwoObjects);
    addPosition(oMsg, fromPos);
    oMsg.addU16(fromThingId);
    oMsg.addU8(fromStackpos);
    addPosition(oMsg, toPos);
    oMsg.addU16(toThingId);
    oMsg.addU8(toStackpos);
    send(oMsg);
}

void ProtocolGame::sendUseOnCreature(const Position& pos, int thingId, int stackpos, uint creatureId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientUseOnCreature);
    addPosition(oMsg, pos);
    oMsg.addU16(thingId);
    oMsg.addU8(stackpos);
    oMsg.addU32(creatureId);
    send(oMsg);
}

void ProtocolGame::sendRotateItem(const Position& pos, int thingId, int stackpos)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientTurnObject);
    addPosition(oMsg, pos);
    oMsg.addU16(thingId);
    oMsg.addU8(stackpos);
    send(oMsg);
}

void ProtocolGame::sendCloseContainer(int containerId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientCloseContainer);
    oMsg.addU8(containerId);
    send(oMsg);
}

void ProtocolGame::sendUpContainer(int containerId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientUpContainer);
    oMsg.addU8(containerId);
    send(oMsg);
}

void ProtocolGame::sendTextWindow(uint windowTextId, const std::string& text)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientEditText);
    oMsg.addU32(windowTextId);
    oMsg.addString(text);
    send(oMsg);
}

void ProtocolGame::sendHouseWindow(int doorId, uint id, const std::string& text)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientEditList);
    oMsg.addU8(doorId);
    oMsg.addU32(id);
    oMsg.addString(text);
    send(oMsg);
}

void ProtocolGame::sendLookAt(const Position& position, int thingId, int stackpos)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientLook);
    addPosition(oMsg, position);
    oMsg.addU16(thingId);
    oMsg.addU8(stackpos);
    send(oMsg);
}

void ProtocolGame::sendTalk(const std::string& speakTypeDesc, int channelId, const std::string& receiver, const std::string& message)
{
    if(message.length() > 255 || message.length() <= 0)
        return;

    int speakType = Proto::translateSpeakTypeDesc(speakTypeDesc);

    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientTalk);
    oMsg.addU8(speakType);

    switch(speakType) {
    case Proto::SpeakPrivate:
    case Proto::SpeakPrivateRed:
        oMsg.addString(receiver);
        break;
    case Proto::SpeakChannelYellow:
    case Proto::SpeakChannelRed:
        oMsg.addU16(channelId);
        break;
    }

    oMsg.addString(message);
    send(oMsg);
}

void ProtocolGame::sendGetChannels()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGetChannels);
    send(oMsg);
}

void ProtocolGame::sendJoinChannel(int channelId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientJoinChannel);
    oMsg.addU16(channelId);
    send(oMsg);
}

void ProtocolGame::sendLeaveChannel(int channelId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientLeaveChannel);
    oMsg.addU16(channelId);
    send(oMsg);
}

void ProtocolGame::sendPrivateChannel(const std::string& receiver)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientPrivateChannel);
    oMsg.addString(receiver);
    send(oMsg);
}

// removed from game
// process report
// gm closes report
// cancel report

void ProtocolGame::sendCloseNpcChannel()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientCloseNpcChannel);
    send(oMsg);
}

void ProtocolGame::sendFightTatics(Otc::FightModes fightMode, Otc::ChaseModes chaseMode, bool safeFight)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientSetTactics);
    oMsg.addU8(fightMode);
    oMsg.addU8(chaseMode);
    oMsg.addU8(safeFight ? 0x01: 0x00);
    send(oMsg);
}

void ProtocolGame::sendAttack(uint creatureId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientAttack);
    oMsg.addU32(creatureId);
    oMsg.addU32(0);
    oMsg.addU32(0);
    send(oMsg);
}

void ProtocolGame::sendFollow(uint creatureId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientFollow);
    oMsg.addU32(creatureId);
    send(oMsg);
}

void ProtocolGame::sendInviteToParty(uint creatureId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientInviteToParty);
    oMsg.addU32(creatureId);
    send(oMsg);
}

void ProtocolGame::sendJoinParty(uint creatureId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientJoinParty);
    oMsg.addU32(creatureId);
    send(oMsg);
}

void ProtocolGame::sendRevokeInvitation(uint creatureId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientRevokeInvitation);
    oMsg.addU32(creatureId);
    send(oMsg);
}

void ProtocolGame::sendPassLeadership(uint creatureId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientPassLeadership);
    oMsg.addU32(creatureId);
    send(oMsg);
}

void ProtocolGame::sendLeaveParty()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientLeaveParty);
    send(oMsg);
}

void ProtocolGame::sendShareExperience(bool active, int unknown)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientShareExperience);
    oMsg.addU8(active ? 0x01 : 0x00);
    oMsg.addU8(unknown);
    send(oMsg);
}

void ProtocolGame::sendOpenChannel(int channelId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientOpenChannel);
    oMsg.addU16(channelId);
    send(oMsg);
}

void ProtocolGame::sendInviteToChannel(const std::string& name)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientInviteToChannel);
    oMsg.addString(name);
    send(oMsg);
}

void ProtocolGame::sendExcludeFromChannel(const std::string& name)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientExcludeFromChannel);
    oMsg.addString(name);
    send(oMsg);
}

void ProtocolGame::sendCancel()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientCancel);
    send(oMsg);
}

// update tile (not used)

void ProtocolGame::sendUpdateContainer()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientRefreshContainer);
    send(oMsg);
}

void ProtocolGame::sendGetOutfit()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGetOutfit);
    send(oMsg);
}

void ProtocolGame::sendSetOutfit(const Outfit& outfit)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientSetOutfit);

    oMsg.addU16(outfit.getId());
    oMsg.addU8(outfit.getHead());
    oMsg.addU8(outfit.getBody());
    oMsg.addU8(outfit.getLegs());
    oMsg.addU8(outfit.getFeet());
    oMsg.addU8(outfit.getAddons());

    send(oMsg);
}

void ProtocolGame::sendAddVip(const std::string& name)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientAddBuddy);
    oMsg.addString(name);
    send(oMsg);
}

void ProtocolGame::sendRemoveVip(uint playerId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientRemoveBuddy);
    oMsg.addU32(playerId);
    send(oMsg);
}

// bug report
// violation window
// debug assert

void ProtocolGame::sendGetQuestLog()
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGetQuestLog);
    send(oMsg);
}

void ProtocolGame::sendGetQuestLine(int questId)
{
    OutputMessage oMsg;
    oMsg.addU8(Proto::ClientGetQuestLine);
    oMsg.addU16(questId);
    send(oMsg);
}

void ProtocolGame::addPosition(OutputMessage& msg, const Position& position)
{
    msg.addU16(position.x);
    msg.addU16(position.y);
    msg.addU8(position.z);
}
