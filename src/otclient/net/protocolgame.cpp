#include <otclient/net/protocolgame.h>
#include <otclient/core/game.h>
#include <framework/net/rsa.h>

ProtocolGame::ProtocolGame()
{
    m_checksumEnabled = false;
    g_game.setProtocol(ProtocolGamePtr(this));
}

ProtocolGame::~ProtocolGame()
{
    sendLogout();
    g_game.setProtocol(NULL);
}

void ProtocolGame::login(const std::string& accountName, const std::string& accountPassword, uint32 ip, uint16 port, const std::string& characterName)
{
    if(accountName.empty() || accountPassword.empty()) {
        callLuaField("onError", "You must enter an account name and password.");
        return;
    }

    m_accountName = accountName;
    m_accountPassword = accountPassword;
    m_characterName = characterName;

    char host[16];
    sprintf(host, "%d.%d.%d.%d", (uint8)ip, (uint8)(ip >> 8), (uint8)(ip >> 16), (uint8)(ip >> 24));

    connect(host, port);
}

void ProtocolGame::onConnect()
{
    recv();
}

void ProtocolGame::sendLoginPacket(uint32 timestamp, uint8 unknown)
{
    OutputMessage oMsg;

    oMsg.addU8(0x0A); // Protocol id
    oMsg.addU16(0x02);  // OS
    oMsg.addU16(862); // Client version

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

    oMsg.addU8(0); // is gm set?
    oMsg.addString(m_accountName); // Account Name // 20
    oMsg.addString(m_characterName); // Character Name // 22
    oMsg.addString(m_accountPassword); // Password // 24

    oMsg.addU32(timestamp); // 28
    oMsg.addU8(unknown); // 29

    // Packet data must have since byte 0, start, 128 bytes
    oMsg.addPaddingBytes(128 - (29 + m_accountName.length() + m_characterName.length() + m_accountPassword.length()));

    // Encrypt msg with RSA
    if(!Rsa::encrypt((char*)oMsg.getBuffer() + 6 + oMsg.getMessageSize() - 128, 128, OTSERV_PUBLIC_RSA))
        return;

    send(oMsg);

    m_xteaEncryptionEnabled = true;

    recv();
}

void ProtocolGame::onRecv(InputMessage& inputMessage)
{
    static bool firstRecv = true;
    if(firstRecv) {
        inputMessage.skipBytes(3);
        uint32 timestamp = inputMessage.getU32();
        uint8 unknown = inputMessage.getU8();

        m_checksumEnabled = true;
        sendLoginPacket(timestamp, unknown);

        firstRecv = false;
    }
    else {
        parseMessage(inputMessage);
    }
}
