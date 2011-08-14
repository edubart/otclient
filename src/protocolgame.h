#ifndef PROTOCOLGAME_H
#define PROTOCOLGAME_H

#include <net/protocol.h>
#include "player.h"
#include "thing.h"

class ProtocolGame;
typedef std::shared_ptr<ProtocolGame> ProtocolGamePtr;

class ProtocolGame : public Protocol
{

public:
    ProtocolGame();
    ~ProtocolGame();

public:
    void login(const std::string& accountName, const std::string& accountPassword, uint32 ip, uint16 port, const std::string& characterName);

    void onConnect();
    void onRecv(InputMessage& inputMessage);

    // Send Messages
    void sendPing();
    void sendWalkNorth();
    void sendWalkEast();
    void sendWalkSouth();
    void sendWalkWest();

private:
    void sendLoginPacket(uint32 timestamp, uint8 unknown);

    // Parse Messages
    void parseMessage(InputMessage& msg);

    void parsePlayerLogin(InputMessage& msg);
    void parseGMActions(InputMessage& msg);
    void parseErrorMessage(InputMessage& msg);
    void parseFYIMessage(InputMessage& msg);
    void parseWaitList(InputMessage& msg);
    void parsePing(InputMessage&);
    void parseDeath(InputMessage&);
    void parseCanReportBugs(InputMessage& msg);
    void parseMapDescription(InputMessage& msg);
    void parseMoveNorth(InputMessage& msg);
    void parseMoveEast(InputMessage& msg);
    void parseMoveSouth(InputMessage& msg);
    void parseMoveWest(InputMessage& msg);
    void parseUpdateTile(InputMessage& msg);
    void parseTileAddThing(InputMessage& msg);
    void parseTileTransformThing(InputMessage& msg);
    void parseTileRemoveThing(InputMessage& msg);
    void parseCreatureMove(InputMessage& msg);
    void parseOpenContainer(InputMessage& msg);
    void parseCloseContainer(InputMessage& msg);
    void parseContainerAddItem(InputMessage& msg);
    void parseContainerUpdateItem(InputMessage& msg);
    void parseContainerRemoveItem(InputMessage& msg);
    void parseAddInventoryItem(InputMessage& msg);
    void parseRemoveInventoryItem(InputMessage& msg);
    void parseOpenShopWindow(InputMessage& msg);
    void parsePlayerCash(InputMessage& msg);
    void parseCloseShopWindow(InputMessage&);
    void parseWorldLight(InputMessage& msg);
    void parseMagicEffect(InputMessage& msg);
    void parseAnimatedText(InputMessage& msg);
    void parseDistanceShot(InputMessage& msg);
    void parseCreatureSquare(InputMessage& msg);
    void parseCreatureHealth(InputMessage& msg);
    void parseCreatureLight(InputMessage& msg);
    void parseCreatureOutfit(InputMessage& msg);
    void parseCreatureSpeed(InputMessage& msg);
    void parseCreatureSkulls(InputMessage& msg);
    void parseCreatureShields(InputMessage& msg);
    void parseItemTextWindow(InputMessage& msg);
    void parseHouseTextWindow(InputMessage& msg);
    void parsePlayerStats(InputMessage& msg);
    void parsePlayerSkills(InputMessage& msg);
    void parsePlayerIcons(InputMessage& msg);
    void parsePlayerCancelAttack(InputMessage& msg);
    void parseCreatureSpeak(InputMessage& msg);
    void parseChannelList(InputMessage& msg);
    void parseOpenChannel(InputMessage& msg);
    void parseOpenPrivatePlayerChat(InputMessage& msg);
    void parseOpenRuleViolation(InputMessage& msg);
    void parseRuleViolationAF(InputMessage& msg);
    void parseRuleViolationB0(InputMessage& msg);
    void parseRuleViolationB1(InputMessage& msg);
    void parseCreatePrivateChannel(InputMessage& msg);
    void parseClosePrivateChannel(InputMessage& msg);
    void parseSafeTradeRequest(InputMessage& msg);
    void parseSafeTradeClose(InputMessage&);
    void parseTextMessage(InputMessage& msg);
    void parseCancelWalk(InputMessage& msg);
    void parseFloorChangeUp(InputMessage& msg);
    void parseFloorChangeDown(InputMessage& msg);
    void parseOutfitWindow(InputMessage& msg);
    void parseVipState(InputMessage& msg);
    void parseVipLogin(InputMessage& msg);
    void parseVipLogout(InputMessage& msg);
    void parseShowTutorial(InputMessage& msg);
    void parseAddMarker(InputMessage& msg);
    void parseQuestList(InputMessage& msg);
    void parseQuestPartList(InputMessage& msg);

    void setMapDescription(InputMessage& msg, int32 x, int32 y, int32 z, int32 width, int32 height);
    void setFloorDescription(InputMessage& msg, int32 x, int32 y, int32 z, int32 width, int32 height, int32 offset, int32* skipTiles);
    void setTileDescription(InputMessage& msg, Position position);
    Thing *internalGetThing(InputMessage& msg);
    void internalCreatureOutfit(InputMessage& msg);
    Item *internalGetItem(InputMessage& msg, uint16 id);

    Position parsePosition(InputMessage& msg);

    std::string m_accountName, m_accountPassword, m_characterName;
};

// TODO: place it somewhere else
enum SpeakClasses {
    SPEAK_SAY		= 0x01,	//normal talk
    SPEAK_WHISPER	= 0x02,	//whispering - #w text
    SPEAK_YELL		= 0x03,	//yelling - #y text
    SPEAK_PRIVATE_PN	= 0x04, //Player-to-NPC speaking(NPCs channel)
    SPEAK_PRIVATE_NP	= 0x05, //NPC-to-Player speaking
    SPEAK_PRIVATE	= 0x06, //Players speaking privately to players
    SPEAK_CHANNEL_Y     = 0x07,	//Yellow message in chat
    SPEAK_CHANNEL_W     = 0x08, //White message in chat
    SPEAK_RVR_CHANNEL	= 0x09, //Reporting rule violation - Ctrl+R
    SPEAK_RVR_ANSWER	= 0x0A, //Answering report
    SPEAK_RVR_CONTINUE	= 0x0B, //Answering the answer of the report
    SPEAK_BROADCAST     = 0x0C,	//Broadcast a message - #b
    SPEAK_CHANNEL_R1    = 0x0D,	//Talk red on chat - #c
    SPEAK_PRIVATE_RED   = 0x0E,	//Red private - @name@ text
    SPEAK_CHANNEL_O     = 0x0F,	//Talk orange on text
    //SPEAK_            = 0x10, //?
    SPEAK_CHANNEL_R2    = 0x11,	//Talk red anonymously on chat - #d
    //SPEAK_            = 0x12, //?
    SPEAK_MONSTER_SAY   = 0x13,	//Talk orange
    SPEAK_MONSTER_YELL  = 0x14	//Yell orange
};

#endif // PROTOCOLGAME_H
