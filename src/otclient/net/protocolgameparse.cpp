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

#include <otclient/core/localplayer.h>
#include <otclient/core/thingstype.h>
#include <otclient/core/game.h>
#include <otclient/core/map.h>
#include <otclient/core/item.h>
#include <otclient/core/effect.h>
#include <otclient/core/missile.h>
#include <otclient/core/tile.h>
#include <otclient/luascript/luavaluecasts.h>
#include <framework/core/eventdispatcher.h>

void ProtocolGame::parseMessage(InputMessage& msg)
{
    try {
        while(!msg.eof()) {
            int opt = msg.getU8();

            switch(opt) {
            case Proto::GameServerInitGame:
                parseInitGame(msg);
                break;
            case Proto::GameServerGMActions:
                parseGMActions(msg);
                break;
            case Proto::GameServerLoginError:
                parseLoginError(msg);
                break;
            case Proto::GameServerLoginAdvice:
                parseLoginAdvice(msg);
                break;
            case Proto::GameServerLoginWait:
                parseLoginWait(msg);
                break;
            case Proto::GameServerPing:
                parsePing(msg);
                break;
            //case Proto::GameServerChallange:
            case Proto::GameServerDeath:
                parseDeath(msg);
                break;
            case Proto::GameServerFullMap:
                parseMapDescription(msg);
                break;
            case Proto::GameServerMapTopRow:
                parseMapMoveNorth(msg);
                break;
            case Proto::GameServerMapRightRow:
                parseMapMoveEast(msg);
                break;
            case Proto::GameServerMapBottomRow:
                parseMapMoveSouth(msg);
                break;
            case Proto::GameServerMapLeftRow:
                parseMapMoveWest(msg);
                break;
            case Proto::GameServerUpdateTile:
                parseUpdateTile(msg);
                break;
            case Proto::GameServerCreateOnMap:
                parseTileAddThing(msg);
                break;
            case Proto::GameServerChangeOnMap:
                parseTileTransformThing(msg);
                break;
            case Proto::GameServerDeleteOnMap:
                parseTileRemoveThing(msg);
                break;
            case Proto::GameServerMoveCreature:
                parseCreatureMove(msg);
                break;
            case Proto::GameServerOpenContainer:
                parseOpenContainer(msg);
                break;
            case Proto::GameServerCloseContainer:
                parseCloseContainer(msg);
                break;
            case Proto::GameServerCreateContainer:
                parseContainerAddItem(msg);
                break;
            case Proto::GameServerChangeInContainer:
                parseContainerUpdateItem(msg);
                break;
            case Proto::GameServerDeleteInContainer:
                parseContainerRemoveItem(msg);
                break;
            case Proto::GameServerSetInventory:
                parseAddInventoryItem(msg);
                break;
            case Proto::GameServerDeleteInventory:
                parseRemoveInventoryItem(msg);
                break;
            case Proto::GameServerOpenNpcTrade:
                parseOpenNpcTrade(msg);
                break;
            case Proto::GameServerPlayerGoods:
                parsePlayerGoods(msg);
                break;
            case Proto::GameServerCloseNpcTrade:
                parseCloseNpcTrade(msg);
                break;
            case Proto::GameServerOwnTrade:
                parseOpenTrade(msg);
                break;
            case Proto::GameServerCounterTrade:
                parseOpenTrade(msg);
                break;
            case Proto::GameServerCloseTrade:
                parseCloseTrade(msg);
                break;
            case Proto::GameServerAmbient:
                parseWorldLight(msg);
                break;
            case Proto::GameServerGraphicalEffect:
                parseMagicEffect(msg);
                break;
            case Proto::GameServerTextEffect:
                parseAnimatedText(msg);
                break;
            case Proto::GameServerMissleEffect:
                parseDistanceMissile(msg);
                break;
            case Proto::GameServerMarkCreature:
                parseCreatureMark(msg);
                break;
            //case Proto::GameServerTrappers
            case Proto::GameServerCreatureHealth:
                parseCreatureHealth(msg);
                break;
            case Proto::GameServerCreatureLight:
                parseCreatureLight(msg);
                break;
            case Proto::GameServerCreatureOutfit:
                parseCreatureOutfit(msg);
                break;
            case Proto::GameServerCreatureSpeed:
                parseCreatureSpeed(msg);
                break;
            case Proto::GameServerCreatureSkull:
                parseCreatureSkulls(msg);
                break;
            case Proto::GameServerCreatureParty:
                parseCreatureShields(msg);
                break;
            case Proto::GameServerCreatureUnpass:
                parseCreatureUnpass(msg);
                break;
            case Proto::GameServerEditText:
                parseEditText(msg);
                break;
            case Proto::GameServerEditList:
                parseEditList(msg);
                break;
            case Proto::GameServerPlayerData:
                parsePlayerStats(msg);
                break;
            case Proto::GameServerPlayerSkills:
                parsePlayerSkills(msg);
                break;
            case Proto::GameServerPlayerState:
                parsePlayerState(msg);
                break;
            case Proto::GameServerClearTarget:
                parsePlayerCancelAttack(msg);
                break;
#if PROTOCOL>=870
            case Proto::GameServerSpellDelay:
                parseSpellDelay(msg);
                break;
            case Proto::GameServerSpellGroupDelay:
                parseSpellGroupDelay(msg);
                break;
#endif
            case Proto::GameServerTalk:
                parseCreatureSpeak(msg);
                break;
            case Proto::GameServerChannels:
                parseChannelList(msg);
                break;
            case Proto::GameServerOpenChannel:
                parseOpenChannel(msg);
                break;
            case Proto::GameServerOpenPrivateChannel:
                parseOpenPrivateChannel(msg);
                break;
            case Proto::GameServerRuleViolationChannel:
                parseRuleViolationChannel(msg);
                break;
            case Proto::GameServerRuleViolationRemove:
                parseRuleViolationRemove(msg);
                break;
            case Proto::GameServerRuleViolationCancel:
                parseRuleViolationCancel(msg);
                break;
            case Proto::GameServerRuleViolationLock:
                parseRuleViolationLock(msg);
                break;
            case Proto::GameServerOpenOwnChannel:
                parseOpenOwnPrivateChannel(msg);
                break;
            case Proto::GameServerCloseChannel:
                parseCloseChannel(msg);
                break;
            case Proto::GameServerTextMessage:
                parseTextMessage(msg);
                break;
            case Proto::GameServerCancelWalk:
                parseCancelWalk(msg);
                break;
            //case Proto::GameServerWait:
            case Proto::GameServerFloorChangeUp:
                parseFloorChangeUp(msg);
                break;
            case Proto::GameServerFloorChangeDown:
                parseFloorChangeDown(msg);
                break;
            case Proto::GameServerChooseOutfit:
                parseOpenOutfitWindow(msg);
                break;
            case Proto::GameServerVipAdd:
                parseVipAdd(msg);
                break;
            case Proto::GameServerVipLogin:
                parseVipLogin(msg);
                break;
            case Proto::GameServerVipLogout:
                parseVipLogout(msg);
                break;
            case Proto::GameServerTutorialHint:
                parseTutorialHint(msg);
                break;
            case Proto::GameServerAutomapFlag:
                parseAutomapFlag(msg);
                break;
            case Proto::GameServerQuestLog:
                parseQuestLog(msg);
                break;
            case Proto::GameServerQuestLine:
                parseQuestLine(msg);
                break;
            //case Proto::GameServerChannelEvent:
            //case Proto::GameServerObjectInfo:
            //case Proto::GameServerPlayerInventory:
            default:
                Fw::throwException("unknown opt byte ", (int)opt);
                break;
            }
        }
    } catch(Exception& e) {
        logTraceError(e.what());
    }
}

