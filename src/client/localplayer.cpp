/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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
#include <framework/core/eventdispatcher.h>
#include <framework/graphics/graphics.h>

LocalPlayer::LocalPlayer()
{
    m_states = 0;
    m_vocation = 0;
    m_blessings = Otc::BlessingNone;
    m_walkLockExpiration = 0;

    m_skillsLevel.fill(-1);
    m_skillsBaseLevel.fill(-1);
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
    m_baseMagicLevel = -1;
    m_soul = -1;
    m_stamina = -1;
    m_baseSpeed = -1;
    m_regenerationTime = -1;
    m_offlineTrainingTime = -1;
    m_totalCapacity = -1;
}

void LocalPlayer::lockWalk(int millis)
{
    m_walkLockExpiration = std::max<ticks_t>(m_walkLockExpiration, g_clock.millis() + millis);
}

bool LocalPlayer::canWalk(Otc::Direction)
{
    // cannot walk while locked
    if(m_walkLockExpiration != 0 && g_clock.millis() < m_walkLockExpiration)
        return false;

    // paralyzed
    if(m_speed == 0)
        return false;

    // last walk is not done yet
    if((m_walkTimer.ticksElapsed() < getStepDuration()) && !isAutoWalking())
        return false;

    // prewalk has a timeout, because for some reason that I don't know yet the server sometimes doesn't answer the prewalk
    bool prewalkTimeouted = m_walking && m_preWalking && m_walkTimer.ticksElapsed() >= getStepDuration() + PREWALK_TIMEOUT;

    // avoid doing more walks than wanted when receiving a lot of walks from server
    if(!m_lastPrewalkDone && m_preWalking && !prewalkTimeouted)
        return false;

    // cannot walk while already walking
    if((m_walking && !isAutoWalking()) && (!prewalkTimeouted || m_secondPreWalk))
        return false;

    return true;
}

void LocalPlayer::walk(const Position& oldPos, const Position& newPos)
{
    // a prewalk was going on
    if(m_preWalking) {
        // switch to normal walking
        m_preWalking = false;
        m_secondPreWalk = false;
        m_lastPrewalkDone = true;
        // if is to the last prewalk destination, updates the walk preserving the animation
        if(newPos == m_lastPrewalkDestination) {
            updateWalk();
        // was to another direction, replace the walk
        } else {
            Creature::walk(oldPos, newPos);
        }
    }
    // no prewalk was going on, this must be an server side automated walk
    else {
        m_serverWalking = true;
        if(m_serverWalkEndEvent)
            m_serverWalkEndEvent->cancel();

        Creature::walk(oldPos, newPos);
    }
}

void LocalPlayer::preWalk(Otc::Direction direction)
{
    Position newPos = m_position.translatedToDirection(direction);

    // avoid reanimating prewalks
    if(m_preWalking) {
        m_secondPreWalk = true;
        return;
    }

    m_preWalking = true;

    if(m_serverWalkEndEvent)
        m_serverWalkEndEvent->cancel();

    // start walking to direction
    m_lastPrewalkDone = false;
    m_lastPrewalkDestination = newPos;
    Creature::walk(m_position, newPos);
}

void LocalPlayer::cancelWalk(Otc::Direction direction)
{
    // only cancel client side walks
    if(m_walking && m_preWalking)
        stopWalk();

    m_lastPrewalkDone = true;
    m_idleTimer.restart();
    lockWalk();

    if(m_autoWalkDestination.isValid()) {
        g_game.stop();
        auto self = asLocalPlayer();
        if(m_autoWalkContinueEvent)
            m_autoWalkContinueEvent->cancel();
        m_autoWalkContinueEvent = g_dispatcher.scheduleEvent([self]() {
            if(self->m_autoWalkDestination.isValid())
                self->autoWalk(self->m_autoWalkDestination);
        }, 500);
    }

    // turn to the cancel direction
    if(direction != Otc::InvalidDirection)
        setDirection(direction);

    callLuaField("onCancelWalk", direction);
}

