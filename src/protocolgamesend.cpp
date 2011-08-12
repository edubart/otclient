#include "protocolgame.h"

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

