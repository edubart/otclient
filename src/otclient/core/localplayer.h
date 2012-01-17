/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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
    void setCanReportBugs(uint8 canReportBugs) { m_canReportBugs = (canReportBugs != 0); }
    void setSkill(Otc::Skill skill, Otc::SkillType skillType, int value) { m_skills[skill][skillType] = value; }
    void setStatistic(Otc::Statistic statistic, double value) { m_statistics[statistic] = value; }
    void setAttackingCreature(const CreaturePtr& creature);
    void setFollowingCreature(const CreaturePtr& creature);
    void setIcons(Otc::PlayerIcons icons) { m_icons = icons; }
    void setKnown(bool known) { m_known = known; }

    bool getCanReportBugs() { return m_canReportBugs; }
    int getSkill(Otc::Skill skill, Otc::SkillType skillType) { return m_skills[skill][skillType]; }
    double getStatistic(Otc::Statistic statistic) { return m_statistics[statistic]; }
    CreaturePtr getAttackingCreature() { return m_attackingCreature; }
    CreaturePtr getFollowingCreature() { return m_followingCreature; }
    Otc::PlayerIcons getIcons() { return m_icons; }

    bool isKnown() { return m_known; }
    bool isAttacking() { return m_attackingCreature != nullptr; }
    bool isFollowing() { return m_followingCreature != nullptr; }

    void preWalk(Otc::Direction direction);
    bool canWalk(Otc::Direction direction);

    LocalPlayerPtr asLocalPlayer() { return std::static_pointer_cast<LocalPlayer>(shared_from_this()); }

    double getHealth() { return getStatistic(Otc::Health); }
    double getMaxHealth() { return getStatistic(Otc::MaxHealth); }
    double getFreeCapacity() { return getStatistic(Otc::FreeCapacity); }
    double getExperience() { return getStatistic(Otc::Experience); }
    double getLevel() { return getStatistic(Otc::Level); }
    double getLevelPercent() { return getStatistic(Otc::LevelPercent); }
    double getMana() { return getStatistic(Otc::Mana); }
    double getMaxMana() { return getStatistic(Otc::MaxMana); }
    double getMagicLevel() { return getStatistic(Otc::MagicLevel); }
    double getMagicLevelPercent() { return getStatistic(Otc::MagicLevelPercent); }
    double getSoul() { return getStatistic(Otc::Soul); }
    double getStamina() { return getStatistic(Otc::Stamina); }

private:
    bool m_canReportBugs;
    bool m_known;
    CreaturePtr m_attackingCreature, m_followingCreature;
    Otc::PlayerIcons m_icons;
    int m_skills[Otc::LastSkill][Otc::LastSkillType];
    double m_statistics[Otc::LastStatistic];
};

#endif