void ProtocolGame::parseInitGame(InputMessage& msg)
{
    uint playerId = msg.getU32();
    int serverBeat = msg.getU16();
    bool canReportBugs = msg.getU8();

    m_localPlayer = LocalPlayerPtr(new LocalPlayer);
    m_localPlayer->setId(playerId);

    g_game.processGameStart(m_localPlayer, serverBeat, canReportBugs);
}

void ProtocolGame::parseGMActions(InputMessage& msg)
{
    std::vector<uint8> actions;
    for(int i = 0; i < Proto::NumViolationReasons; ++i)
        actions.push_back(msg.getU8());
    g_game.processGMActions(actions);
}

void ProtocolGame::parseLoginError(InputMessage& msg)
{
    std::string error = msg.getString();

    g_game.processLoginError(error);
}

void ProtocolGame::parseLoginAdvice(InputMessage& msg)
{
    std::string message = msg.getString();

    g_game.processLoginAdvice(message);
}

void ProtocolGame::parseLoginWait(InputMessage& msg)
{
    std::string message = msg.getString();
    int time = msg.getU8();

    g_game.processLoginWait(message, time);
}

void ProtocolGame::parsePing(InputMessage&)
{
    g_game.processPing();
    sendPingResponse();
}

void ProtocolGame::parseDeath(InputMessage& msg)
{
    int penality = 100;
#if PROTOCOL>=862
    penality = msg.getU8();
#endif
    g_game.processDeath(penality);
}