bool LocalPlayer::autoWalk(const Position& destination)
{
    if(g_game.getClientVersion() <= 740 && m_position.isInRange(destination, 1, 1))
        return g_game.walk(m_position.getDirectionFromPosition(destination));

    bool tryKnownPath = false;
    if(destination != m_autoWalkDestination) {
        m_knownCompletePath = false;
        tryKnownPath = true;
    }

    std::tuple<std::vector<Otc::Direction>, Otc::PathFindResult> result;
    std::vector<Otc::Direction> limitedPath;

    if(destination == m_position)
        return true;

    // try to find a path that we know
    if(tryKnownPath || m_knownCompletePath) {
        result = g_map.findPath(m_position, destination, 50000, 0);
        if(std::get<1>(result) == Otc::PathFindResultOk) {
            limitedPath = std::get<0>(result);
            // limit to 127 steps
            if(limitedPath.size() > 127)
                limitedPath.resize(127);
            m_knownCompletePath = true;
        }
    }

    // no known path found, try to discover one
    if(limitedPath.empty()) {
        result = g_map.findPath(m_position, destination, 50000, Otc::PathFindAllowNotSeenTiles);
        if(std::get<1>(result) != Otc::PathFindResultOk) {
            callLuaField("onAutoWalkFail", std::get<1>(result));
            stopAutoWalk();
            return false;
        }

        Position currentPos = m_position;
        for(auto dir : std::get<0>(result)) {
            currentPos = currentPos.translatedToDirection(dir);
            if(!hasSight(currentPos))
                break;
            else
                limitedPath.push_back(dir);
        }
    }

    m_autoWalkDestination = destination;
    m_lastAutoWalkPosition = m_position.translatedToDirections(limitedPath).back();

    /*
    // debug calculated path using minimap
    for(auto pos : m_position.translatedToDirections(limitedPath)) {
        g_map.getOrCreateTile(pos)->overwriteMinimapColor(215);
        g_map.notificateTileUpdate(pos);
    }
    */

    g_game.autoWalk(limitedPath);
    return true;
}

void LocalPlayer::stopAutoWalk()
{
    m_autoWalkDestination = Position();
    m_lastAutoWalkPosition = Position();
    m_knownCompletePath = false;

    if(m_autoWalkContinueEvent)
        m_autoWalkContinueEvent->cancel();
}

void LocalPlayer::stopWalk()
{
    Creature::stopWalk(); // will call terminateWalk

    m_lastPrewalkDone = true;
    m_lastPrewalkDestination = Position();
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
    } else {
        Creature::updateWalkOffset(totalPixelsWalked);
    }
}

void LocalPlayer::updateWalk()
{
    int stepDuration = getStepDuration(true);
    int totalPixelsWalked = stepDuration ? std::min<int>((m_walkTimer.ticksElapsed() * Otc::TILE_PIXELS) / stepDuration, Otc::TILE_PIXELS) : 0;

    // update walk animation and offsets
    updateWalkAnimation(totalPixelsWalked, stepDuration);
    updateWalkOffset(totalPixelsWalked);
    updateWalkingTile();

    // terminate walk only when client and server side walk are completed
    if(m_walking && !m_preWalking && m_walkTimer.ticksElapsed() >= getStepDuration())
        terminateWalk();
}

void LocalPlayer::terminateWalk()
{
    Creature::terminateWalk();
    m_preWalking = false;
    m_secondPreWalk = false;
    m_idleTimer.restart();

    auto self = asLocalPlayer();

    if(m_serverWalking) {
        if(m_serverWalkEndEvent)
            m_serverWalkEndEvent->cancel();
        m_serverWalkEndEvent = g_dispatcher.scheduleEvent([self] {
            self->m_serverWalking = false;
        }, 100);
    }
}

void LocalPlayer::onAppear()
{
    Creature::onAppear();

    /* Does not seem to be needed anymore
    // on teleports lock the walk
    if(!m_oldPosition.isInRange(m_position,1,1))
        lockWalk();
    */
}

