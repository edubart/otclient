#ifndef OTCLIENT_NET_DECLARATIONS_H
#define OTCLIENT_NET_DECLARATIONS_H

#include <otclient/global.h>
#include <framework/net/declarations.h>

class ProtocolLogin;
class ProtocolGame;

typedef std::shared_ptr<ProtocolGame> ProtocolGamePtr;
typedef std::shared_ptr<ProtocolLogin> ProtocolLoginPtr;



#define CIPSOFT_PUBLIC_RSA "1321277432058722840622950990822933849527763264961655079678763618" \
                           "4334395343554449668205332383339435179772895415509701210392836078" \
                           "6959821132214473291575712138800495033169914814069637740318278150" \
                           "2907336840325241747827401343576296990629870233111328210165697754" \
                           "88792221429527047321331896351555606801473202394175817"


#define OTSERV_PUBLIC_RSA  "1091201329673994292788609605089955415282375029027981291234687579" \
                           "3726629149257644633073969600111060390723088861007265581882535850" \
                           "3429057592827629436413108566029093628212635953836686562675849720" \
                           "6207862794310902180176810615217550567108238764764442605581471797" \
                           "07119674283982419152118103759076030616683978566631413"

// TODO: place it somewhere else
enum SpeakClasses {
    SPEAK_SAY               = 0x01, //normal talk
    SPEAK_WHISPER           = 0x02, //whispering - #w text
    SPEAK_YELL              = 0x03, //yelling - #y text
    SPEAK_PRIVATE_PN        = 0x04, //Player-to-NPC speaking(NPCs channel)
    SPEAK_PRIVATE_NP        = 0x05, //NPC-to-Player speaking
    SPEAK_PRIVATE           = 0x06, //Players speaking privately to players
    SPEAK_CHANNEL_Y         = 0x07, //Yellow message in chat
    SPEAK_CHANNEL_W         = 0x08, //White message in chat
    SPEAK_BROADCAST         = 0x09, //Broadcast a message - #b
    SPEAK_CHANNEL_R1        = 0x0A, //Talk red on chat - #c
    SPEAK_PRIVATE_RED       = 0x0B, //Red private - @name@ text
    SPEAK_CHANNEL_O         = 0x0C, //Talk orange on text
    SPEAK_MONSTER_SAY       = 0x0D, //Talk orange
    SPEAK_MONSTER_YELL      = 0x0E  //Yell orange
};

#endif