void ProtocolGame::parseMapDescription(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    g_map.setCentralPosition(pos);
    setMapDescription(msg, pos.x - Otc::AWARE_X_LEFT_TILES, pos.y - Otc::AWARE_Y_TOP_TILES, pos.z, Otc::AWARE_X_TILES, Otc::AWARE_Y_TILES);
}

void ProtocolGame::parseMapMoveNorth(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.y--;

    setMapDescription(msg, pos.x - Otc::AWARE_X_LEFT_TILES, pos.y - Otc::AWARE_Y_TOP_TILES, pos.z, Otc::AWARE_X_TILES, 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveEast(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.x++;

    setMapDescription(msg, pos.x + Otc::AWARE_X_RIGHT_TILES, pos.y - Otc::AWARE_Y_TOP_TILES, pos.z, 1, Otc::AWARE_Y_TILES);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveSouth(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.y++;

    setMapDescription(msg, pos.x - Otc::AWARE_X_LEFT_TILES, pos.y + Otc::AWARE_Y_BOTTOM_TILES, pos.z, Otc::AWARE_X_TILES, 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveWest(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.x--;

    setMapDescription(msg, pos.x - Otc::AWARE_X_LEFT_TILES, pos.y - Otc::AWARE_Y_TOP_TILES, pos.z, 1, Otc::AWARE_Y_TILES);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseUpdateTile(InputMessage& msg)
{
    Position tilePos = parsePosition(msg);
    int thingId = msg.getU16(true);
    if(thingId == 0xFF01) {
        msg.getU16();
    } else {
        setTileDescription(msg, tilePos);
        msg.getU16();
    }
}

void ProtocolGame::parseTileAddThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    int stackPos = msg.getU8();

    ThingPtr thing = internalGetThing(msg);
    g_map.addThing(thing, pos, stackPos);
}

void ProtocolGame::parseTileTransformThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    int stackPos = msg.getU8();

    int thingId = msg.getU16();
    if(thingId == 0x0061 || thingId == 0x0062 || thingId == 0x0063) {
        parseCreatureTurn(msg);
    } else {
        ThingPtr thing = internalGetItem(msg, thingId);
        if(!g_map.removeThingByPos(pos, stackPos))
            logTraceError("could not remove thing");
        g_map.addThing(thing, pos, stackPos);
    }
}

void ProtocolGame::parseTileRemoveThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    int stackPos = msg.getU8();

    if(!g_map.removeThingByPos(pos, stackPos))
        logTraceError("could not remove thing");
}

void ProtocolGame::parseCreatureMove(InputMessage& msg)
{
    Position oldPos = parsePosition(msg);
    int oldStackpos = msg.getU8();
    Position newPos = parsePosition(msg);

    ThingPtr thing = g_map.getThing(oldPos, oldStackpos);
    if(!thing) {
        logTraceError("could not get thing");
        return;
    }

    CreaturePtr creature = thing->asCreature();
    if(!creature) {
        logTraceError("thing is not a creature");
        return;
    }

    // update map tiles
    if(!g_map.removeThing(thing))
        logTraceError("could not remove thing");
    g_map.addThing(thing, newPos);

    g_game.processCreatureMove(creature, oldPos, newPos);
}

void ProtocolGame::parseOpenContainer(InputMessage& msg)
{
    int containerId = msg.getU8();
    int itemId = msg.getU16();
    std::string name = msg.getString();
    int capacity = msg.getU8();
    bool hasParent = (msg.getU8() != 0);
    int itemCount = msg.getU8();

    std::vector<ItemPtr> items(itemCount);
    for(int i = 0; i < itemCount; i++)
        items[i] = internalGetItem(msg);

    g_game.processOpenContainer(containerId, itemId, name, capacity, hasParent, items);
}

void ProtocolGame::parseCloseContainer(InputMessage& msg)
{
    int containerId = msg.getU8();
    g_game.processCloseContainer(containerId);
}

void ProtocolGame::parseContainerAddItem(InputMessage& msg)
{
    int containerId = msg.getU8();
    ItemPtr item = internalGetItem(msg);
    g_game.processContainerAddItem(containerId, item);
}

void ProtocolGame::parseContainerUpdateItem(InputMessage& msg)
{
    int containerId = msg.getU8();
    int slot = msg.getU8();
    ItemPtr item = internalGetItem(msg);
    g_game.processContainerUpdateItem(containerId, slot, item);
}

void ProtocolGame::parseContainerRemoveItem(InputMessage& msg)
{
    int containerId = msg.getU8();
    int slot = msg.getU8();
    g_game.processContainerRemoveItem(containerId, slot);
}

void ProtocolGame::parseAddInventoryItem(InputMessage& msg)
{
    int slot = msg.getU8();
    ItemPtr item = internalGetItem(msg);
    g_game.processInventoryChange(slot, item);
}

void ProtocolGame::parseRemoveInventoryItem(InputMessage& msg)
{
    int slot = msg.getU8();
    g_game.processInventoryChange(slot, ItemPtr());
}

void ProtocolGame::parseOpenNpcTrade(InputMessage& msg)
{
    std::vector<std::tuple<ItemPtr, std::string, int, int, int>> items;
    int listCount = msg.getU8();
    for(int i = 0; i < listCount; ++i) {
        uint16 itemId = msg.getU16();
        uint8 count = msg.getU8();

        ItemPtr item = Item::create(itemId);
        item->setCountOrSubType(count);

        std::string name = msg.getString();
        int weight = msg.getU32();
        int buyPrice = msg.getU32();
        int sellPrice = msg.getU32();
        items.push_back(std::make_tuple(item, name, weight, buyPrice, sellPrice));
    }

    g_game.processOpenNpcTrade(items);
}

void ProtocolGame::parsePlayerGoods(InputMessage& msg)
{
    std::vector<std::tuple<ItemPtr, int>> goods;

    int money = msg.getU32();
    int size = msg.getU8();
    for(int i = 0; i < size; i++) {
        int itemId = msg.getU16();
        int count = msg.getU8();

        goods.push_back(std::make_tuple(Item::create(itemId), count));
    }

    g_game.processPlayerGoods(money, goods);
}

void ProtocolGame::parseCloseNpcTrade(InputMessage&)
{
    g_game.processCloseNpcTrade();
}

void ProtocolGame::parseOpenTrade(InputMessage& msg)
{
    std::string name = msg.getString();
    int count = msg.getU8();

    std::vector<ItemPtr> items(count);
    for(int i = 0; i < count; i++)
        items[i] = internalGetItem(msg);

    g_game.processOpenTrade(name, items);
}

void ProtocolGame::parseCloseTrade(InputMessage&)
{
    g_game.processCloseTrade();
}

void ProtocolGame::parseWorldLight(InputMessage& msg)
{
    Light light;
    light.intensity = msg.getU8();
    light.color = msg.getU8();

    g_map.setLight(light);
}

void ProtocolGame::parseMagicEffect(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    int effectId = msg.getU8();

    EffectPtr effect = EffectPtr(new Effect());
    effect->setId(effectId);
    g_map.addThing(effect, pos);
}

void ProtocolGame::parseAnimatedText(InputMessage& msg)
{
    Position position = parsePosition(msg);
    int color = msg.getU8();
    std::string text = msg.getString();

    AnimatedTextPtr animatedText = AnimatedTextPtr(new AnimatedText);
    animatedText->setColor(color);
    animatedText->setText(text);
    g_map.addThing(animatedText, position);
}

void ProtocolGame::parseDistanceMissile(InputMessage& msg)
{
    Position fromPos = parsePosition(msg);
    Position toPos = parsePosition(msg);
    int shotId = msg.getU8();

    MissilePtr missile = MissilePtr(new Missile());
    missile->setId(shotId);
    missile->setPath(fromPos, toPos);
    g_map.addThing(missile, fromPos);
}

void ProtocolGame::parseCreatureMark(InputMessage& msg)
{
    uint id = msg.getU32();
    int color = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->addTimedSquare(color);
    else
        logTraceError("could not get greature");
}

void ProtocolGame::parseCreatureHealth(InputMessage& msg)
{
    uint id = msg.getU32();
    int healthPercent = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setHealthPercent(healthPercent);
    else
        logTraceError("could not get greature");
}

void ProtocolGame::parseCreatureLight(InputMessage& msg)
{
    uint id = msg.getU32();

    Light light;
    light.intensity = msg.getU8();
    light.color = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setLight(light);
    else
        logTraceError("could not get greature");
}

void ProtocolGame::parseCreatureOutfit(InputMessage& msg)
{
    uint id = msg.getU32();
    Outfit outfit = internalGetOutfit(msg);

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setOutfit(outfit);
    else
        logTraceError("could not get greature");
}

void ProtocolGame::parseCreatureSpeed(InputMessage& msg)
{
    uint id = msg.getU32();
    int speed = msg.getU16();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setSpeed(speed);
    else
        logTraceError("could not get greature");
}

void ProtocolGame::parseCreatureSkulls(InputMessage& msg)
{
    uint id = msg.getU32();
    int skull = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setSkull(skull);
    else
        logTraceError("could not get greature");
}

void ProtocolGame::parseCreatureShields(InputMessage& msg)
{
    uint id = msg.getU32();
    int shield = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setShield(shield);
    else
        logTraceError("could not get greature");
}

void ProtocolGame::parseCreatureUnpass(InputMessage& msg)
{
    uint id = msg.getU32();
    bool unpass = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setPassable(!unpass);
    else
        logTraceError("could not get greature");
}

void ProtocolGame::parseCreatureTurn(InputMessage& msg)
{
    uint id = msg.getU32();
    Otc::Direction direction = (Otc::Direction)msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->turn(direction);
    else
        logTraceError("could not get greature");
}

void ProtocolGame::parseEditText(InputMessage& msg)
{
    int id = msg.getU32();
    int itemId = msg.getU16();
    int maxLength = msg.getU16();
    std::string text = msg.getString();
    std::string writter = msg.getString();
    std::string date = msg.getString();

    g_game.processEditText(id, itemId, maxLength, text, writter, date);
}

void ProtocolGame::parseEditList(InputMessage& msg)
{
    int listId = msg.getU8();
    int id = msg.getU32();
    const std::string& text = msg.getString();

    g_game.processEditList(listId, id, text);
}

void ProtocolGame::parsePlayerStats(InputMessage& msg)
{
    double health = msg.getU16();
    double maxHealth = msg.getU16();
    double freeCapacity = msg.getU32() / 100.0;
#if PROTOCOL >= 870
    double experience = msg.getU64();
#else
    double experience = msg.getU32();
#endif
    double level = msg.getU16();
    double levelPercent = msg.getU8();
    double mana = msg.getU16();
    double maxMana = msg.getU16();
    double magicLevel = msg.getU8();
    double magicLevelPercent = msg.getU8();
    double soul = msg.getU8();
    double stamina = msg.getU16();

    g_game.processPlayerStats(health, maxHealth,
                              freeCapacity, experience,
                              level, levelPercent,
                              mana, maxMana,
                              magicLevel, magicLevelPercent,
                              soul, stamina);
}

void ProtocolGame::parsePlayerSkills(InputMessage& msg)
{
    if(!m_localPlayer) {
        logTraceError("there is no local player");
        return;
    }

    for(int skill = 0; skill < Otc::LastSkill; skill++) {
        int level = msg.getU8();
        int levelPercent = msg.getU8();

        m_localPlayer->setSkill((Otc::Skill)skill, level, levelPercent);
    }
}

void ProtocolGame::parsePlayerState(InputMessage& msg)
{
    if(!m_localPlayer) {
        logTraceError("there is no local player");
        return;
    }

    int states = msg.getU16();
    m_localPlayer->setStates((Otc::PlayerStates)states);
}

void ProtocolGame::parsePlayerCancelAttack(InputMessage& msg)
{
    msg.getU32(); // unknown
    g_game.processAttackCancel();
}


void ProtocolGame::parseSpellDelay(InputMessage& msg)
{
    msg.getU16(); // spell id
    msg.getU16(); // cooldown
    msg.getU8(); // unknown
}

void ProtocolGame::parseSpellGroupDelay(InputMessage& msg)
{
    msg.getU16(); // spell id
    msg.getU16(); // cooldown
    msg.getU8(); // unknown
}

void ProtocolGame::parseCreatureSpeak(InputMessage& msg)
{
    msg.getU32(); // unknown

    std::string name = msg.getString();
    int level = msg.getU16();
    int serverType = msg.getU8();
    int channelId = 0;
    Position creaturePos;

    switch(serverType) {
        case Proto::ServerSpeakSay:
        case Proto::ServerSpeakWhisper:
        case Proto::ServerSpeakYell:
        case Proto::ServerSpeakMonsterSay:
        case Proto::ServerSpeakMonsterYell:
        case Proto::ServerSpeakPrivateNpcToPlayer:
            creaturePos = parsePosition(msg);
            break;
        case Proto::ServerSpeakChannelYellow:
        case Proto::ServerSpeakChannelWhite:
        case Proto::ServerSpeakChannelRed:
        case Proto::ServerSpeakChannelRed2:
        case Proto::ServerSpeakChannelOrange:
            channelId = msg.getU16();
            break;
        case Proto::ServerSpeakPrivate:
        case Proto::ServerSpeakPrivatePlayerToNpc:
        case Proto::ServerSpeakBroadcast:
        case Proto::ServerSpeakPrivateRed:
            break;
        case Proto::ServerSpeakRVRChannel:
            msg.getU32();
            break;
        default:
            logTraceError("unknown speak type ", serverType);
            break;
    }

    std::string message = msg.getString();
    Otc::SpeakType type = Proto::translateSpeakTypeFromServer(serverType);

    g_game.processCreatureSpeak(name, level, type, message, channelId, creaturePos);
}

void ProtocolGame::parseChannelList(InputMessage& msg)
{
    int count = msg.getU8();
    std::vector<std::tuple<int, std::string>> channelList;
    for(int i = 0; i < count; i++) {
        int id = msg.getU16();
        std::string name = msg.getString();
        channelList.push_back(std::make_tuple(id, name));
    }

    g_game.processChannelList(channelList);
}

void ProtocolGame::parseOpenChannel(InputMessage& msg)
{
    int channelId = msg.getU16();
    std::string name = msg.getString();

    g_game.processOpenChannel(channelId, name);
}

void ProtocolGame::parseOpenPrivateChannel(InputMessage& msg)
{
    std::string name = msg.getString();

    g_game.processOpenPrivateChannel(name);
}

void ProtocolGame::parseOpenOwnPrivateChannel(InputMessage& msg)
{
    int channelId = msg.getU16();
    std::string name = msg.getString();

    g_game.processOpenOwnPrivateChannel(channelId, name);
}

void ProtocolGame::parseCloseChannel(InputMessage& msg)
{
    int channelId = msg.getU16();

    g_game.processCloseChannel(channelId);
}


void ProtocolGame::parseRuleViolationChannel(InputMessage& msg)
{
    int channelId = msg.getU16();

    g_game.processRuleViolationChannel(channelId);
}

void ProtocolGame::parseRuleViolationRemove(InputMessage& msg)
{
    std::string name = msg.getString();

    g_game.processRuleViolationRemove(name);
}

void ProtocolGame::parseRuleViolationCancel(InputMessage& msg)
{
    std::string name = msg.getString();

    g_game.processRuleViolationCancel(name);
}

void ProtocolGame::parseRuleViolationLock(InputMessage& msg)
{
    g_game.processRuleViolationLock();
}

void ProtocolGame::parseTextMessage(InputMessage& msg)
{
    int type = msg.getU8();

    std::string typeDesc = Proto::translateTextMessageType(type);
    std::string message = msg.getString();

    g_game.processTextMessage(typeDesc, message);
}

void ProtocolGame::parseCancelWalk(InputMessage& msg)
{
    Otc::Direction direction = (Otc::Direction)msg.getU8();

    g_game.processWalkCancel(direction);
}

void ProtocolGame::parseFloorChangeUp(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.z--;

    int skip = 0;
    if(pos.z == Otc::SEA_FLOOR)
        for(int i = Otc::SEA_FLOOR - Otc::AWARE_UNDEGROUND_FLOOR_RANGE; i >= 0; i--)
            setFloorDescription(msg, pos.x - Otc::AWARE_X_LEFT_TILES, pos.y - Otc::AWARE_Y_TOP_TILES, i, Otc::AWARE_X_TILES, Otc::AWARE_Y_TILES, 8 - i, &skip);
    else if(pos.z > Otc::SEA_FLOOR)
        setFloorDescription(msg, pos.x - Otc::AWARE_X_LEFT_TILES, pos.y - Otc::AWARE_Y_TOP_TILES, pos.z - Otc::AWARE_UNDEGROUND_FLOOR_RANGE, Otc::AWARE_X_TILES, Otc::AWARE_Y_TILES, 3, &skip);

    pos.x++;
    pos.y++;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseFloorChangeDown(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.z++;

    int skip = 0;
    if(pos.z == Otc::UNDERGROUND_FLOOR) {
        int j, i;
        for(i = pos.z, j = -1; i <= pos.z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE; ++i, --j)
            setFloorDescription(msg, pos.x - Otc::AWARE_X_LEFT_TILES, pos.y - Otc::AWARE_Y_TOP_TILES, i, Otc::AWARE_X_TILES, Otc::AWARE_Y_TILES, j, &skip);
    }
    else if(pos.z > Otc::UNDERGROUND_FLOOR && pos.z < Otc::MAX_Z-1)
        setFloorDescription(msg, pos.x - Otc::AWARE_X_LEFT_TILES, pos.y - Otc::AWARE_Y_TOP_TILES, pos.z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE, Otc::AWARE_X_TILES, Otc::AWARE_Y_TILES, -3, &skip);

    pos.x--;
    pos.y--;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseOpenOutfitWindow(InputMessage& msg)
{
    Outfit currentOutfit = internalGetOutfit(msg);

    std::vector<std::tuple<int, std::string, int>> outfitList;
    int outfitCount = msg.getU8();
    for(int i = 0; i < outfitCount; i++) {
        int outfitId = msg.getU16();
        std::string outfitName = msg.getString();
        int outfitAddons = msg.getU8();

        outfitList.push_back(std::make_tuple(outfitId, outfitName, outfitAddons));
    }

#if PROTOCOL>=870
    int mountCount = msg.getU8();
    for(int i=0;i<mountCount;++i) {
        msg.getU16(); // mount type
        msg.getString(); // mount name
    }
#endif

    g_game.processOpenOutfitWindow(currentOutfit, outfitList);
}

void ProtocolGame::parseVipAdd(InputMessage& msg)
{
    uint id = msg.getU32();
    std::string name = msg.getString();
    bool online = msg.getU8() != 0;

    g_game.processVipAdd(id, name, online);
}

void ProtocolGame::parseVipLogin(InputMessage& msg)
{
    uint id = msg.getU32();
    g_game.processVipStateChange(id, true);
}

void ProtocolGame::parseVipLogout(InputMessage& msg)
{
    uint id = msg.getU32();
    g_game.processVipStateChange(id, false);
}

void ProtocolGame::parseTutorialHint(InputMessage& msg)
{
    int id = msg.getU8(); // tutorial id
    g_game.processTutorialHint(id);
}

void ProtocolGame::parseAutomapFlag(InputMessage& msg)
{
    // ignored
    parsePosition(msg); // position
    msg.getU8(); // icon
    msg.getString(); // message
}

void ProtocolGame::parseQuestLog(InputMessage& msg)
{
    std::vector<std::tuple<int, std::string, bool>> questList;
    int questsCount = msg.getU16();
    for(int i = 0; i < questsCount; i++) {
        int id = msg.getU16();
        std::string name = msg.getString();
        bool completed = msg.getU8();
        questList.push_back(std::make_tuple(id, name, completed));
    }

    g_game.processQuestLog(questList);
}

void ProtocolGame::parseQuestLine(InputMessage& msg)
{
    std::vector<std::tuple<std::string, std::string>> questMissions;
    int questId = msg.getU16();
    int missionCount = msg.getU8();
    for(int i = 0; i < missionCount; i++) {
        std::string missionName = msg.getString();
        std::string missionDescrition = msg.getString();
        questMissions.push_back(std::make_tuple(missionName, missionDescrition));
    }

    g_game.processQuestLine(questId, questMissions);
}

void ProtocolGame::setMapDescription(InputMessage& msg, int32 x, int32 y, int32 z, int32 width, int32 height)
{
    int startz, endz, zstep, skip = 0;

    if(z > Otc::SEA_FLOOR) {
        startz = z - Otc::AWARE_UNDEGROUND_FLOOR_RANGE;
        endz = std::min(z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE, (int)Otc::MAX_Z);
        zstep = 1;
    }
    else {
        startz = Otc::SEA_FLOOR;
        endz = 0;
        zstep = -1;
    }

    for(int nz = startz; nz != endz + zstep; nz += zstep)
        setFloorDescription(msg, x, y, nz, width, height, z - nz, &skip);
}

void ProtocolGame::setFloorDescription(InputMessage& msg, int32 x, int32 y, int32 z, int32 width, int32 height, int32 offset, int32* skipTiles)
{
    int skip = *skipTiles;

    for(int nx = 0; nx < width; nx++) {
        for(int ny = 0; ny < height; ny++) {
            Position tilePos(x + nx + offset, y + ny + offset, z);

            // clean pre stored tiles
            g_map.cleanTile(tilePos);

            if(skip == 0) {
                int tileOpt = msg.getU16(true);
                if(tileOpt >= 0xFF00)
                    skip = (msg.getU16() & 0xFF);
                else {
                    setTileDescription(msg, tilePos);
                    skip = (msg.getU16() & 0xFF);
                }
            }
            else
                skip--;
        }
    }
    *skipTiles = skip;
}

void ProtocolGame::setTileDescription(InputMessage& msg, Position position)
{
    g_map.cleanTile(position);

    int stackPos = 0;
    while(true) {
        int inspectTileId = msg.getU16(true);
        if(inspectTileId >= 0xFF00)
            return;
        else {
            if(stackPos >= 10)
                logTraceError("too many things, stackpos=", stackPos, " pos=", position);

            ThingPtr thing = internalGetThing(msg);
            if(thing)
                g_map.addThing(thing, position, -1);
        }
        stackPos++;
    }
}

Outfit ProtocolGame::internalGetOutfit(InputMessage& msg)
{
    Outfit outfit;

    int id = msg.getU16();
    if(id != 0) {
        outfit.setCategory(ThingsType::Creature);
        int head = msg.getU8();
        int body = msg.getU8();
        int legs = msg.getU8();
        int feet = msg.getU8();
        int addons = msg.getU8();
#if PROTOCOL>=870
        msg.getU16(); // mount
#endif

        outfit.setId(id);
        outfit.setHead(head);
        outfit.setBody(body);
        outfit.setLegs(legs);
        outfit.setFeet(feet);
        outfit.setAddons(addons);
    }
    else {
        int id = msg.getU16();
        if(id == 0) {
            outfit.setCategory(ThingsType::Effect);
            outfit.setId(13);
        }
        else {
            outfit.setCategory(ThingsType::Item);
            outfit.setId(id);
        }
    }

    return outfit;
}

ThingPtr ProtocolGame::internalGetThing(InputMessage& msg)
{
    ThingPtr thing;

    int thingId = msg.getU16();
    assert(thingId != 0);
    if(thingId == 0x0061 || thingId == 0x0062) { // add new creature
        CreaturePtr creature;

        if(thingId == 0x0062) { //creature is known
            uint id = msg.getU32();

            CreaturePtr knownCreature = g_map.getCreatureById(id);
            if(knownCreature)
                creature = knownCreature;
            else
                logTraceError("server said that a creature is known, but it's not");
        } else if(thingId == 0x0061) { //creature is not known
            uint removeId = msg.getU32();
            uint id = msg.getU32();
            std::string name = msg.getString();

            if(name.length() > 0) // every creature name must start with a capital letter
                name[0] = toupper(name[0]);

            g_map.removeCreatureById(removeId);

            if(id == m_localPlayer->getId())
                creature = m_localPlayer;
            else if(id >= Proto::PlayerStartId && id < Proto::PlayerEndId)
                creature = PlayerPtr(new Player);
            else if(id >= Proto::MonsterStartId && id < Proto::MonsterEndId)
                creature = MonsterPtr(new Monster);
            else if(id >= Proto::NpcStartId && id < Proto::NpcEndId)
                creature = NpcPtr(new Npc);
            else
                logTraceError("creature id is invalid");

            creature->setId(id);
            creature->setName(name);

            g_map.addCreature(creature);
        }

        int healthPercent = msg.getU8();
        Otc::Direction direction = (Otc::Direction)msg.getU8();
        Outfit outfit = internalGetOutfit(msg);

        Light light;
        light.intensity = msg.getU8();
        light.color = msg.getU8();

        int speed = msg.getU16();
        int skull = msg.getU8();
        int shield = msg.getU8();

        // emblem is sent only when the creature is not known
        int emblem = -1;
        if(thingId == 0x0061)
            emblem = msg.getU8();

        bool passable = (msg.getU8() == 0);

        if(creature) {
            creature->setHealthPercent(healthPercent);
            creature->setDirection(direction);
            creature->setOutfit(outfit);
            creature->setLight(light);
            creature->setSpeed(speed);
            creature->setSkull(skull);
            creature->setShield(shield);
            if(emblem != -1)
                creature->setEmblem(emblem);
            creature->setPassable(passable);
            creature->setDirection(direction);

            // now that the local player is known, we can schedule login event
            if(creature == m_localPlayer && !m_localPlayer->isKnown()) {
                m_localPlayer->setKnown(true);
                g_eventDispatcher.addEvent([] { g_game.processLogin(); });
            }
        }

        thing = creature;
    } else if(thingId == 0x0063) { // creature turn
        parseCreatureTurn(msg);
    } else // item
        thing = internalGetItem(msg, thingId);

    return thing;
}

ItemPtr ProtocolGame::internalGetItem(InputMessage& msg, int id)
{
    if(id == 0)
        id = msg.getU16();

    ItemPtr item = Item::create(id);
    if(item->isStackable() || item->isFluidContainer() || item->isFluid())
        item->setCountOrSubType(msg.getU8());

    return item;
}

Position ProtocolGame::parsePosition(InputMessage& msg)
{
    uint16 x = msg.getU16();
    uint16 y = msg.getU16();
    uint8  z = msg.getU8();

    return Position(x, y, z);
}
