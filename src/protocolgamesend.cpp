#include "protocolgame.h"

void ProtocolGame::sendPing()
{
    OutputMessage oMsg;
    oMsg.reset();
    oMsg.addU8(0x1E);
    send(oMsg);
}
