#include "protocolgame.h"

#include <otclient/core/localplayer.h>
#include <otclient/core/datmanager.h>
#include <otclient/core/game.h>
#include <otclient/core/map.h>
#include <otclient/core/effect.h>

void ProtocolGame::parseMessage(InputMessage& msg)
{
    while(!msg.eof()) {
        uint8 opt = msg.getU8();
        //dump << "protocol id:" << std::hex << (int)opt;

        switch(opt) {
            case 0x0A:
                parsePlayerLogin(msg);
                break;
            case 0x0B:
                parseGMActions(msg);
                break;
            case 0x14:
                parseErrorMessage(msg);
                break;
            case 0x15:
                parseFYIMessage(msg);
                break;
            case 0x16:
                parseWaitList(msg);
                break;
            case 0x1E:
                parsePing(msg);
                break;
            case 0x28:
                parseDeath(msg);
                break;
            case 0x32:
                parseCanReportBugs(msg);
                break;
            case 0x64:
                parseMapDescription(msg);
                break;
            case 0x65:
                parseMoveNorth(msg);
                break;
            case 0x66:
                parseMoveEast(msg);
                break;
            case 0x67:
                parseMoveSouth(msg);
                break;
            case 0x68:
                parseMoveWest(msg);
                break;
            case 0x69:
                parseUpdateTile(msg);
                break;
            case 0x6A:
                parseTileAddThing(msg);
                break;
            case 0x6B:
                parseTileTransformThing(msg);
                break;
            case 0x6C:
                parseTileRemoveThing(msg);
                break;
            case 0x6D:
                parseCreatureMove(msg);
                break;
            case 0x6E:
                parseOpenContainer(msg);
                break;
            case 0x6F:
                parseCloseContainer(msg);
                break;
            case 0x70:
                parseContainerAddItem(msg);
                break;
            case 0x71:
                parseContainerUpdateItem(msg);
                break;
            case 0x72:
                parseContainerRemoveItem(msg);
                break;
            case 0x78:
                parseAddInventoryItem(msg);
                break;
            case 0x79:
                parseRemoveInventoryItem(msg);
                break;
            case 0x7A:
                parseOpenShopWindow(msg);
                break;
            case 0x7B:
                parsePlayerCash(msg);
                break;
            case 0x7C:
                parseCloseShopWindow(msg);
                break;
            case 0x7D:
                parseSafeTradeRequest(msg);
                break;
            case 0x7E:
                parseSafeTradeRequest(msg);
                break;
            case 0x7F:
                parseSafeTradeClose(msg);
                break;
            case 0x82:
                parseWorldLight(msg);
                break;
            case 0x83:
                parseMagicEffect(msg);
                break;
            case 0x84:
                parseAnimatedText(msg);
                break;
            case 0x85:
                parseDistanceShot(msg);
                break;
            case 0x86:
                parseCreatureSquare(msg);
                break;
            case 0x8C:
                parseCreatureHealth(msg);
                break;
            case 0x8D:
                parseCreatureLight(msg);
                break;
            case 0x8E:
                parseCreatureOutfit(msg);
                break;
            case 0x8F:
                parseCreatureSpeed(msg);
                break;
            case 0x90:
                parseCreatureSkulls(msg);
                break;
            case 0x91:
                parseCreatureShields(msg);
                break;
            case 0x96:
                parseItemTextWindow(msg);
                break;
            case 0x97:
                parseHouseTextWindow(msg);
                break;
            case 0xA0:
                parsePlayerStats(msg);
                break;
            case 0xA1:
                parsePlayerSkills(msg);
                break;
            case 0xA2:
                parsePlayerIcons(msg);
                break;
            case 0xA3:
                parsePlayerCancelAttack(msg);
                break;
            case 0xAA:
                parseCreatureSpeak(msg);
                break;
            case 0xAB:
                parseChannelList(msg);
                break;
            case 0xAC:
                parseOpenChannel(msg);
                break;
            case 0xAD:
                parseOpenPrivatePlayerChat(msg);
                break;
            case 0xAE:
                parseOpenRuleViolation(msg);
                break;
            case 0xAF:
                parseRuleViolationAF(msg);
                break;
            case 0xB0:
                parseRuleViolationB0(msg);
                break;
            case 0xB1:
                parseRuleViolationB1(msg);
                break;
            case 0xB2:
                parseCreatePrivateChannel(msg);
                break;
            case 0xB3:
                parseClosePrivateChannel(msg);
                break;
            case 0xB4:
                parseTextMessage(msg);
                break;
            case 0xB5:
                parseCancelWalk(msg);
                break;
            case 0xBE:
                parseFloorChangeUp(msg);
                break;
            case 0xBF:
                parseFloorChangeDown(msg);
                break;
            case 0xC8:
                parseOutfitWindow(msg);
                break;
            case 0xD2:
                parseVipState(msg);
                break;
            case 0xD3:
                parseVipLogin(msg);
                break;
            case 0xD4:
                parseVipLogout(msg);
                break;
            case 0xDC:
                parseShowTutorial(msg);
                break;
            case 0xDD:
                parseAddMarker(msg);
                break;
            case 0xF0:
                parseQuestList(msg);
                break;
            case 0xF1:
                parseQuestPartList(msg);
                break;
            default:
                logError("UNKNOWN PACKET OPT BYTE: 0x", std::hex, opt);
                break;
        }
    }
}

