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
                parsePlayerLogin(msg);
                break;
            case Proto::GameServerGMActions:
                parseGMActions(msg);
                break;
            case Proto::GameServerLoginError:
                parseLoginError(msg);
                break;
            case Proto::GameServerLoginAdvice:
                parseFYIMessage(msg);
                break;
            case Proto::GameServerLoginWait:
                parseWaitList(msg);
                break;
            case Proto::GameServerPing:
                parsePing(msg);
                break;
            //case Proto::GameServerChallange:
            case Proto::GameServerDead:
                parseDeath(msg);
                break;
            case Proto::GameServerFullMap:
                parseMapDescription(msg);
                break;
            case Proto::GameServerMapTopRow:
                parseMoveNorth(msg);
                break;
            case Proto::GameServerMapRightRow:
                parseMoveEast(msg);
                break;
            case Proto::GameServerMapBottomRow:
                parseMoveSouth(msg);
                break;
            case Proto::GameServerMapLeftRow:
                parseMoveWest(msg);
                break;
            case Proto::GameServerTileData:
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
            case Proto::GameServerNpcOffer:
                parseOpenShopWindow(msg);
                break;
            case Proto::GameServerPlayerGoods:
                parsePlayerCash(msg);
                break;
            case Proto::GameServerCloseNpcTrade:
                parseCloseShopWindow(msg);
                break;
            case Proto::GameServerOwnOffer:
                parseSafeTradeRequest(msg);
                break;
            case Proto::GameServerCounterOffer:
                parseSafeTradeRequest(msg);
                break;
            case Proto::GameServerCloseTrade:
                parseSafeTradeClose(msg);
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
                parseCreatureSquare(msg);
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
            // case Proto::GameServerCreatureUnpass
            case Proto::GameServerEditText:
                parseItemTextWindow(msg);
                break;
            case Proto::GameServerEditList:
                parseHouseTextWindow(msg);
                break;
            case Proto::GameServerPlayerData:
                parsePlayerStats(msg);
                break;
            case Proto::GameServerPlayerSkills:
                parsePlayerSkills(msg);
                break;
            case Proto::GameServerPlayerState:
                parsePlayerIcons(msg);
                break;
            case Proto::GameServerClearTarget:
                parsePlayerCancelAttack(msg);
                break;
            //case Proto::GameServerSpellDelay:
            //case Proto::GameServerSpellGroupDelay:
            case Proto::GameServerTalk:
                parseCreatureSpeak(msg);
                break;
            case Proto::GameServerChannels:
                parseChannelList(msg);
                break;
            case Proto::GameServerOpenChannel:
                parseOpenChannel(msg);
                break;
            case Proto::GameServerPrivateChannel:
                parseOpenPrivatePlayerChat(msg);
                break;
            case Proto::GameServerRuleViolationChannel:
                msg.getU16();
                break;
            case Proto::GameServerRuleViolationRemove:
                msg.getString();
                break;
            case Proto::GameServerRuleViolationCancel:
                msg.getString();
                break;
            case Proto::GameServerRuleViolationLock:
                break;
            case Proto::GameServerOpenOwnChannel:
                parseCreatePrivateChannel(msg);
                break;
            case Proto::GameServerCloseChannel:
                parseClosePrivateChannel(msg);
                break;
            case Proto::GameServerMessage:
                parseTextMessage(msg);
                break;
            case Proto::GameServerSnapBack:
                parseCancelWalk(msg);
                break;
            //case Proto::GameServerWait:
            case Proto::GameServerTopFloor:
                parseFloorChangeUp(msg);
                break;
            case Proto::GameServerBottomFloor:
                parseFloorChangeDown(msg);
                break;
            case Proto::GameServerOutfit:
                parseOutfitWindow(msg);
                break;
            case Proto::GameServerBuddyData:
                parseVipState(msg);
                break;
            case Proto::GameServerBuddyLogin:
                parseVipLogin(msg);
                break;
            case Proto::GameServerBuddyLogout:
                parseVipLogout(msg);
                break;
            case Proto::GameServerTutorialHint:
                parseShowTutorial(msg);
                break;
            case Proto::GameServerAutomapFlag:
                parseAddMarker(msg);
                break;
            case Proto::GameServerQuestLog:
                parseQuestList(msg);
                break;
            case Proto::GameServerQuestLine:
                parseQuestPartList(msg);
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

