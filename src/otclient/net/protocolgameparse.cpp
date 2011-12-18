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
#include <framework/graphics/particlemanager.h>

void ProtocolGame::parseMessage(InputMessage& msg)
{
    try {
        while(!msg.eof()) {
            uint8 opt = msg.getU8();

            switch(opt) {
            case Otc::GameServerInitGame:
                parsePlayerLogin(msg);
                break;
            case 0x0B:
                parseGMActions(msg);
                break;
            case Otc::GameServerLoginError:
                parseLoginError(msg);
                break;
            case Otc::GameServerLoginAdvice:
                parseFYIMessage(msg);
                break;
            case Otc::GameServerLoginWait:
                parseWaitList(msg);
                break;
            case Otc::GameServerPing:
                parsePing(msg);
                break;
            //case Otc::GameServerChallange:
            case Otc::GameServerDead:
                parseDeath(msg);
                break;
            case Otc::GameServerFullMap:
                parseMapDescription(msg);
                break;
            case Otc::GameServerMapTopRow:
                parseMoveNorth(msg);
                break;
            case Otc::GameServerMapRightRow:
                parseMoveEast(msg);
                break;
            case Otc::GameServerMapBottomRow:
                parseMoveSouth(msg);
                break;
            case Otc::GameServerMapLeftRow:
                parseMoveWest(msg);
                break;
            case Otc::GameServerTileData:
                parseUpdateTile(msg);
                break;
            case Otc::GameServerCreateOnMap:
                parseTileAddThing(msg);
                break;
            case Otc::GameServerChangeOnMap:
                parseTileTransformThing(msg);
                break;
            case Otc::GameServerDeleteOnMap:
                parseTileRemoveThing(msg);
                break;
            case Otc::GameServerMoveCreature:
                parseCreatureMove(msg);
                break;
            case Otc::GameServerOpenContainer:
                parseOpenContainer(msg);
                break;
            case Otc::GameServerCloseContainer:
                parseCloseContainer(msg);
                break;
            case Otc::GameServerCreateContainer:
                parseContainerAddItem(msg);
                break;
            case Otc::GameServerChangeInContainer:
                parseContainerUpdateItem(msg);
                break;
            case Otc::GameServerDeleteInContainer:
                parseContainerRemoveItem(msg);
                break;
            case Otc::GameServerSetInventory:
                parseAddInventoryItem(msg);
                break;
            case Otc::GameServerDeleteInventory:
                parseRemoveInventoryItem(msg);
                break;
            case Otc::GameServerNpcOffer:
                parseOpenShopWindow(msg);
                break;
            case Otc::GameServerPlayerGoods:
                parsePlayerCash(msg);
                break;
            case Otc::GameServerCloseNpcTrade:
                parseCloseShopWindow(msg);
                break;
            case Otc::GameServerOwnOffer:
                parseSafeTradeRequest(msg);
                break;
            case Otc::GameServerCounterOffer:
                parseSafeTradeRequest(msg);
                break;
            case Otc::GameServerCloseTrade:
                parseSafeTradeClose(msg);
                break;
            case Otc::GameServerAmbient:
                parseWorldLight(msg);
                break;
            case Otc::GameServerGraphicalEffect:
                parseMagicEffect(msg);
                break;
            case Otc::GameServerTextEffect:
                parseAnimatedText(msg);
                break;
            case Otc::GameServerMissleEffect:
                parseDistanceMissile(msg);
                break;
            case Otc::GameServerMarkCreature:
                parseCreatureSquare(msg);
                break;
            //case Otc::GameServerTrappers
            case Otc::GameServerCreatureHealth:
                parseCreatureHealth(msg);
                break;
            case Otc::GameServerCreatureLight:
                parseCreatureLight(msg);
                break;
            case Otc::GameServerCreatureOutfit:
                parseCreatureOutfit(msg);
                break;
            case Otc::GameServerCreatureSpeed:
                parseCreatureSpeed(msg);
                break;
            case Otc::GameServerCreatureSkull:
                parseCreatureSkulls(msg);
                break;
            case Otc::GameServerCreatureParty:
                parseCreatureShields(msg);
                break;
            // case Otc::GameServerCreatureUnpass
            case Otc::GameServerEditText:
                parseItemTextWindow(msg);
                break;
            case Otc::GameServerEditList:
                parseHouseTextWindow(msg);
                break;
            case Otc::GameServerPlayerData:
                parsePlayerStats(msg);
                break;
            case Otc::GameServerPlayerSkills:
                parsePlayerSkills(msg);
                break;
            case Otc::GameServerPlayerState:
                parsePlayerIcons(msg);
                break;
            case Otc::GameServerClearTarget:
                parsePlayerCancelAttack(msg);
                break;
            //case Otc::GameServerSpellDelay:
            //case Otc::GameServerSpellGroupDelay:
            case Otc::GameServerTalk:
                parseCreatureSpeak(msg);
                break;
            case Otc::GameServerChannels:
                parseChannelList(msg);
                break;
            case Otc::GameServerOpenChannel:
                parseOpenChannel(msg);
                break;
            case Otc::GameServerPrivateChannel:
                parseOpenPrivatePlayerChat(msg);
                break;
            case Otc::GameServerRuleViolation:
            case Otc::GameServerRuleViolation1:
            case Otc::GameServerRuleViolation2:
            case Otc::GameServerRuleViolation3:
                parseOpenRuleViolation(msg);
                break;
            case Otc::GameServerOpenOwnChannel:
                parseCreatePrivateChannel(msg);
                break;
            case Otc::GameServerCloseChannel:
                parseClosePrivateChannel(msg);
                break;
            case Otc::GameServerMessage:
                parseTextMessage(msg);
                break;
            case Otc::GameServerSnapBack:
                parseCancelWalk(msg);
                break;
            //case Otc::GameServerWait:
            case Otc::GameServerTopFloor:
                parseFloorChangeUp(msg);
                break;
            case Otc::GameServerBottomFloor:
                parseFloorChangeDown(msg);
                break;
            case Otc::GameServerOutfit:
                parseOutfitWindow(msg);
                break;
            case Otc::GameServerBuddyData:
                parseVipState(msg);
                break;
            case Otc::GameServerBuddyLogin:
                parseVipLogin(msg);
                break;
            case Otc::GameServerBuddyLogout:
                parseVipLogout(msg);
                break;
            case Otc::GameServerTutorialHint:
                parseShowTutorial(msg);
                break;
            case Otc::GameServerAutomapFlag:
                parseAddMarker(msg);
                break;
            case Otc::GameServerQuestLog:
                parseQuestList(msg);
                break;
            case Otc::GameServerQuestLine:
                parseQuestPartList(msg);
                break;
            //case Otc::GameServerChannelEvent:
            //case Otc::GameServerObjectInfo:
            //case Otc::GameServerPlayerInventory:
            default:
                Fw::throwException("unknown opt byte ", (int)opt);
                break;
            }
        }
    } catch(Exception& e) {
        logTraceError(e.what());
    }
}