void LocalPlayer::onPositionChange(const Position& newPos, const Position& oldPos)
{
    Creature::onPositionChange(newPos, oldPos);

    if(newPos == m_autoWalkDestination)
        stopAutoWalk();
    else if(m_autoWalkDestination.isValid() && newPos == m_lastAutoWalkPosition)
        autoWalk(m_autoWalkDestination);
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
    if(skill >= Otc::LastSkill) {
        g_logger.traceError("invalid skill");
        return;
    }

    int oldLevel = m_skillsLevel[skill];
    int oldLevelPercent = m_skillsLevelPercent[skill];

    if(level != oldLevel || levelPercent != oldLevelPercent) {
        m_skillsLevel[skill] = level;
        m_skillsLevelPercent[skill] = levelPercent;

        callLuaField("onSkillChange", skill, level, levelPercent, oldLevel, oldLevelPercent);
    }
}

void LocalPlayer::setBaseSkill(Otc::Skill skill, int baseLevel)
{
    if(skill >= Otc::LastSkill) {
        g_logger.traceError("invalid skill");
        return;
    }

    int oldBaseLevel = m_skillsBaseLevel[skill];
    if(baseLevel != oldBaseLevel) {
        m_skillsBaseLevel[skill] = baseLevel;

        callLuaField("onBaseSkillChange", skill, baseLevel, oldBaseLevel);
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

void LocalPlayer::setTotalCapacity(double totalCapacity)
{
    if(m_totalCapacity != totalCapacity) {
        double oldTotalCapacity = m_totalCapacity;
        m_totalCapacity = totalCapacity;

        callLuaField("onTotalCapacityChange", totalCapacity, oldTotalCapacity);
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
        double oldMaxMana = m_maxMana;
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

void LocalPlayer::setBaseMagicLevel(double baseMagicLevel)
{
    if(m_baseMagicLevel != baseMagicLevel) {
        double oldBaseMagicLevel = m_baseMagicLevel;
        m_baseMagicLevel = baseMagicLevel;

        callLuaField("onBaseMagicLevelChange", baseMagicLevel, oldBaseMagicLevel);
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

void LocalPlayer::setInventoryItem(Otc::InventorySlot inventory, const ItemPtr& item)
{
    if(inventory >= Otc::LastInventorySlot) {
        g_logger.traceError("invalid slot");
        return;
    }

    if(m_inventoryItems[inventory] != item) {
        ItemPtr oldItem = m_inventoryItems[inventory];
        m_inventoryItems[inventory] = item;

        callLuaField("onInventoryChange", inventory, item, oldItem);
    }
}

void LocalPlayer::setVocation(int vocation)
{
    if(m_vocation != vocation) {
        int oldVocation = m_vocation;
        m_vocation = vocation;

        callLuaField("onVocationChange", vocation, oldVocation);
    }
}

void LocalPlayer::setPremium(bool premium)
{
    if(m_premium != premium) {
        m_premium = premium;

        callLuaField("onPremiumChange", premium);
    }
}

void LocalPlayer::setRegenerationTime(double regenerationTime)
{
    if(m_regenerationTime != regenerationTime) {
        double oldRegenerationTime = m_regenerationTime;
        m_regenerationTime = regenerationTime;

        callLuaField("onRegenerationChange", regenerationTime, oldRegenerationTime);
    }
}

void LocalPlayer::setOfflineTrainingTime(double offlineTrainingTime)
{
    if(m_offlineTrainingTime != offlineTrainingTime) {
        double oldOfflineTrainingTime = m_offlineTrainingTime;
        m_offlineTrainingTime = offlineTrainingTime;

        callLuaField("onOfflineTrainingChange", offlineTrainingTime, oldOfflineTrainingTime);
    }
}

void LocalPlayer::setSpells(const std::vector<int>& spells)
{
    if(m_spells != spells) {
        std::vector<int> oldSpells = m_spells;
        m_spells = spells;

        callLuaField("onSpellsChange", spells, oldSpells);
    }
}

void LocalPlayer::setBlessings(int blessings)
{
    if(blessings != m_blessings) {
        int oldBlessings = m_blessings;
        m_blessings = blessings;

        callLuaField("onBlessingsChange", blessings, oldBlessings);
    }
}

bool LocalPlayer::hasSight(const Position& pos)
{
    return m_position.isInRange(pos, g_map.getAwareRange().left - 1, g_map.getAwareRange().top - 1);
}