void ProtocolGame::parsePlayerLogin(InputMessage& msg)
{
    uint playerId = msg.getU32();
    int serverBeat = msg.getU16();
    int playerCanReportBugs = msg.getU8();

    m_localPlayer = LocalPlayerPtr(new LocalPlayer);
    m_localPlayer->setId(playerId);
    m_localPlayer->setCanReportBugs(playerCanReportBugs);
    g_game.processLogin(m_localPlayer, serverBeat);
}

void ProtocolGame::parseGMActions(InputMessage& msg)
{
    for(int i = 0; i < Proto::NumViolationReasons; ++i)
        msg.getU8();
}

void ProtocolGame::parseLoginError(InputMessage& msg)
{
    std::string error = msg.getString();
    g_game.processLoginError(error);
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
#if PROTOCOL==862
    msg.getU8(); // 100 is a fair death. < 100 is a unfair death.
#endif
    g_game.processDeath();
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
    assert(thingId != 0);
    if(thingId == 0x0061 || thingId == 0x0062 || thingId == 0x0063) {
        parseCreatureTurn(msg);
    } else {
        ThingPtr thing = internalGetItem(msg, thingId);
        g_map.removeThingByPos(pos, stackPos);
        g_map.addThing(thing, pos, stackPos);
    }
}

void ProtocolGame::parseTileRemoveThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    int stackPos = msg.getU8();

    g_map.removeThingByPos(pos, stackPos);
}

void ProtocolGame::parseCreatureMove(InputMessage& msg)
{
    Position oldPos = parsePosition(msg);
    int oldStackpos = msg.getU8();
    Position newPos = parsePosition(msg);

    ThingPtr thing = g_map.getTile(oldPos)->getThing(oldStackpos);
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
    g_map.removeThing(thing);
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

    std::vector<ItemPtr> items;
    items.reserve(itemCount);
    for(int i = 0; i < itemCount; i++) {
        ItemPtr item = internalGetItem(msg);
        items.push_back(item);
    }

    g_lua.callGlobalField("Game", "onContainerOpen", containerId, itemId, name, capacity, hasParent, items);
}

void ProtocolGame::parseCloseContainer(InputMessage& msg)
{
    int containerId = msg.getU8();
    g_lua.callGlobalField("Game", "onContainerClose", containerId);
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
    g_lua.callGlobalField("Game", "onContainerUpdateItem", containerId, slot, item);
}

