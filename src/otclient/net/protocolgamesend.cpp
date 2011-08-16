#include "protocolgame.h"
#include <framework/net/rsa.h>

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
}

void ProtocolGame::sendLogout()
{
    OutputMessage oMsg;
    oMsg.addU8(0x14);
    send(oMsg);
}

void ProtocolGame::sendPing()
{
    OutputMessage oMsg;
    oMsg.addU8(0x1E);
    send(oMsg);
}

void ProtocolGame::sendWalkNorth()
{
    OutputMessage oMsg;
    oMsg.addU8(0x65);
    send(oMsg);
}

void ProtocolGame::sendWalkEast()
{
    OutputMessage oMsg;
    oMsg.addU8(0x66);
    send(oMsg);
}

void ProtocolGame::sendWalkSouth()
{
    OutputMessage oMsg;
    oMsg.addU8(0x67);
    send(oMsg);
}

void ProtocolGame::sendWalkWest()
{
    OutputMessage oMsg;
    oMsg.addU8(0x68);
    send(oMsg);
}

