#include "protocolgame.h"
#include "player.h"
#include "tibiadat.h"
#include "game.h"
#include "map.h"
#include <iomanip>

void ProtocolGame::parseMessage(InputMessage& msg)
{
    while(!msg.end()) {
        uint8 opt = msg.getU8();

        dump << "Protocol opt: " << std::hex << (int)opt;

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
                logDebug("UNKNOWN PACKET BYTE.", opt);
                //skipPacket = true;
                break;
        }
    }
}

void ProtocolGame::parsePlayerLogin(InputMessage& msg)
{
    msg.getU32(); // player id
    msg.getU16(); // drawing speed.
    msg.getU8(); // can report bugs
}

void ProtocolGame::parseGMActions(InputMessage& msg)
{
    for(uint8 i = 0; i < 28; ++i)
        msg.getU8();
}

void ProtocolGame::parseErrorMessage(InputMessage& msg)
{
    msg.getString(); // message
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
    Player *player = g_game.getPlayer();
    player->setPosition(parsePosition(msg));
    setMapDescription(msg, player->getPosition().getX() - 8, player->getPosition().getY() - 6, player->getPosition().getZ(), 18, 14);
}

void ProtocolGame::parseMoveNorth(InputMessage& msg)
{
    Player *player = g_game.getPlayer();
    player->getPosition().setY(player->getPosition().getY() - 1);
    setMapDescription(msg, player->getPosition().getX() - 8, player->getPosition().getY() - 6, player->getPosition().getZ(), 18, 1);
}

void ProtocolGame::parseMoveEast(InputMessage& msg)
{
    Player *player = g_game.getPlayer();
    player->getPosition().setX(player->getPosition().getX() + 1);
    setMapDescription(msg, player->getPosition().getX() + 9, player->getPosition().getY() - 6, player->getPosition().getZ(), 1, 14);
}

void ProtocolGame::parseMoveSouth(InputMessage& msg)
{
    Player *player = g_game.getPlayer();
    player->getPosition().setY(player->getPosition().getY() + 1);
    setMapDescription(msg, player->getPosition().getX() - 8, player->getPosition().getY() + 7, player->getPosition().getZ(), 18, 1);
}

void ProtocolGame::parseMoveWest(InputMessage& msg)
{
    Player *player = g_game.getPlayer();
    player->getPosition().setX(player->getPosition().getX() - 1);
    setMapDescription(msg, player->getPosition().getX() - 8, player->getPosition().getY() - 6, player->getPosition().getZ(), 1, 14);
}

void ProtocolGame::parseUpdateTile(InputMessage& msg)
{
    Position tilePos = parsePosition(msg);
    uint16 thingId = msg.getU16(true);
    if(thingId == 0xFF01) {
        msg.getU16();
    }
    else  {
        setTileDescription(msg, tilePos);
        msg.getU16();
    }
}

void ProtocolGame::parseTileAddThing(InputMessage& msg)
{
    parsePosition(msg); // tilePos
    msg.getU8(); // stackPos
    internalGetThing(msg);
}

void ProtocolGame::parseTileTransformThing(InputMessage& msg)
{
    parsePosition(msg); // tilePos
    msg.getU8(); // stackPos

    uint16 thingId = msg.getU16();
    if(thingId == 0x0061 || thingId == 0x0062 || thingId == 0x0063) {
        msg.getU32(); // creatureId
        msg.getU8(); // direction
    }
    else {
        internalGetItem(msg, thingId);
    }
}

void ProtocolGame::parseTileRemoveThing(InputMessage& msg)
{
    parsePosition(msg); // tilePos
    msg.getU8(); // stackPos
}

void ProtocolGame::parseCreatureMove(InputMessage& msg)
{
    parsePosition(msg); // oldPos
    msg.getU8(); // oldStackPos
    parsePosition(msg); // newPos
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
    msg.getU8(); // level
    msg.getU8(); // color
}

void ProtocolGame::parseMagicEffect(InputMessage& msg)
{
    parsePosition(msg); // effect pos
    msg.getU8(); // effect
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
    msg.getU32(); // creatureId
    msg.getU8(); // percent
}

void ProtocolGame::parseCreatureLight(InputMessage& msg)
{
    msg.getU32(); // creature id
    msg.getU8(); // level
    msg.getU8(); // color
}

void ProtocolGame::parseCreatureOutfit(InputMessage& msg)
{
    msg.getU32(); // creature id
    internalCreatureOutfit(msg);
}

void ProtocolGame::parseCreatureSpeed(InputMessage& msg)
{
    msg.getU32(); // creature id
    msg.getU16(); // speed
}

void ProtocolGame::parseCreatureSkulls(InputMessage& msg)
{
    msg.getU32(); // creature id
    msg.getU8(); // skull
}