void ProtocolGame::parseContainerRemoveItem(InputMessage& msg)
{
    int containerId = msg.getU8();
    int slot = msg.getU8();
    g_lua.callGlobalField("Game", "onContainerRemoveItem", containerId, slot);
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

void ProtocolGame::parseOpenShopWindow(InputMessage& msg)
{
    int listCount = msg.getU8();
    for(int i = 0; i < listCount; ++i) {
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
    int size = msg.getU8();

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
    int count = msg.getU8();

    for(int i = 0; i < count; i++)
        internalGetItem(msg);
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

    MissilePtr shot = MissilePtr(new Missile());
    shot->setId(shotId);
    shot->setPath(fromPos, toPos);
    g_map.addThing(shot, fromPos);
}

void ProtocolGame::parseCreatureSquare(InputMessage& msg)
{
    uint id = msg.getU32();
    int color = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->addVolatileSquare(color);
}

void ProtocolGame::parseCreatureHealth(InputMessage& msg)
{
    uint id = msg.getU32();
    int healthPercent = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setHealthPercent(healthPercent);
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
}

void ProtocolGame::parseCreatureOutfit(InputMessage& msg)
{
    uint id = msg.getU32();
    Outfit outfit = internalGetOutfit(msg);

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setOutfit(outfit);
}

void ProtocolGame::parseCreatureSpeed(InputMessage& msg)
{
    uint id = msg.getU32();
    int speed = msg.getU16();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setSpeed(speed);
}

void ProtocolGame::parseCreatureSkulls(InputMessage& msg)
{
    uint id = msg.getU32();
    int skull = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setSkull(skull);
}

void ProtocolGame::parseCreatureShields(InputMessage& msg)
{
    uint id = msg.getU32();
    int shield = msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setShield(shield);
}

void ProtocolGame::parseCreatureTurn(InputMessage& msg)
{
    uint id = msg.getU32();
    Otc::Direction direction = (Otc::Direction)msg.getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->turn(direction);
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
    double health = msg.getU16();
    double maxHealth = msg.getU16();
    double freeCapacity = msg.getU32() / 100.0;
    double experience = msg.getU32();
    double level = msg.getU16();
    double levelPercent = msg.getU8();
    double mana = msg.getU16();
    double maxMana = msg.getU16();
    double magicLevel = msg.getU8();
    double magicLevelPercent = msg.getU8();
    double soul = msg.getU8();
    double stamina = msg.getU16();

    //TODO: move to game
    if(m_localPlayer->getStatistic(Otc::Health) != health ||
       m_localPlayer->getStatistic(Otc::MaxHealth) != maxHealth) {
        m_localPlayer->setStatistic(Otc::Health, health);
        m_localPlayer->setStatistic(Otc::MaxHealth, maxHealth);
        g_lua.callGlobalField("Game", "onHealthChange", health, maxHealth);
    }

    if(m_localPlayer->getStatistic(Otc::FreeCapacity) != freeCapacity) {
        m_localPlayer->setStatistic(Otc::FreeCapacity, freeCapacity);
        g_lua.callGlobalField("Game", "onFreeCapacityChange", freeCapacity);
    }

    if(m_localPlayer->getStatistic(Otc::Experience) != experience) {
        m_localPlayer->setStatistic(Otc::Experience, experience);
        g_lua.callGlobalField("Game", "onExperienceChange", experience);
    }

    if(m_localPlayer->getStatistic(Otc::Level) != level ||
       m_localPlayer->getStatistic(Otc::LevelPercent) != levelPercent) {
        m_localPlayer->setStatistic(Otc::Level, level);
        m_localPlayer->setStatistic(Otc::LevelPercent, levelPercent);
        g_lua.callGlobalField("Game", "onLevelChange", level, levelPercent);
    }

    if(m_localPlayer->getStatistic(Otc::Mana) != mana ||
       m_localPlayer->getStatistic(Otc::MaxMana) != maxMana) {
        m_localPlayer->setStatistic(Otc::Mana, mana);
        m_localPlayer->setStatistic(Otc::MaxMana, maxMana);
        g_lua.callGlobalField("Game", "onManaChange", mana, maxMana);
    }

    if(m_localPlayer->getStatistic(Otc::MagicLevel) != magicLevel ||
       m_localPlayer->getStatistic(Otc::MagicLevelPercent) != magicLevelPercent) {
        m_localPlayer->setStatistic(Otc::MagicLevel, magicLevel);
        m_localPlayer->setStatistic(Otc::MagicLevelPercent, magicLevelPercent);
        g_lua.callGlobalField("Game", "onMagicLevelChange", magicLevel, magicLevelPercent);
    }

    if(m_localPlayer->getStatistic(Otc::Soul) != soul) {
        m_localPlayer->setStatistic(Otc::Soul, soul);
        g_lua.callGlobalField("Game", "onSoulChange", soul);
    }

    if(m_localPlayer->getStatistic(Otc::Stamina) != stamina) {
        m_localPlayer->setStatistic(Otc::Stamina, stamina);
        g_lua.callGlobalField("Game", "onStaminaChange", stamina);
    }
}

void ProtocolGame::parsePlayerSkills(InputMessage& msg)
{
    for(int skill = 0; skill < Otc::LastSkill; skill++) {
        int values[Otc::LastSkillType];
        for(int skillType = 0; skillType < Otc::LastSkillType; skillType++) {
            values[skillType] = msg.getU8();
            m_localPlayer->setSkill((Otc::Skill)skill, (Otc::SkillType)skillType, values[skillType]);
        }

        g_lua.callGlobalField("Game", "onSkillChange", skill, values[Otc::SkillLevel], values[Otc::SkillPercent]);
    }
}

void ProtocolGame::parsePlayerIcons(InputMessage& msg)
{
    int icons = msg.getU16();
    m_localPlayer->setIcons((Otc::PlayerIcons)icons);
}

void ProtocolGame::parsePlayerCancelAttack(InputMessage& msg)
{
    msg.getU32(); // unknown
    g_game.processAttackCancel();
}

void ProtocolGame::parseCreatureSpeak(InputMessage& msg)
{
    msg.getU32(); // unkSpeak
    std::string name = msg.getString();
    int level = msg.getU16();
    int type = msg.getU8();
    int channelId = 0;
    Position creaturePos;

    switch(type) {
        case Proto::SpeakSay:
        case Proto::SpeakWhisper:
        case Proto::SpeakYell:
        case Proto::SpeakMonsterSay:
        case Proto::SpeakMonsterYell:
        case Proto::SpeakPrivateNpcToPlayer:
            creaturePos = parsePosition(msg);
            break;
        case Proto::SpeakChannelYellow:
        case Proto::SpeakChannelWhite:
        case Proto::SpeakChannelRed:
        case Proto::SpeakChannelRed2:
        case Proto::SpeakChannelOrange:
            channelId = msg.getU16();
            break;
        case Proto::SpeakPrivate:
        case Proto::SpeakPrivatePlayerToNpc:
        case Proto::SpeakBroadcast:
        case Proto::SpeakPrivateRed:
            break;
        case Proto::SpeakRVRChannel:
            msg.getU32();
            break;
        default:
            logTraceError("unknown speak type ", type);
            break;
    }

    std::string message = msg.getString();
    std::string typeDesc = Proto::translateSpeakType(type);

    g_game.processCreatureSpeak(name, level, typeDesc, message, channelId, creaturePos);
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

    g_lua.callGlobalField("Game", "onChannelList", channelList);
}

void ProtocolGame::parseOpenChannel(InputMessage& msg)
{
    int channelId = msg.getU16();
    std::string name = msg.getString();

    g_lua.callGlobalField("Game", "onOpenChannel", channelId, name);
}

void ProtocolGame::parseOpenPrivatePlayerChat(InputMessage& msg)
{
    msg.getString(); // name
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
    if(pos.z == 7)
        for(int i = 5; i >= 0; i--)
            setFloorDescription(msg, pos.x - 8, pos.y - 6, i, 18, 14, 8 - i, &skip);
    else if(pos.z > 7)
        setFloorDescription(msg, pos.x - 8, pos.y - 6, pos.z - 2, 18, 14, 3, &skip);

    pos.x++;
    pos.y++;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseFloorChangeDown(InputMessage& msg)
{
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

    g_lua.callGlobalField("Game", "onOpenOutfitWindow", creature, outfitList);
}

void ProtocolGame::parseVipState(InputMessage& msg)
{
    uint id = msg.getU32();
    std::string name = msg.getString();
    bool online = msg.getU8() != 0;

    g_lua.callGlobalField("Game", "onAddVip", id, name, online);
}

void ProtocolGame::parseVipLogin(InputMessage& msg)
{
    uint id = msg.getU32();

    g_lua.callGlobalField("Game", "onVipStateChange", id, true);
}

void ProtocolGame::parseVipLogout(InputMessage& msg)
{
    uint id = msg.getU32();

    g_lua.callGlobalField("Game", "onVipStateChange", id, false);
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
    int questsCount = msg.getU16();
    for(int i = 0; i < questsCount; i++) {
        msg.getU16(); // quest id
        msg.getString(); // quest name
        msg.getU8(); // quest state
    }
}

void ProtocolGame::parseQuestPartList(InputMessage& msg)
{
    msg.getU16(); // quest id
    int missionCount = msg.getU8();
    for(int i = 0; i < missionCount; i++) {
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
    int skip = *skipTiles;

    for(int nx = 0; nx < width; nx++) {
        for(int ny = 0; ny < height; ny++) {
            if(skip == 0) {
                int tileOpt = msg.getU16(true);
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
        int inspectTileId = msg.getU16(true);
        if(inspectTileId >= 0xFF00)
            return;
        else {
            if(stackPos >= 10)
                logTraceError("too many things, stackpos=", stackPos, " pos=", position);

            ThingPtr thing = internalGetThing(msg);
            if(thing)
                g_map.addThing(thing, position, 255);
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
                logTraceError("server says creature is known, but its not on creatures list");
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
        }

        uint8 healthPercent = msg.getU8();
        Otc::Direction direction = (Otc::Direction)msg.getU8();
        Outfit outfit = internalGetOutfit(msg);

        Light light;
        light.intensity = msg.getU8();
        light.color = msg.getU8();

        int speed = msg.getU16();
        int skull = msg.getU8();
        int shield = msg.getU8();

        int emblem = -1;
        if(thingId == 0x0061) // emblem is sent only in packet type 0x61
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

            if(creature == m_localPlayer) {
                m_localPlayer->setKnown(true);
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
