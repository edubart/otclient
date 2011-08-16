#include "protocolgame.h"
#include <framework/net/rsa.h>

void ProtocolGame::sendLoginPacket(uint32 timestamp, uint8 unknown)
{
    OutputMessage oMsg;

    oMsg.addU8(0x0A); // protocol id
    oMsg.addU16(0x02); // os
    oMsg.addU16(862); // client version

    oMsg.addU8(0); // first RSA byte must be 0

    // xtea key
    generateXteaKey();
    oMsg.addU32(m_xteaKey[0]);
    oMsg.addU32(m_xteaKey[1]);
    oMsg.addU32(m_xteaKey[2]);
    oMsg.addU32(m_xteaKey[3]);

    oMsg.addU8(0); // is gm set?
    oMsg.addString(m_accountName);
    oMsg.addString(m_characterName);
    oMsg.addString(m_accountPassword);

    oMsg.addU32(timestamp);
    oMsg.addU8(unknown);

    // fill the rest with zeros
    oMsg.addPaddingBytes(128 - (29 + m_accountName.length() + m_characterName.length() + m_accountPassword.length()));

    // encrypt with RSA
    if(!Rsa::encrypt((char*)oMsg.getBuffer() + 6 + oMsg.getMessageSize() - 128, 128, OTSERV_PUBLIC_RSA))
        return;

    send(oMsg);

    enableXteaEncryption();
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