void ProtocolGame::parsePlayerLogin(InputMessage& msg)
{
    int playerId = msg.getU32();
    int playerDrawSpeed = msg.getU16();
    int playerCanReportBugs = msg.getU8();

    m_localPlayer = LocalPlayerPtr(new LocalPlayer);
    m_localPlayer->setId(playerId);
    m_localPlayer->setDrawSpeed(playerDrawSpeed);
    m_localPlayer->setCanReportBugs(playerCanReportBugs);
}

void ProtocolGame::parseGMActions(InputMessage& msg)
{
    for(uint8 i = 0; i < 28; ++i)
        msg.getU8();
}

void ProtocolGame::parseLoginError(InputMessage& msg)
{
    std::string error = msg.getString();
    g_dispatcher.addEvent(std::bind(&Game::processLoginError, &g_game, error));
}

void ProtocolGame::parseFYIMessage(InputMessage& msg)
{
    msg.getString(); // message
}

void ProtocolGame::parseWaitList(InputMessage& msg)
{
    msg.getString(); // message
    msg.getU8();// + 1 // time
}

void ProtocolGame::parsePing(InputMessage&)
{
    sendPing();
}

void ProtocolGame::parseDeath(InputMessage& msg)
{
    msg.getU8(); // 100 is a fair death. < 100 is a unfair death.
}

