#ifndef PLAYER_H
#define PLAYER_H

#include "creature.h"

class Player : public Creature
{
public:
    void setDrawSpeed(uint16 drawSpeed) { m_drawSpeed = drawSpeed; }
    uint16 getDrawSpeed() { return m_drawSpeed; }

    void setCanReportBugs(uint8 canReportBugs) { m_canReportBugs = (canReportBugs != 0); }
    bool getCanReportBugs() { return m_canReportBugs; }

private:

    uint16 m_drawSpeed;
    bool m_canReportBugs;

};

#endif