void ProtocolGame::parsePlayerLogin(InputMessage& msg)
{
    int playerId = msg.getU32();
    int playerDrawSpeed = msg.getU16();
    int playerCanReportBugs = msg.getU8();

    g_game.onLogin();

    m_localPlayer = g_game.getLocalPlayer();
    m_localPlayer->setId(playerId);
    m_localPlayer->setDrawSpeed(playerDrawSpeed);
    m_localPlayer->setCanReportBugs(playerCanReportBugs);
}

void ProtocolGame::parseGMActions(InputMessage& msg)
{
    for(uint8 i = 0; i < 28; ++i)
        msg.getU8();
}

void ProtocolGame::parseErrorMessage(InputMessage& msg)
{
    std::string error = msg.getString();
    callLuaField("onError", error);
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

void ProtocolGame::parseDeath(InputMessage&)
{

}

void ProtocolGame::parseCanReportBugs(InputMessage& msg)
{
    msg.getU8(); // report bugs
}

void ProtocolGame::parseMapDescription(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    m_localPlayer->setPosition(pos);

    // we must clean, creatures and all.
    g_map.clean();

    // now we get new map.
    setMapDescription(msg, pos.x - 8, pos.y - 6, pos.z, 18, 14);
}

void ProtocolGame::parseMoveNorth(InputMessage& msg)
{
    Position pos = m_localPlayer->getPosition();
    setMapDescription(msg, pos.x - 8, pos.y - 6, pos.z, 18, 1);
}

void ProtocolGame::parseMoveEast(InputMessage& msg)
{
    Position pos = m_localPlayer->getPosition();
    setMapDescription(msg, pos.x + 9, pos.y - 6, pos.z, 1, 14);
}

void ProtocolGame::parseMoveSouth(InputMessage& msg)
{
    Position pos = m_localPlayer->getPosition();
    setMapDescription(msg, pos.x - 8, pos.y + 7, pos.z, 18, 1);
}

void ProtocolGame::parseMoveWest(InputMessage& msg)
{
    Position pos = m_localPlayer->getPosition();
    setMapDescription(msg, pos.x - 8, pos.y - 6, pos.z, 1, 14);
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
    uint8 stackpos = msg.getU8();

    ThingPtr thing = internalGetThing(msg);
    thing->setPosition(pos);

    g_map.addThing(thing, stackpos);
}

void ProtocolGame::parseTileTransformThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    uint8 stackpos = msg.getU8();

    uint16 thingId = msg.getU16();
    if(thingId == 0x0061 || thingId == 0x0062 || thingId == 0x0063) {
        parseCreatureTurn(msg);
    }
    else {
        ThingPtr thing = internalGetItem(msg, thingId);
        thing->setPosition(pos);

        g_map.removeThing(pos, stackpos);
        g_map.addThing(thing, stackpos);
    }
}

void ProtocolGame::parseTileRemoveThing(InputMessage& msg)
{
    Position pos = parsePosition(msg);
    uint8 stackpos = msg.getU8();

    g_map.removeThing(pos, stackpos);
}