void ProtocolGame::parseCreatureShields(InputMessage& msg)
{
    msg.getU32(); // creature id
    msg.getU8(); // shield
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
        case SPEAK_CHANNEL_R2:
        case SPEAK_CHANNEL_O:
        case SPEAK_CHANNEL_Y:
        case SPEAK_CHANNEL_W:
            msg.getU16(); // channelId
            break;
        case SPEAK_RVR_CHANNEL:
            msg.getU32(); // time
            break;
        default:
            //qDebug() << "Unknown speak type. opt: 0xAA, type: " << type;
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
    logDebug(message);
}

void ProtocolGame::parseCancelWalk(InputMessage& msg)
{
    msg.getU8(); // direction
}

void ProtocolGame::parseFloorChangeUp(InputMessage& msg)
{
    Player *player = g_game.getPlayer();
    player->getPosition().setZ(player->getPosition().getZ() - 1);

    int32 skip = 0;
    if(player->getPosition().getZ() == 7)
        for(int32 i = 5; i >= 0; i--)
            setFloorDescription(msg, player->getPosition().getX() - 8, player->getPosition().getY() - 6, i, 18, 14, 8 - i, &skip);
    else if(player->getPosition().getZ() > 7)
        setFloorDescription(msg, player->getPosition().getX() - 8, player->getPosition().getY() - 6, player->getPosition().getZ() - 2, 18, 14, 3, &skip);

    player->getPosition().setX(player->getPosition().getX() + 1);
    player->getPosition().setY(player->getPosition().getY() + 1);
}

void ProtocolGame::parseFloorChangeDown(InputMessage& msg)
{
    Player *player = g_game.getPlayer();
    player->getPosition().setZ(player->getPosition().getZ() + 1);

    int32 skip = 0;
    if(player->getPosition().getZ() == 8) {
        int32 j, i;
        for(i = player->getPosition().getZ(), j = -1; i < (int32)player->getPosition().getZ() + 3; ++i, --j)
            setFloorDescription(msg, player->getPosition().getX() - 8, player->getPosition().getY() - 6, i, 18, 14, j, &skip);
    }
    else if(player->getPosition().getZ() > 8 && player->getPosition().getZ() < 14)
        setFloorDescription(msg, player->getPosition().getX() - 8, player->getPosition().getY() - 6, player->getPosition().getZ() + 2, 18, 14, -3, &skip);

    player->getPosition().setX(player->getPosition().getX() - 1);
    player->getPosition().setY(player->getPosition().getY() - 1);
}

void ProtocolGame::parseOutfitWindow(InputMessage& msg)
{
    internalCreatureOutfit(msg);
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
    int n = 0;
    while(1){
        n++;
        uint16 inspectTileId = msg.getU16(true);
        if(inspectTileId >= 0xFF00)
            return;
        else {
            if(n > 10) {
                logDebug("[ProtocolGame::setTileDescription] Too many things!.");
                return;
            }

            g_game.getMap()->addThing(internalGetThing(msg), position);
        }
    }
}

Thing *ProtocolGame::internalGetThing(InputMessage& msg)
{
    Thing *thing = NULL;

    uint16 thingId = msg.getU16();
    if(thingId == 0x0061 || thingId == 0x0062) { // add new creature
        if(thingId == 0x0062) { //creature is known
            msg.getU32(); // creature id
        }
        else if(thingId == 0x0061) { //creature is not known
            msg.getU32(); // remove id
            msg.getU32(); // creature id
            msg.getString(); // creature name
        }

        msg.getU8(); // hp
        msg.getU8(); // direction
        internalCreatureOutfit(msg);
        msg.getU8(); // level
        msg.getU8(); // color
        msg.getU16(); // speed
        msg.getU8(); // skull
        msg.getU8(); // shield

        if(thingId == 0x0061) // emblem is sent only in packet type 0x61
            msg.getU8();
        msg.getU8(); // impassable

    }
    else if(thingId == 0x0063) { // creature turn
        msg.getU32(); // creature id
        msg.getU8(); // direction
    }
    else // item
        thing = internalGetItem(msg, thingId);

    return thing;
}

void ProtocolGame::internalCreatureOutfit(InputMessage& msg)
{
    uint16 lookType = msg.getU16(); // looktype
    if(lookType != 0) {
        msg.getU8(); // lookhead
        msg.getU8(); // lookbody
        msg.getU8(); // looklegs
        msg.getU8(); // lookfeet
        msg.getU8(); // lookaddons
    }
    else {
        msg.getU16(); // looktype
    }
}

Item *ProtocolGame::internalGetItem(InputMessage& msg, uint16 id)
{
    Item *item = new Item();

    if(id == 0xFFFF)
        id = msg.getU16();
    item->setId(id);

    ItemAttributes *itemAttributes = g_tibiaDat.getItemAttributes(id);
    if(itemAttributes->stackable || itemAttributes->group == ITEM_GROUP_FLUID || itemAttributes->group == ITEM_GROUP_SPLASH)
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
