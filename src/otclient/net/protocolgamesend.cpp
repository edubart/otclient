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
    OutputMessage msg;

    msg.addU8(Proto::ClientEnterGame);

#ifdef WIN32
    msg.addU16(Proto::OsWindows);
#else
    msg.addU16(Proto::OsLinux);
#endif

    msg.addU16(Proto::ClientVersion);

    msg.addU8(0); // first RSA byte must be 0

    // xtea key
    generateXteaKey();
    msg.addU32(m_xteaKey[0]);
    msg.addU32(m_xteaKey[1]);
    msg.addU32(m_xteaKey[2]);
    msg.addU32(m_xteaKey[3]);

    msg.addU8(0); // is gm set?
    msg.addString(m_accountName);
    msg.addString(m_characterName);
    msg.addString(m_accountPassword);

    msg.addU32(timestamp);
    msg.addU8(unknown);

    // complete the 128 bytes for rsa encryption with zeros
    msg.addPaddingBytes(128 - (29 + m_accountName.length() + m_characterName.length() + m_accountPassword.length()));

    // encrypt with RSA
    Rsa::encrypt((char*)msg.getBuffer() + 6 + msg.getMessageSize() - 128, 128, Proto::RSA);

    send(msg);

    enableXteaEncryption();
}

void ProtocolGame::sendLogout()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientLeaveGame);
    send(msg);
}

void ProtocolGame::sendPingResponse()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientPingResponse);
    send(msg);
}

void ProtocolGame::sendAutoWalk(const std::vector<Otc::Direction>& path)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientAutoWalk);
    msg.addU8(path.size());
    for(Otc::Direction dir : path) {
        uint8 byte;
        switch(dir) {
            case Otc::East:
                byte = 1;
                break;
            case Otc::NorthEast:
                byte = 2;
                break;
            case Otc::North:
                byte = 3;
                break;
            case Otc::NorthWest:
                byte = 4;
                break;
            case Otc::West:
                byte = 5;
                break;
            case Otc::SouthWest:
                byte = 6;
                break;
            case Otc::South:
                byte = 7;
                break;
            case Otc::SouthEast:
                byte = 8;
                break;
            default:
                byte = 0;
                break;
        }
        msg.addU8(byte);
    }
    send(msg);
}

void ProtocolGame::sendWalkNorth()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientWalkNorth);
    send(msg);
}

void ProtocolGame::sendWalkEast()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientWalkEast);
    send(msg);
}

void ProtocolGame::sendWalkSouth()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientWalkSouth);
    send(msg);
}

void ProtocolGame::sendWalkWest()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientWalkWest);
    send(msg);
}

void ProtocolGame::sendStop()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientStop);
    send(msg);
}

void ProtocolGame::sendWalkNorthEast()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientWalkNorthEast);
    send(msg);
}

void ProtocolGame::sendWalkSouthEast()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientWalkSouthEast);
    send(msg);
}

void ProtocolGame::sendWalkSouthWest()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientWalkSouthWest);
    send(msg);
}

void ProtocolGame::sendWalkNorthWest()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientWalkNorthWest);
    send(msg);
}

void ProtocolGame::sendTurnNorth()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientTurnNorth);
    send(msg);
}

void ProtocolGame::sendTurnEast()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientTurnEast);
    send(msg);
}

void ProtocolGame::sendTurnSouth()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientTurnSouth);
    send(msg);
}

void ProtocolGame::sendTurnWest()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientTurnWest);
    send(msg);
}

void ProtocolGame::sendMove(const Position& fromPos, int thingId, int stackpos, const Position& toPos, int count)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientMove);
    addPosition(msg, fromPos);
    msg.addU16(thingId);
    msg.addU8(stackpos);
    addPosition(msg, toPos);
    msg.addU8(count);
    send(msg);
}

void ProtocolGame::sendInspectNpcTrade(int itemId, int count)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientInspectNpcTrade);
    msg.addU16(itemId);
    msg.addU8(count);
    send(msg);
}

void ProtocolGame::sendBuyItem(int itemId, int subType, int amount, bool ignoreCapacity, bool buyWithBackpack)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientBuyItem);
    msg.addU16(itemId);
    msg.addU8(subType);
    msg.addU8(amount);
    msg.addU8(ignoreCapacity ? 0x01 : 0x00);
    msg.addU8(buyWithBackpack ? 0x01 : 0x00);
    send(msg);
}

