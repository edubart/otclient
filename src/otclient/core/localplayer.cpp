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

#include "localplayer.h"
#include "map.h"
#include "game.h"
#include "tile.h"

LocalPlayer::LocalPlayer()
{
    m_preWalking = false;
    m_walkLocked = false;
    m_lastPrewalkDone = true;
    m_known = false;
    m_states = 0;

    m_skillsLevel.fill(-1);
    m_skillsLevelPercent.fill(-1);

    m_health = -1;
    m_maxHealth = -1;
    m_freeCapacity = -1;
    m_experience = -1;
    m_level = -1;
    m_levelPercent = -1;
    m_mana = -1;
    m_maxMana = -1;
    m_magicLevel = -1;
    m_magicLevelPercent = -1;
    m_soul = -1;
    m_stamina = -1;
}

void LocalPlayer::lockWalk()
{
    // prevents double locks
    if(m_walkLocked)
        return;

    m_walkLocked = true;
    m_walkLockTimer.restart();
}

bool LocalPlayer::canWalk(Otc::Direction direction)
{
    // prewalk has a timeout, because for some reason that I don't know yet the server sometimes doesn't answer the prewalk
    bool prewalkTimeouted = m_walking && m_preWalking && m_walkTimer.ticksElapsed() >= m_walkInterval + PREWALK_TIMEOUT;

    // cannot walk while already walking
    if(m_walking && !prewalkTimeouted)
        return false;

    // avoid doing more walks than wanted when receiving a lot of walks from server
    if(!m_lastPrewalkDone && m_preWalking && !prewalkTimeouted)
        return false;

    // cannot walk while locked
    if(m_walkLocked && m_walkLockTimer.ticksElapsed() <= WALK_LOCK_INTERVAL)
        return false;
    else
        m_walkLocked = false;

    return true;
}

void LocalPlayer::walk(const Position& oldPos, const Position& newPos)
{
    // a prewalk was going on
    if(m_preWalking) {
        // switch to normal walking
        m_preWalking = false;
        m_lastPrewalkDone = true;
        // if is to the last prewalk destination, updates the walk preserving the animation
        if(newPos == m_lastPrewalkDestionation) {
            updateWalk();
        // was to another direction, replace the walk
        } else
            Creature::walk(oldPos, newPos);
    } else
        Creature::walk(oldPos, newPos);
}

void LocalPlayer::preWalk(Otc::Direction direction)
{
    // start walking to direction
    Position newPos = m_position.translatedToDirection(direction);
    m_preWalking = true;
    m_lastPrewalkDone = false;
    m_lastPrewalkDestionation = newPos;
    Creature::walk(m_position, newPos);
}

void LocalPlayer::cancelWalk(Otc::Direction direction)
{
    // only cancel client side walks
    if(m_walking && m_preWalking)
        stopWalk();

    m_lastPrewalkDone = true;

    // turn to the cancel direction
    if(direction != Otc::InvalidDirection)
        setDirection(direction);
}

void LocalPlayer::stopWalk()
{
    Creature::stopWalk();
    m_lastPrewalkDone = true;
    m_lastPrewalkDestionation = Position();
}

