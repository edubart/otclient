#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H

#include "player.h"

class LocalPlayer : public Player
{
public:
    LocalPlayer() { }

    void setDrawSpeed(uint16 drawSpeed) { m_drawSpeed = drawSpeed; }
    uint16 getDrawSpeed() { return m_drawSpeed; }

    void setCanReportBugs(uint8 canReportBugs) { m_canReportBugs = (canReportBugs != 0); }
    bool getCanReportBugs() { return m_canReportBugs; }

    LocalPlayerPtr asLocalPlayer() { return std::static_pointer_cast<LocalPlayer>(shared_from_this()); }

private:
    uint16 m_drawSpeed;
    bool m_canReportBugs;
};

#endif