void ProtocolGame::sendSellItem(int itemId, int subType, int amount, bool ignoreEquipped)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientSellItem);
    msg.addU16(itemId);
    msg.addU8(subType);
    msg.addU8(amount);
    msg.addU8(ignoreEquipped ? 0x01 : 0x00);
    send(msg);
}

void ProtocolGame::sendCloseNpcTrade()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientCloseNpcTrade);
    send(msg);
}

void ProtocolGame::sendRequestTrade(const Position& pos, int thingId, int stackpos, uint creatureId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientRequestTrade);
    addPosition(msg, pos);
    msg.addU16(thingId);
    msg.addU8(stackpos);
    msg.addU32(creatureId);
    send(msg);
}

void ProtocolGame::sendInspectTrade(bool counterOffer, int index)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientInspectTrade);
    msg.addU8(counterOffer ? 0x01 : 0x00);
    msg.addU8(index);
    send(msg);
}

void ProtocolGame::sendAcceptTrade()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientAcceptTrade);
    send(msg);
}

void ProtocolGame::sendRejectTrade()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientRejectTrade);
    send(msg);
}

void ProtocolGame::sendUseItem(const Position& position, int itemId, int stackpos, int index)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientUseItem);
    addPosition(msg, position);
    msg.addU16(itemId);
    msg.addU8(stackpos);
    msg.addU8(index);
    send(msg);
}

void ProtocolGame::sendUseItemWith(const Position& fromPos, int itemId, int fromStackpos, const Position& toPos, int toThingId, int toStackpos)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientUseItemWith);
    addPosition(msg, fromPos);
    msg.addU16(itemId);
    msg.addU8(fromStackpos);
    addPosition(msg, toPos);
    msg.addU16(toThingId);
    msg.addU8(toStackpos);
    send(msg);
}

void ProtocolGame::sendUseOnCreature(const Position& pos, int thingId, int stackpos, uint creatureId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientUseOnCreature);
    addPosition(msg, pos);
    msg.addU16(thingId);
    msg.addU8(stackpos);
    msg.addU32(creatureId);
    send(msg);
}

void ProtocolGame::sendRotateItem(const Position& pos, int thingId, int stackpos)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientRotateItem);
    addPosition(msg, pos);
    msg.addU16(thingId);
    msg.addU8(stackpos);
    send(msg);
}

void ProtocolGame::sendCloseContainer(int containerId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientCloseContainer);
    msg.addU8(containerId);
    send(msg);
}

void ProtocolGame::sendUpContainer(int containerId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientUpContainer);
    msg.addU8(containerId);
    send(msg);
}

void ProtocolGame::sendEditText(uint id, const std::string& text)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientEditText);
    msg.addU32(id);
    msg.addString(text);
    send(msg);
}

void ProtocolGame::sendEditList(int listId, uint id, const std::string& text)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientEditList);
    msg.addU8(listId);
    msg.addU32(id);
    msg.addString(text);
    send(msg);
}

void ProtocolGame::sendLook(const Position& position, int thingId, int stackpos)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientLook);
    addPosition(msg, position);
    msg.addU16(thingId);
    msg.addU8(stackpos);
    send(msg);
}

void ProtocolGame::sendTalk(Otc::SpeakType speakType, int channelId, const std::string& receiver, const std::string& message)
{
    if(message.length() > 255 || message.length() <= 0)
        return;

    int serverSpeakType = Proto::translateSpeakTypeToServer(speakType);

    OutputMessage msg;
    msg.addU8(Proto::ClientTalk);
    msg.addU8(serverSpeakType);

    switch(serverSpeakType) {
    case Proto::ServerSpeakPrivate:
    case Proto::ServerSpeakPrivateRed:
        msg.addString(receiver);
        break;
    case Proto::ServerSpeakChannelYellow:
    case Proto::ServerSpeakChannelRed:
        msg.addU16(channelId);
        break;
    }

    msg.addString(message);
    send(msg);
}

void ProtocolGame::sendRequestChannels()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientRequestChannels);
    send(msg);
}

void ProtocolGame::sendJoinChannel(int channelId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientJoinChannel);
    msg.addU16(channelId);
    send(msg);
}

void ProtocolGame::sendLeaveChannel(int channelId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientLeaveChannel);
    msg.addU16(channelId);
    send(msg);
}

void ProtocolGame::sendOpenPrivateChannel(const std::string& receiver)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientOpenPrivateChannel);
    msg.addString(receiver);
    send(msg);
}