void ProtocolGame::parseMapDescription(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    g_map.setCentralPosition(pos);
    setMapDescription(msg, pos.x - 8, pos.y - 6, pos.z, 18, 14);
}

void ProtocolGame::parseMoveNorth(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.y--;

    setMapDescription(msg, pos.x - 8, pos.y - 6, pos.z, 18, 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMoveEast(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.x++;

    setMapDescription(msg, pos.x + 9, pos.y - 6, pos.z, 1, 14);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMoveSouth(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.y++;

    setMapDescription(msg, pos.x - 8, pos.y + 7, pos.z, 18, 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMoveWest(InputMessage& msg)
{
    Position pos = g_map.getCentralPosition();
    pos.x--;

    setMapDescription(msg, pos.x - 8, pos.y - 6, pos.z, 1, 14);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseUpdateTile(InputMessage& msg)
{
    logDebug("PARSE UPDATE TILE!");

    Position tilePos = parsePosition(msg);
    uint16 thingId = msg.getU16(true);
    if(thingId == 0xFF01) {
        msg.getU16();
        /*msg->AddByte(0);
        msg->AddByte(0xFF);*/
    }
    else  {
        setTileDescription(msg, tilePos);
        msg.getU16();
        /*msg->AddByte(0x01);
        msg->AddByte(0xFF);*/
    }
}

void ProtocolGame::parseTileAddThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    uint8 stackPos = msg.getU8();

    ThingPtr thing = internalGetThing(msg);
    g_map.addThing(thing, pos, stackPos);
}

void ProtocolGame::parseTileTransformThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    uint8 stackPos = msg.getU8();

    uint16 thingId = msg.getU16();
    if(thingId == 0x0061 || thingId == 0x0062 || thingId == 0x0063) {
        parseCreatureTurn(msg);
    }
    else {
        ThingPtr thing = internalGetItem(msg, thingId);
        g_map.removeThing(pos, stackPos);
        g_map.addThing(thing, pos, stackPos);
    }
}

void ProtocolGame::parseTileRemoveThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    uint8 stackPos = msg.getU8();

    g_map.removeThing(pos, stackPos);
}

void ProtocolGame::parseCreatureMove(InputMessage& msg)
{
    Position oldPos = parsePosition(msg);
    uint8 oldStackpos = msg.getU8();
    Position newPos = parsePosition(msg);

    ThingPtr thing = g_map.getTile(oldPos)->getThing(oldStackpos);
    assert(thing);
    CreaturePtr creature = thing->asCreature();
    assert(creature);
    creature->walk(newPos);

    // update map tiles
    g_map.removeThing(thing);
    g_map.addThing(thing, newPos);
}

void ProtocolGame::parseOpenContainer(InputMessage& msg)
{
    msg.getU8(); // cid
    msg.getU16(); // itemid
    msg.getString(); // name
    msg.getU8(); // capacity
    msg.getU8(); // hasParent
    uint8 size = msg.getU8(); // size

    for(uint32 i = 0; i < size; i++)
        internalGetItem(msg, 0xFFFF);
}

void ProtocolGame::parseCloseContainer(InputMessage& msg)
{
    msg.getU8(); // cid
}

void ProtocolGame::parseContainerAddItem(InputMessage& msg)
{
    msg.getU8(); // cid
    internalGetItem(msg, 0xFFFF);
}

void ProtocolGame::parseContainerUpdateItem(InputMessage& msg)
{
    msg.getU8(); // cid
    msg.getU8(); // slot
    internalGetItem(msg, 0xFFFF);
}

void ProtocolGame::parseContainerRemoveItem(InputMessage& msg)
{
    msg.getU8(); // cid
    msg.getU8(); // slot
}

void ProtocolGame::parseAddInventoryItem(InputMessage& msg)
{
    uint8 slot = msg.getU8();
    ItemPtr item = internalGetItem(msg, 0xFFFF);
    g_dispatcher.addEvent(std::bind(&Game::processInventoryChange, &g_game, slot, item));
}

void ProtocolGame::parseRemoveInventoryItem(InputMessage& msg)
{
    uint8 slot = msg.getU8();
    g_dispatcher.addEvent(std::bind(&Game::processInventoryChange, &g_game, slot, ItemPtr()));
}

void ProtocolGame::parseOpenShopWindow(InputMessage& msg)
{
    uint8 listCount = msg.getU8();
    for(uint8 i = 0; i < listCount; ++i) {
        msg.getU16(); // item id
        msg.getU8(); // runecharges
        msg.getString(); // item name
        msg.getU32(); // weight
        msg.getU32(); // buy price
        msg.getU32(); // sell price
    }
}

void ProtocolGame::parsePlayerCash(InputMessage& msg)
{
    msg.getU32();
    uint8 size = msg.getU8();

    for(int i = 0; i < size; i++) {
        msg.getU16(); // itemid
        msg.getU8(); // runecharges
    }
}

void ProtocolGame::parseCloseShopWindow(InputMessage&)
{
}

void ProtocolGame::parseSafeTradeRequest(InputMessage& msg)
{
    msg.getString(); // name
    uint8 count = msg.getU8();

    for(uint8 i = 0; i < count; i++)
        internalGetItem(msg, 0xFFFF);
}

void ProtocolGame::parseSafeTradeClose(InputMessage&)
{
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
    effect->startAnimation();

    TilePtr tile = g_map.getTile(pos);
    tile->addEffect(effect);

    // TODO: check if particles effect exists, if not, play standard sprite effect.
    //g_particleManager.load("particle.otpa");
}

void ProtocolGame::parseAnimatedText(InputMessage& msg)
{
    parsePosition(msg); // textPos
    msg.getU8(); // color
    msg.getString(); // text
}

void ProtocolGame::parseDistanceMissile(InputMessage& msg)
{
    Position fromPos = parsePosition(msg);
    Position toPos = parsePosition(msg);
    int shotId = msg.getU8();

    MissilePtr shot = MissilePtr(new Missile());
    shot->setId(shotId);
    shot->setPath(fromPos, toPos);
    g_map.addThing(shot, fromPos);
}

void ProtocolGame::parseCreatureSquare(InputMessage& msg)
{
    uint32 id = msg.getU32();
    uint8 color = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setSquareColor(color);
}

void ProtocolGame::parseCreatureHealth(InputMessage& msg)
{
    uint32 id = msg.getU32();
    uint8 healthPercent = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setHealthPercent(healthPercent);
}

void ProtocolGame::parseCreatureLight(InputMessage& msg)
{
    uint32 id = msg.getU32();

    Light light;
    light.intensity = msg.getU8();
    light.color = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setLight(light);
}

void ProtocolGame::parseCreatureOutfit(InputMessage& msg)
{
    uint32 id = msg.getU32();
    Outfit outfit = internalGetOutfit(msg);

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setOutfit(outfit);
}

void ProtocolGame::parseCreatureSpeed(InputMessage& msg)
{
    uint32 id = msg.getU32();
    uint16 speed = msg.getU16();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setSpeed(speed);
}

void ProtocolGame::parseCreatureSkulls(InputMessage& msg)
{
    uint32 id = msg.getU32();
    uint8 skull = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setSkull(skull);
}

void ProtocolGame::parseCreatureShields(InputMessage& msg)
{
    uint32 id = msg.getU32();
    uint8 shield = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setShield(shield);
}

void ProtocolGame::parseCreatureTurn(InputMessage& msg)
{
    uint32 id = msg.getU32();
    Otc::Direction direction = (Otc::Direction)msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setDirection(direction);
}

void ProtocolGame::parseItemTextWindow(InputMessage& msg)
{
    msg.getU32(); // windowId
    msg.getU16(); // itemid
    msg.getU16(); // max length
    msg.getString(); // text
    msg.getString(); // writter
    msg.getString(); // date
}

void ProtocolGame::parseHouseTextWindow(InputMessage& msg)
{
    msg.getU8(); // unknown
    msg.getU32(); // windowId
    msg.getString(); // text
}

void ProtocolGame::parsePlayerStats(InputMessage& msg)
{
    m_localPlayer->setStatistic(Otc::Health, msg.getU16());
    m_localPlayer->setStatistic(Otc::MaxHealth, msg.getU16());

    g_dispatcher.addEvent([=] {
        g_lua.callGlobalField("Game", "onHealthChange", m_localPlayer->getStatistic(Otc::Health), m_localPlayer->getStatistic(Otc::MaxHealth));
    });

    m_localPlayer->setStatistic(Otc::FreeCapacity, msg.getU32() / 100.0);

    g_dispatcher.addEvent([=] {
        g_lua.callGlobalField("Game", "onFreeCapacityChange", m_localPlayer->getStatistic(Otc::FreeCapacity));
    });

    m_localPlayer->setStatistic(Otc::Experience, msg.getU32());
    m_localPlayer->setStatistic(Otc::Level, msg.getU16());
    m_localPlayer->setStatistic(Otc::LevelPercent, msg.getU8());

    m_localPlayer->setStatistic(Otc::Mana, msg.getU16());
    m_localPlayer->setStatistic(Otc::MaxMana, msg.getU16());

    g_dispatcher.addEvent([=] {
        g_lua.callGlobalField("Game", "onManaChange", m_localPlayer->getStatistic(Otc::Mana), m_localPlayer->getStatistic(Otc::MaxMana));
    });

    m_localPlayer->setStatistic(Otc::MagicLevel, msg.getU8());
    m_localPlayer->setStatistic(Otc::MagicLevelPercent, msg.getU8());

    m_localPlayer->setStatistic(Otc::Soul, msg.getU8());

    g_dispatcher.addEvent([=] {
        g_lua.callGlobalField("Game", "onSoulChange", m_localPlayer->getStatistic(Otc::Soul));
    });

    m_localPlayer->setStatistic(Otc::Stamina, msg.getU16());
}

void ProtocolGame::parsePlayerSkills(InputMessage& msg)
{
    for(int skill = 0; skill < Otc::LastSkill; skill++) {
        int values[Otc::LastSkillType];
        for(int skillType = 0; skillType < Otc::LastSkillType; skillType++) {
            values[skillType] = msg.getU8();
            m_localPlayer->setSkill((Otc::Skill)skill, (Otc::SkillType)skillType, values[skillType]);
        }

        g_dispatcher.addEvent([=] {
            g_lua.callGlobalField("Game", "onSkillUpdate", skill, values[Otc::SkillLevel], values[Otc::SkillPercent]);
        });
    }
}

void ProtocolGame::parsePlayerIcons(InputMessage& msg)
{
    msg.getU16(); // icons
}

void ProtocolGame::parsePlayerCancelAttack(InputMessage& msg)
{
    msg.getU32();
}

void ProtocolGame::parseCreatureSpeak(InputMessage& msg)
{
    msg.getU32(); // unkSpeak
    std::string name = msg.getString(); // name
    int level = msg.getU16(); // level
    int type = msg.getU8();
    int channelId = 0;
    Position creaturePos;

    switch(type) {
        case Otc::SpeakSay:
        case Otc::SpeakWhisper:
        case Otc::SpeakYell:
        case Otc::SpeakMonsterSay:
        case Otc::SpeakMonsterYell:
        case Otc::SpeakPrivateNpcToPlayer:
            creaturePos = parsePosition(msg); // creaturePos
            break;
        case Otc::SpeakChannelRed:
        case Otc::SpeakChannelOrange:
        case Otc::SpeakChannelYellow:
        case Otc::SpeakChannelWhite:
            channelId = msg.getU16(); // channelId
            break;
        case Otc::SpeakPrivate:
        case Otc::SpeakPrivatePlayerToNpc:
        case Otc::SpeakBroadcast:
        case Otc::SpeakPrivateRed:
            break;
        default:
            logTraceError("unknown speak type ", type);
            break;
    }

    std::string message = msg.getString(); // message

    g_dispatcher.addEvent([=] {
        g_lua.callGlobalField("Game", "onCreatureSpeak", name, level, type, message, channelId, creaturePos);
    });
}

void ProtocolGame::parseChannelList(InputMessage& msg)
{
    int count = msg.getU8();
    std::vector<std::tuple<int, std::string> > channelList(count);
    for(int i = 0; i < count; i++) {
        int id = msg.getU16();
        std::string name = msg.getString();
        channelList.push_back(std::make_tuple(id, name));
    }

    g_dispatcher.addEvent([=] {
        g_lua.callGlobalField("Game", "onChannelList", channelList);
    });
}

void ProtocolGame::parseOpenChannel(InputMessage& msg)
{
    int channelId = msg.getU16();
    std::string name = msg.getString();

    g_dispatcher.addEvent([=] {
        g_lua.callGlobalField("Game", "onOpenChannel", channelId, name);
    });
}

void ProtocolGame::parseOpenPrivatePlayerChat(InputMessage& msg)
{
    msg.getString(); // name
}

void ProtocolGame::parseOpenRuleViolation(InputMessage& msg)
{
    msg.getU16(); // a
}

void ProtocolGame::parseCreatePrivateChannel(InputMessage& msg)
{
    msg.getU16(); // channel id
    msg.getString(); // channel name
}

void ProtocolGame::parseClosePrivateChannel(InputMessage& msg)
{
    msg.getU16(); // channel id
}

void ProtocolGame::parseTextMessage(InputMessage& msg)
{
    uint8 type = msg.getU8();
    std::string message = msg.getString();

    g_dispatcher.addEvent(std::bind(&Game::processTextMessage, &g_game, type, message));
}

void ProtocolGame::parseCancelWalk(InputMessage& msg)
{
    Otc::Direction direction = (Otc::Direction)msg.getU8();
    m_localPlayer->cancelWalk(direction);
}

void ProtocolGame::parseFloorChangeUp(InputMessage& msg)
{
    logTraceDebug("this function has never been tested.");

    Position pos = g_map.getCentralPosition();
    pos.z--;

    int32 skip = 0;
    if(pos.z == 7)
        for(int32 i = 5; i >= 0; i--)
            setFloorDescription(msg, pos.x - 8, pos.y - 6, i, 18, 14, 8 - i, &skip);
    else if(pos.z > 7)
        setFloorDescription(msg, pos.x - 8, pos.y - 6, pos.z - 2, 18, 14, 3, &skip);

    pos.x++;
    pos.y++;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseFloorChangeDown(InputMessage& msg)
{
    logTraceDebug("this function has never been tested.");

    Position pos = g_map.getCentralPosition();
    pos.z++;

    int skip = 0;
    if(pos.z == 8) {
        int j, i;
        for(i = pos.z, j = -1; i < pos.z + 3; ++i, --j)
            setFloorDescription(msg, pos.x - 8, pos.y - 6, i, 18, 14, j, &skip);
    }
    else if(pos.z > 8 && pos.z < 14)
        setFloorDescription(msg, pos.x - 8, pos.y - 6, pos.z + 2, 18, 14, -3, &skip);

    pos.x--;
    pos.y--;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseOutfitWindow(InputMessage& msg)
{
    Outfit outfit = internalGetOutfit(msg);

    typedef std::tuple<int, std::string, int> OutfitInfo;
    std::vector<OutfitInfo> outfitList;

    uint8 outfitCount = msg.getU8();
    for(int i = 0; i < outfitCount; i++) {
        uint16 outfitId = msg.getU16();
        std::string outfitName = msg.getString();
        uint8 outfitAddons = msg.getU8();

        outfitList.push_back(OutfitInfo(outfitId, outfitName, outfitAddons));
    }

    CreaturePtr creature = CreaturePtr(new Creature);
    creature->setXPattern(2);
    creature->setOutfit(outfit);

    g_dispatcher.addEvent([=] {
        g_lua.callGlobalField("Game", "onOpenOutfitWindow", creature, outfitList);
    });
}

void ProtocolGame::parseVipState(InputMessage& msg)
{
    int id = msg.getU32();
    std::string name = msg.getString();
    bool online = msg.getU8() != 0;

    g_dispatcher.addEvent([=] {
        g_lua.callGlobalField("Game", "onAddVip", id, name, online);
    });
}

void ProtocolGame::parseVipLogin(InputMessage& msg)
{
    int id = msg.getU32();

    g_dispatcher.addEvent([=] {
        g_lua.callGlobalField("Game", "onVipStateChange", id, true);
    });
}

void ProtocolGame::parseVipLogout(InputMessage& msg)
{
    int id = msg.getU32();

    g_dispatcher.addEvent([=] {
        g_lua.callGlobalField("Game", "onVipStateChange", id, false);
    });
}

void ProtocolGame::parseShowTutorial(InputMessage& msg)
{
    msg.getU8(); // tutorial id
}

void ProtocolGame::parseAddMarker(InputMessage& msg)
{
    parsePosition(msg); // position
    msg.getU8(); // icon
    msg.getString(); // message
}

void ProtocolGame::parseQuestList(InputMessage& msg)
{
    uint16 questsCount = msg.getU16();
    for(uint16 i = 0; i < questsCount; i++) {
        msg.getU16(); // quest id
        msg.getString(); // quest name
        msg.getU8(); // quest state
    }
}

void ProtocolGame::parseQuestPartList(InputMessage& msg)
{
    msg.getU16(); // quest id
    uint8 missionCount = msg.getU8();
    for(uint8 i = 0; i < missionCount; i++) {
        msg.getString(); // quest name
        msg.getString(); // quest description
    }
}

void ProtocolGame::setMapDescription(InputMessage& msg, int32 x, int32 y, int32 z, int32 width, int32 height)
{
    int startz, endz, zstep, skip = 0;

    if(z > 7) {
        startz = z - 2;
        endz = (15 < z+2) ? 15 : z+2;
        zstep = 1;
    }
    else {
        startz = 7;
        endz = 0;
        zstep = -1;
    }

    for(int nz = startz; nz != endz + zstep; nz += zstep)
        setFloorDescription(msg, x, y, nz, width, height, z - nz, &skip);
}

void ProtocolGame::setFloorDescription(InputMessage& msg, int32 x, int32 y, int32 z, int32 width, int32 height, int32 offset, int32* skipTiles)
{
    int32 skip = *skipTiles;

    for(int32 nx = 0; nx < width; nx++) {
        for(int32 ny = 0; ny < height; ny++) {
            if(skip == 0) {
                uint16 tileOpt = msg.getU16(true);
                if(tileOpt >= 0xFF00)
                    skip = (msg.getU16() & 0xFF);
                else {
                    Position pos(x + nx + offset, y + ny + offset, z);
                    setTileDescription(msg, pos);
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
        uint16 inspectTileId = msg.getU16(true);
        if(inspectTileId >= 0xFF00)
            return;
        else {
            if(stackPos >= 10)
                logTraceWarning("too many things");

            ThingPtr thing = internalGetThing(msg);
            g_map.addThing(thing, position, 255);
        }
        stackPos++;
    }
}

Outfit ProtocolGame::internalGetOutfit(InputMessage& msg)
{
    Outfit outfit;

    uint16 type = msg.getU16();
    if(type != 0) {
        uint8 head = msg.getU8();
        uint8 body = msg.getU8();
        uint8 legs = msg.getU8();
        uint8 feet = msg.getU8();
        uint8 addons = msg.getU8();

        outfit.setType(type);
        outfit.setHead(head);
        outfit.setBody(body);
        outfit.setLegs(legs);
        outfit.setFeet(feet);
        outfit.setAddons(addons);
    }
    else {
        uint16 type = msg.getU16();
        outfit.setType(type);
    }

    return outfit;
}

ThingPtr ProtocolGame::internalGetThing(InputMessage& msg)
{
    ThingPtr thing;

    uint16 thingId = msg.getU16();
    if(thingId == 0x0061 || thingId == 0x0062) { // add new creature
        CreaturePtr creature = CreaturePtr(new Creature);

        if(thingId == 0x0062) { //creature is known
            uint32 id = msg.getU32();

            CreaturePtr knownCreature = g_map.getCreatureById(id);
            if(knownCreature)
                creature = knownCreature;
        }
        else if(thingId == 0x0061) { //creature is not known
            uint32 removeId = msg.getU32();
            uint32 id = msg.getU32();
            std::string name = msg.getString();

            g_map.removeCreatureById(removeId);

            if(m_localPlayer->getId() == id)
                creature = m_localPlayer->asCreature();

            creature->setId(id);
            creature->setName(name);
        }

        uint8 healthPercent = msg.getU8();
        Otc::Direction direction = (Otc::Direction)msg.getU8();
        Outfit outfit = internalGetOutfit(msg);

        Light light;
        light.intensity = msg.getU8();
        light.color = msg.getU8();

        uint16 speed = msg.getU16();
        uint8 skull = msg.getU8();
        uint8 shield = msg.getU8();

        uint8 emblem = 0;
        if(thingId == 0x0061) // emblem is sent only in packet type 0x61
            emblem = msg.getU8();

        bool impassable = (msg.getU8() == 0); // impassable

        creature->setHealthPercent(healthPercent);
        creature->setDirection(direction);
        creature->setOutfit(outfit);
        creature->setLight(light);
        creature->setSpeed(speed);
        creature->setSkull(skull);
        creature->setShield(shield);
        creature->setEmblem(emblem);
        creature->setImpassable(impassable);
        creature->cancelWalk(direction);

        thing = creature;

        // login event is generated the first time that local player gets known
        if(!g_game.isOnline() && creature == m_localPlayer) {
            // this event must be scheduled because the entire map description is not known yet
            g_dispatcher.addEvent(std::bind(&Game::processLogin, &g_game, m_localPlayer));
        }
    }
    else if(thingId == 0x0063) { // creature turn
        parseCreatureTurn(msg);
    }
    else // item
        thing = internalGetItem(msg, thingId);

    return thing;
}

ItemPtr ProtocolGame::internalGetItem(InputMessage& msg, uint16 id)
{
    ItemPtr item = ItemPtr(new Item());

    if(id == 0xFFFF)
        id = msg.getU16();
    item->setId(id);

    ThingType *itemType = item->getType();
    if(itemType->properties[ThingType::IsStackable] || itemType->properties[ThingType::IsFluidContainer] || itemType->properties[ThingType::IsFluid])
        item->setData(msg.getU8());

    return item;
}

Position ProtocolGame::parsePosition(InputMessage& msg)
{
    uint16 x = msg.getU16();
    uint16 y = msg.getU16();
    uint8  z = msg.getU8();

    return Position(x, y, z);
}