void LocalPlayer::updateWalkOffset(int totalPixelsWalked)
{
    // pre walks offsets are calculated in the oposite direction
    if(m_preWalking) {
        m_walkOffset = Point(0,0);
        if(m_direction == Otc::North || m_direction == Otc::NorthEast || m_direction == Otc::NorthWest)
            m_walkOffset.y = -totalPixelsWalked;
        else if(m_direction == Otc::South || m_direction == Otc::SouthEast || m_direction == Otc::SouthWest)
            m_walkOffset.y = totalPixelsWalked;

        if(m_direction == Otc::East || m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
            m_walkOffset.x = totalPixelsWalked;
        else if(m_direction == Otc::West || m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
            m_walkOffset.x = -totalPixelsWalked;
    } else
        Creature::updateWalkOffset(totalPixelsWalked);
}

void LocalPlayer::updateWalk()
{
    float walkTicksPerPixel = m_walkAnimationInterval / 32.0f;
    int totalPixelsWalked = std::min(m_walkTimer.ticksElapsed() / walkTicksPerPixel, 32.0f);

    // update walk animation and offsets
    updateWalkAnimation(totalPixelsWalked);
    updateWalkOffset(totalPixelsWalked);
    updateWalkingTile();

    // terminate walk only when client and server side walk are complated
    if(m_walking && !m_preWalking && m_walkTimer.ticksElapsed() >= m_walkInterval)
        terminateWalk();
}

void LocalPlayer::terminateWalk()
{
    Creature::terminateWalk();
    m_preWalking = false;
}

void LocalPlayer::setStates(int states)
{
    if(m_states != states) {
        int oldStates = m_states;
        m_states = states;
        callLuaField("onStatesChange", states, oldStates);
    }
}

void LocalPlayer::setSkill(Otc::Skill skill, int level, int levelPercent)
{
    int oldLevel = m_skillsLevel[skill];
    int oldLevelPercent = m_skillsLevelPercent[skill];

    if(level != oldLevel || levelPercent != oldLevelPercent) {
        m_skillsLevel[skill] = level;
        m_skillsLevelPercent[skill] = levelPercent;

        callLuaField("onSkillChange", skill, level, levelPercent, oldLevel, oldLevelPercent);
    }
}

void LocalPlayer::setHealth(double health, double maxHealth)
{
    if(m_health != health || m_maxHealth != maxHealth) {
        double oldHealth = m_health;
        double oldMaxHealth = m_maxHealth;
        m_health = health;
        m_maxHealth = maxHealth;

        callLuaField("onHealthChange", health, maxHealth, oldHealth, oldMaxHealth);

        // cannot walk while dying
        if(health == 0) {
            if(isPreWalking())
                stopWalk();
            lockWalk();
        }
    }
}

void LocalPlayer::setFreeCapacity(double freeCapacity)
{
    if(m_freeCapacity != freeCapacity) {
        double oldFreeCapacity = m_freeCapacity;
        m_freeCapacity = freeCapacity;

        callLuaField("onFreeCapacityChange", freeCapacity, oldFreeCapacity);
    }
}

void LocalPlayer::setExperience(double experience)
{
    if(m_experience != experience) {
        double oldExperience = m_experience;
        m_experience = experience;

        callLuaField("onExperienceChange", experience, oldExperience);
    }
}

void LocalPlayer::setLevel(double level, double levelPercent)
{
    if(m_level != level || m_levelPercent != levelPercent) {
        double oldLevel = m_level;
        double oldLevelPercent = m_levelPercent;
        m_level = level;
        m_levelPercent = levelPercent;

        callLuaField("onLevelChange", level, levelPercent, oldLevel, oldLevelPercent);
    }
}

void LocalPlayer::setMana(double mana, double maxMana)
{
    if(m_mana != mana || m_maxMana != maxMana) {
        double oldMana = m_mana;
        double oldMaxMana;
        m_mana = mana;
        m_maxMana = maxMana;

        callLuaField("onManaChange", mana, maxMana, oldMana, oldMaxMana);
    }
}

void LocalPlayer::setMagicLevel(double magicLevel, double magicLevelPercent)
{
    if(m_magicLevel != magicLevel || m_magicLevelPercent != magicLevelPercent) {
        double oldMagicLevel = m_magicLevel;
        double oldMagicLevelPercent = m_magicLevelPercent;
        m_magicLevel = magicLevel;
        m_magicLevelPercent = magicLevelPercent;

        callLuaField("onMagicLevelChange", magicLevel, magicLevelPercent, oldMagicLevel, oldMagicLevelPercent);
    }
}

void LocalPlayer::setSoul(double soul)
{
    if(m_soul != soul) {
        double oldSoul = m_soul;
        m_soul = soul;

        callLuaField("onSoulChange", soul, oldSoul);
    }
}

void LocalPlayer::setStamina(double stamina)
{
    if(m_stamina != stamina) {
        double oldStamina = m_stamina;
        m_stamina = stamina;

        callLuaField("onStaminaChange", stamina, oldStamina);
    }
}

void LocalPlayer::setInventoryItem(Otc::Inventory inventory, const ItemPtr& item)
{
    if(m_inventoryItems[inventory] != item) {
        ItemPtr oldItem = m_inventoryItems[inventory];
        m_inventoryItems[inventory] = item;

        callLuaField("onInventoryChange", inventory, item, oldItem);
    }
}