void ProtocolGame::sendCloseNpcChannel()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientCloseNpcChannel);
    send(msg);
}

void ProtocolGame::sendChangeFightModes(Otc::FightModes fightMode, Otc::ChaseModes chaseMode, bool safeFight)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientChangeFightModes);
    msg.addU8(fightMode);
    msg.addU8(chaseMode);
    msg.addU8(safeFight ? 0x01: 0x00);
    send(msg);
}

void ProtocolGame::sendAttack(uint creatureId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientAttack);
    msg.addU32(creatureId);
    msg.addU32(0);
    msg.addU32(0);
    send(msg);
}

void ProtocolGame::sendFollow(uint creatureId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientFollow);
    msg.addU32(creatureId);
    send(msg);
}

void ProtocolGame::sendInviteToParty(uint creatureId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientInviteToParty);
    msg.addU32(creatureId);
    send(msg);
}

void ProtocolGame::sendJoinParty(uint creatureId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientJoinParty);
    msg.addU32(creatureId);
    send(msg);
}

void ProtocolGame::sendRevokeInvitation(uint creatureId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientRevokeInvitation);
    msg.addU32(creatureId);
    send(msg);
}

void ProtocolGame::sendPassLeadership(uint creatureId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientPassLeadership);
    msg.addU32(creatureId);
    send(msg);
}

void ProtocolGame::sendLeaveParty()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientLeaveParty);
    send(msg);
}

void ProtocolGame::sendShareExperience(bool active, int unknown)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientShareExperience);
    msg.addU8(active ? 0x01 : 0x00);
    msg.addU8(unknown);
    send(msg);
}

void ProtocolGame::sendOpenOwnChannel()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientOpenOwnChannel);
    send(msg);
}

void ProtocolGame::sendInviteToOwnChannel(const std::string& name)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientInviteToOwnChannel);
    msg.addString(name);
    send(msg);
}

void ProtocolGame::sendExcludeFromOwnChannel(const std::string& name)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientExcludeFromOwnChannel);
    msg.addString(name);
    send(msg);
}

void ProtocolGame::sendCancelAttackAndFollow()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientCancelAttackAndFollow);
    send(msg);
}

void ProtocolGame::sendRefreshContainer()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientRefreshContainer);
    send(msg);
}

void ProtocolGame::sendRequestOutfit()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientRequestOutfit);
    send(msg);
}

void ProtocolGame::sendChangeOutfit(const Outfit& outfit)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientChangeOutfit);

    msg.addU16(outfit.getId());
    msg.addU8(outfit.getHead());
    msg.addU8(outfit.getBody());
    msg.addU8(outfit.getLegs());
    msg.addU8(outfit.getFeet());
    msg.addU8(outfit.getAddons());

    send(msg);
}

void ProtocolGame::sendAddVip(const std::string& name)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientAddVip);
    msg.addString(name);
    send(msg);
}

void ProtocolGame::sendRemoveVip(uint playerId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientRemoveVip);
    msg.addU32(playerId);
    send(msg);
}

void ProtocolGame::sendBugReport(const std::string& comment)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientBugReport);
    msg.addString(comment);
    send(msg);
}

void ProtocolGame::sendRuleVilation(const std::string& target, int reason, int action, const std::string& comment, const std::string& statement, int statementId, bool ipBanishment)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientRuleViolation);
    msg.addString(target);
    msg.addU8(reason);
    msg.addU8(action);
    msg.addString(comment);
    msg.addString(statement);
    msg.addU16(statementId);
    msg.addU8(ipBanishment);
    send(msg);
}

void ProtocolGame::sendDebugReport(const std::string& a, const std::string& b, const std::string& c, const std::string& d)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientDebugReport);
    msg.addString(a);
    msg.addString(b);
    msg.addString(c);
    msg.addString(d);
    send(msg);
}

void ProtocolGame::sendRequestQuestLog()
{
    OutputMessage msg;
    msg.addU8(Proto::ClientRequestQuestLog);
    send(msg);
}

void ProtocolGame::sendRequestQuestLine(int questId)
{
    OutputMessage msg;
    msg.addU8(Proto::ClientRequestQuestLine);
    msg.addU16(questId);
    send(msg);
}

void ProtocolGame::addPosition(OutputMessage& msg, const Position& position)
{
    msg.addU16(position.x);
    msg.addU16(position.y);
    msg.addU8(position.z);
}