void ProtocolGame::parseCreatureMove(InputMessage& msg)
{
    Position oldPos = parsePosition(msg);
    uint8 oldStackpos = msg.getU8();
    Position newPos = parsePosition(msg);

    ThingPtr thing = g_map.getThing(oldPos, oldStackpos);
    if(thing) {
        CreaturePtr creature = thing->asCreature();
        if(creature)
            creature->walk(newPos);
    }
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
    msg.getU8(); // slot
    internalGetItem(msg, 0xFFFF);
}

void ProtocolGame::parseRemoveInventoryItem(InputMessage& msg)
{
    msg.getU8(); // slot
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
    EffectPtr effect = EffectPtr(new Effect());
    effect->setPosition(parsePosition(msg));
    effect->setId(msg.getU8());

    g_map.addThing(effect);
}

void ProtocolGame::parseAnimatedText(InputMessage& msg)
{
    parsePosition(msg); // textPos
    msg.getU8(); // color
    msg.getString(); // text
}

void ProtocolGame::parseDistanceShot(InputMessage& msg)
{
    parsePosition(msg); // fromPos
    parsePosition(msg); // toPos
    msg.getU8(); // effect
}

void ProtocolGame::parseCreatureSquare(InputMessage& msg)
{
    msg.getU32(); // creatureId
    msg.getU8(); // color
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
    Direction direction = (Direction)msg.getU8();

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
    msg.getU16(); // health
    msg.getU16(); // max health
    msg.getU32(); // free capacity
    msg.getU32(); // experience
    msg.getU16(); // level
    msg.getU8(); // level percent
    msg.getU16(); // mana
    msg.getU16(); // max mana
    msg.getU8(); // magic level
    msg.getU8(); // magic level percent
    msg.getU8(); // soul
    msg.getU16(); // stamina
}

void ProtocolGame::parsePlayerSkills(InputMessage& msg)
{
    msg.getU8(); // fist skill
    msg.getU8(); // fist percent

    msg.getU8(); // club skill
    msg.getU8(); // club percent

    msg.getU8(); // sword skill
    msg.getU8(); // sword percent

    msg.getU8(); // axe skill
    msg.getU8(); // axe percent

    msg.getU8(); // distance skill
    msg.getU8(); // distance percent

    msg.getU8(); // shielding skill
    msg.getU8(); // shielding percent

    msg.getU8(); // fishing skill
    msg.getU8(); // fishing percent
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
    msg.getString(); // name
    msg.getU16(); // level
    uint8 type = msg.getU8();

    switch(type) {
        case SPEAK_SAY:
        case SPEAK_WHISPER:
        case SPEAK_YELL:
        case SPEAK_MONSTER_SAY:
        case SPEAK_MONSTER_YELL:
        case SPEAK_PRIVATE_NP:
            parsePosition(msg); // creaturePos
            break;
        case SPEAK_CHANNEL_R1:
        case SPEAK_CHANNEL_O:
        case SPEAK_CHANNEL_Y:
        case SPEAK_CHANNEL_W:
            msg.getU16(); // channelId
            break;
        case SPEAK_PRIVATE:
        case SPEAK_PRIVATE_PN:
        case SPEAK_BROADCAST:
        case SPEAK_PRIVATE_RED:
            break;
        default:
            logDebug("[ProtocolGame::parseCreatureSpeak]: Unknown speak type.", (int)type);
            break;
    }

    msg.getString(); // message
}

void ProtocolGame::parseChannelList(InputMessage& msg)
{
    uint8 count = msg.getU8();
    for(uint8 i = 0; i < count; i++) {
        msg.getU16();
        msg.getString();
    }
}

void ProtocolGame::parseOpenChannel(InputMessage& msg)
{
    msg.getU16(); // channelId
    msg.getString(); // name
}

void ProtocolGame::parseOpenPrivatePlayerChat(InputMessage& msg)
{
    msg.getString(); // name
}

void ProtocolGame::parseOpenRuleViolation(InputMessage& msg)
{
    msg.getU16(); // a
}

void ProtocolGame::parseRuleViolationAF(InputMessage& msg)
{
    msg.getU16(); // a
}

void ProtocolGame::parseRuleViolationB0(InputMessage& msg)
{
    msg.getU16(); // a
}

void ProtocolGame::parseRuleViolationB1(InputMessage& msg)
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
    msg.getU8(); // messageType
    std::string message = msg.getString();
    //logDebug(message);
}

