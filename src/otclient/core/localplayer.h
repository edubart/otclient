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

    void setSkill(Otc::Skill skill, Otc::SkillType skillType, int value) { m_skills[skill][skillType] = value; }
    int getSkill(Otc::Skill skill, Otc::SkillType skillType) { return m_skills[skill][skillType]; }

    void walk(Otc::Direction direction);

    LocalPlayerPtr asLocalPlayer() { return std::static_pointer_cast<LocalPlayer>(shared_from_this()); }

private:
    uint16 m_drawSpeed;
    bool m_canReportBugs;

    int m_skills[Otc::LastSkill][Otc::LastSkillType];
};

#endif
