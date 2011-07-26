#include "protocollogin.h"
#include <net/outputmessage.h>
#include <net/rsa.h>
#include <script/luainterface.h>
#include <boost/bind.hpp>

ProtocolLogin::ProtocolLogin()
{

}

ProtocolLogin::~ProtocolLogin()
{

}

void ProtocolLogin::login(const std::string& accountName, const std::string& accountPassword)
{
    if(accountName.empty() || accountPassword.empty()) {
        callField("onError", "You must enter an account name and password.");
        return;
    }

    m_accountName = accountName;
    m_accountPassword = accountPassword;

    static const char hosts[][32] = {
        "login01.tibia.com",
        "login02.tibia.com",
        "login03.tibia.com",
        "login04.tibia.com",
        "login05.tibia.com",
        "tibia01.cipsoft.com",
        "tibia02.cipsoft.com",
        "tibia03.cipsoft.com",
        "tibia04.cipsoft.com",
        "tibia05.cipsoft.com"
    };

    std::string host = hosts[rand() % 10];
    //std::string host = "tecserver.zapto.org";
    uint16 port = 7171;

    connect(host, port, std::bind(&ProtocolLogin::onConnect, asProtocolLogin()));
}

void ProtocolLogin::onConnect()
{
    sendPacket();
}

void ProtocolLogin::sendPacket()
{
    OutputMessage oMsg;

    oMsg.addU8(0x01); // Protocol id
    oMsg.addU16(0x02);  // OS
    oMsg.addU16(874); // Client version

    oMsg.addU32(0x4DBAA20B); // Data Signature
    oMsg.addU32(0x4DAD1A32); // Sprite Signature
    oMsg.addU32(0x4DA2D2B5); // Picture Signature

    oMsg.addU8(0); // First RSA byte must be 0x00 // 1

    // Generete xtea key.
    m_xteaKey[0] = 432324;
    m_xteaKey[1] = 24324;
    m_xteaKey[2] = 423432;
    m_xteaKey[3] = 234324;

    // Add xtea key
    oMsg.addU32(m_xteaKey[0]); // 5
    oMsg.addU32(m_xteaKey[1]); // 9
    oMsg.addU32(m_xteaKey[2]); // 13
    oMsg.addU32(m_xteaKey[3]); // 17

    oMsg.addString(m_accountName); // Account Name // 19
    oMsg.addString(m_accountPassword); // Password // 21

    // Packet data must have since byte 0, start, 128 bytes
    oMsg.addPaddingBytes(128 - (21 + m_accountName.length() + m_accountPassword.length()));

    // Encrypt msg with RSA
    if(!Rsa::encrypt((char*)oMsg.getBuffer() + 6 + oMsg.getMessageSize() - 128, 128, CIPSOFT_PUBLIC_RSA))
        return;

    send(&oMsg);

    m_xteaEncryptionEnabled = true;
}

void ProtocolLogin::onRecv(InputMessage *inputMessage)
{
    Protocol::onRecv(inputMessage);

    while(!inputMessage->end()) {
        uint8 opt = inputMessage->getU8();
        logDebug("opt:",(uint)opt);
        switch(opt) {
        case 0x0A:
            parseError(inputMessage);
            break;
        case 0x14:
            parseMOTD(inputMessage);
            break;
        case 0x1e:
            inputMessage->getU8();
            inputMessage->getU8();
            inputMessage->getU8();
            inputMessage->getU8();
            inputMessage->getU8();
            callField("onError", "Client needs update.");
            break;
        case 0x64:
            parseCharacterList(inputMessage);
            break;
        }
    }
}

void ProtocolLogin::parseError(InputMessage *inputMessage)
{
    std::string error = inputMessage->getString();
    callField("onError", error);
}

void ProtocolLogin::parseMOTD(InputMessage *inputMessage)
{
    std::string motd = inputMessage->getString();
    callField("onMotd", motd);
}

void ProtocolLogin::parseCharacterList(InputMessage *inputMessage)
{
    uint8 characters = inputMessage->getU8();
    for(int i = 0; i < characters; ++i) {
        std::string name = inputMessage->getString();
        std::string world = inputMessage->getString();
        uint32 ip = inputMessage->getU32();
        uint16 port = inputMessage->getU16();

        logDebug("character: ", name.c_str(), world.c_str(), ip, port);
    }
    uint16 premiumDays = inputMessage->getU16();
    logDebug("prem days: ", premiumDays);
}