void ProtocolGame::parseCancelWalk(InputMessage& msg)
{
    Direction direction = (Direction)msg.getU8();
    g_game.getLocalPlayer()->setDirection(direction);
}

void ProtocolGame::parseFloorChangeUp(InputMessage& msg)
{
    Position pos = m_localPlayer->getPosition();
    pos.z--;

    int32 skip = 0;
    if(m_localPlayer->getPosition().z == 7)
        for(int32 i = 5; i >= 0; i--)
            setFloorDescription(msg, pos.x - 8, pos.y - 6, i, 18, 14, 8 - i, &skip);
    else if(m_localPlayer->getPosition().z > 7)
        setFloorDescription(msg, pos.x - 8, pos.y - 6, pos.z - 2, 18, 14, 3, &skip);

    //pos.x++;
    //pos.y++;
    //m_localPlayer->setPosition(pos);
}

void ProtocolGame::parseFloorChangeDown(InputMessage& msg)
{
    Position pos = m_localPlayer->getPosition();
    //pos.z++;

    int skip = 0;
    if(pos.z == 8) {
        int j, i;
        for(i = pos.z, j = -1; i < pos.z + 3; ++i, --j)
            setFloorDescription(msg, pos.x - 8, pos.y - 6, i, 18, 14, j, &skip);
    }
    else if(pos.z > 8 && pos.z < 14)
        setFloorDescription(msg, pos.x - 8, pos.y - 6, pos.z + 2, 18, 14, -3, &skip);

    //pos.x--;
    //pos.y--;
    //m_localPlayer->setPosition(pos);
}

void ProtocolGame::parseOutfitWindow(InputMessage& msg)
{
    internalGetOutfit(msg);
    uint8 outfitCount = msg.getU8();

    for(int i = 0; i < outfitCount; i++) {
        msg.getU16(); // outfit id
        msg.getString(); // outfit name
        msg.getU8(); // addons
    }
}

void ProtocolGame::parseVipState(InputMessage& msg)
{
    msg.getU32(); // player id
    msg.getString(); // player name
    msg.getU8(); // online
}

void ProtocolGame::parseVipLogin(InputMessage& msg)
{
    msg.getU32(); // player id
}

void ProtocolGame::parseVipLogout(InputMessage& msg)
{
    msg.getU32(); // player id
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

    int stackpos = 0;
    while(1){
        uint16 inspectTileId = msg.getU16(true);
        if(inspectTileId >= 0xFF00)
            return;
        else {
            if(stackpos >= 10) {
                logDebug("[ProtocolGame::setTileDescription] Too many things!.");
                return;
            }

            ThingPtr thing = internalGetThing(msg);
            if(thing)
                thing->setPosition(position);
            g_map.addThing(thing);
        }
        stackpos++;
    }
}

Outfit ProtocolGame::internalGetOutfit(InputMessage& msg)
{
    Outfit outfit;

    outfit.type = msg.getU16(); // looktype
    if(outfit.type != 0) {
        outfit.head = msg.getU8();
        outfit.body = msg.getU8();
        outfit.legs = msg.getU8();
        outfit.feet = msg.getU8();
        outfit.addons = msg.getU8();
    }
    else {
        outfit.type = msg.getU16();
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
            /*uint32 removeId = */msg.getU32();
            uint32 id = msg.getU32();
            std::string name = msg.getString();

            LocalPlayerPtr localPlayer = g_game.getLocalPlayer();
            if(localPlayer->getId() == id)
                creature = localPlayer->asCreature();

            creature->setId(id);
            creature->setName(name);
        }

        uint8 healthPercent = msg.getU8();
        Direction direction = (Direction)msg.getU8();
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

        thing = creature;

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

    const ThingAttributes& itemAttributes = g_dat.getItemAttributes(id);
    if(itemAttributes.stackable || itemAttributes.group == THING_GROUP_FLUID || itemAttributes.group == THING_GROUP_SPLASH)
        item->setCount(msg.getU8());

    return item;
}

Position ProtocolGame::parsePosition(InputMessage& msg)
{
    uint16 x = msg.getU16();
    uint16 y = msg.getU16();
    uint8  z = msg.getU8();

    return Position(x, y, z);
}
