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

    // complete the 128 bytes for rsa encryption with zeros
    oMsg.addPaddingBytes(128 - (29 + m_accountName.length() + m_characterName.length() + m_accountPassword.length()));

    // encrypt with RSA
    if(!Rsa::encrypt((char*)oMsg.getBuffer() + 6 + oMsg.getMessageSize() - 128, 128, Otc::OtservPublicRSA))
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

void ProtocolGame::sendWalkNorthEast()
{
    OutputMessage oMsg;
    oMsg.addU8(0x6A);
    send(oMsg);
}

void ProtocolGame::sendWalkSouthEast()
{
    OutputMessage oMsg;
    oMsg.addU8(0x6B);
    send(oMsg);
}

void ProtocolGame::sendWalkSouthWest()
{
    OutputMessage oMsg;
    oMsg.addU8(0x6C);
    send(oMsg);
}

void ProtocolGame::sendWalkNorthWest()
{
    OutputMessage oMsg;
    oMsg.addU8(0x6D);
    send(oMsg);
}

void ProtocolGame::sendTurnNorth()
{
    OutputMessage oMsg;
    oMsg.addU8(0x6F);
    send(oMsg);
}

void ProtocolGame::sendTurnEast()
{
    OutputMessage oMsg;
    oMsg.addU8(0x70);
    send(oMsg);
}

void ProtocolGame::sendTurnSouth()
{
    OutputMessage oMsg;
    oMsg.addU8(0x71);
    send(oMsg);
}

void ProtocolGame::sendTurnWest()
{
    OutputMessage oMsg;
    oMsg.addU8(0x72);
    send(oMsg);
}
