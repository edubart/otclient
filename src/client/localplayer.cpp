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
#include "game.h"
#include "map.h"
#include "tile.h"
#include <framework/core/eventdispatcher.h>

LocalPlayer::LocalPlayer()
{
    m_skillsLevel.fill(-1);
    m_skillsBaseLevel.fill(-1);
    m_skillsLevelPercent.fill(-1);
}

void LocalPlayer::lockWalk(int millis)
{
    m_walkLockExpiration = std::max<ticks_t>(m_walkLockExpiration, g_clock.millis() + millis);
}

bool LocalPlayer::canWalk(bool ignoreLock)
{
    // paralyzed
    if (isParalyzed())
        return false;

    // cannot walk while locked
    if ((m_walkLockExpiration != 0 && g_clock.millis() < m_walkLockExpiration) && !ignoreLock)
        return false;

    if (isAutoWalking())
        return true;

    if (m_forceWalk) {
        m_forceWalk = false;
        return true;
    }

    const int stepDuration = std::max<int>(getStepDuration(), g_game.getPing());
    return m_walkTimer.ticksElapsed() >= stepDuration;
}

void LocalPlayer::walk(const Position& oldPos, const Position& newPos)
{
    m_autoWalkRetries = 0;

    if (m_preWalking) {
        m_preWalking = false;
        if (newPos == m_lastPrewalkDestination) {
            updateWalk();
            return;
        }
    }

    Creature::walk(oldPos, newPos);
}

void LocalPlayer::preWalk(Otc::Direction direction)
{
    // avoid reanimating prewalks
    if (m_preWalking) {
        return;
    }

    m_preWalking = true;

    // start walking to direction
    const Position newPos = m_position.translatedToDirection(direction);
    m_lastPrewalkDestination = newPos;
    Creature::walk(m_position, newPos);
}

bool LocalPlayer::retryAutoWalk()
{
    if (m_autoWalkDestination.isValid()) {
        g_game.stop();

        if (m_autoWalkRetries <= 3) {
            if (m_autoWalkContinueEvent)
                m_autoWalkContinueEvent->cancel();

            m_autoWalkContinueEvent = g_dispatcher.scheduleEvent([capture0 = asLocalPlayer(), this]
            {
                capture0->autoWalk(m_autoWalkDestination, true);
            }, 200);
            m_autoWalkRetries += 1;

            return true;
        }

        m_autoWalkDestination = {};
    }

    return false;
}

void LocalPlayer::cancelWalk(Otc::Direction direction)
{
    // only cancel client side walks
    if (m_walking && m_preWalking)
        stopWalk();

    g_map.notificateCameraMove(m_walkOffset);

    lockWalk();
    if (retryAutoWalk()) return;

    // turn to the cancel direction
    if (direction != Otc::InvalidDirection)
        setDirection(direction);

    callLuaField("onCancelWalk", direction);
}

bool LocalPlayer::autoWalk(const Position& destination, const bool retry)
{
    // reset state
    m_autoWalkDestination = Position();
    m_lastAutoWalkPosition = Position();

    if (!retry)
        m_autoWalkRetries = 0;

    if (destination == m_position)
        return true;

    if (m_position.isInRange(destination, 1, 1))
        return g_game.walk(m_position.getDirectionFromPosition(destination));

    std::tuple<std::vector<Otc::Direction>, Otc::PathFindResult> result;
    std::vector<Otc::Direction> limitedPath;

    m_autoWalkDestination = destination;
    auto self(asLocalPlayer());
    g_map.findPathAsync(m_position, destination, [self](const PathFindResult_ptr& result) {
        if (self->m_autoWalkDestination != result->destination)
            return;

        if (result->status != Otc::PathFindResultOk) {
            if (self->m_autoWalkRetries > 0 && self->m_autoWalkRetries <= 3) { // try again in 300, 700, 1200 ms if canceled by server
                if (self->m_autoWalkContinueEvent)
                    self->m_autoWalkContinueEvent->cancel();

                self->m_autoWalkContinueEvent = g_dispatcher.scheduleEvent(
                    [self, capture0 = result->destination]{ self->autoWalk(capture0, true); }, 200 + self->m_autoWalkRetries * 100);
                return;
            }
            self->m_autoWalkDestination = Position();
            self->callLuaField("onAutoWalkFail", result->status);
            return;
        }

        if (result->path.size() > 127)
            result->path.resize(127);

        if (result->path.empty()) {
            self->m_autoWalkDestination = Position();
            self->callLuaField("onAutoWalkFail", result->status);
            return;
        }

        const auto finalAutowalkPos = self->m_position.translatedToDirections(result->path).back();
        if (self->m_autoWalkDestination != finalAutowalkPos) {
            self->m_lastAutoWalkPosition = finalAutowalkPos;
        }

        g_game.autoWalk(result->path, result->start);
    });

    if (!retry)
        lockWalk();

    return true;
}

void LocalPlayer::stopAutoWalk()
{
    m_autoWalkDestination = Position();
    m_lastAutoWalkPosition = Position();
    m_knownCompletePath = false;

    if (m_autoWalkContinueEvent)
        m_autoWalkContinueEvent->cancel();
}

void LocalPlayer::stopWalk()
{
    Creature::stopWalk(); // will call terminateWalk

    m_lastPrewalkDestination = Position();
}

void LocalPlayer::updateWalkOffset(int totalPixelsWalked)
{
    if (!m_preWalking) {
        Creature::updateWalkOffset(totalPixelsWalked);
        return;
    }

    // pre walks offsets are calculated in the oposite direction
    m_walkOffset = Point();
    if (m_direction == Otc::North || m_direction == Otc::NorthEast || m_direction == Otc::NorthWest)
        m_walkOffset.y = -totalPixelsWalked;
    else if (m_direction == Otc::South || m_direction == Otc::SouthEast || m_direction == Otc::SouthWest)
        m_walkOffset.y = totalPixelsWalked;

    if (m_direction == Otc::East || m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
        m_walkOffset.x = totalPixelsWalked;
    else if (m_direction == Otc::West || m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
        m_walkOffset.x = -totalPixelsWalked;
}

void LocalPlayer::terminateWalk()
{
    Creature::terminateWalk();
    m_preWalking = false;
}

void LocalPlayer::onPositionChange(const Position& newPos, const Position& oldPos)
{
    Creature::onPositionChange(newPos, oldPos);

    if (newPos == m_autoWalkDestination)
        stopAutoWalk();
    else if (m_autoWalkDestination.isValid() && newPos == m_lastAutoWalkPosition)
        autoWalk(m_autoWalkDestination);

    if (newPos.z != oldPos.z) {
        m_forceWalk = true;
    }
}

void LocalPlayer::setStates(int states)
{
    if (m_states != states) {
        const int oldStates = m_states;
        m_states = states;

        callLuaField("onStatesChange", states, oldStates);
    }
}

void LocalPlayer::setSkill(Otc::Skill skill, int level, int levelPercent)
{
    if (skill >= Otc::LastSkill) {
        g_logger.traceError("invalid skill");
        return;
    }

    const int oldLevel = m_skillsLevel[skill];
    const int oldLevelPercent = m_skillsLevelPercent[skill];

    if (level != oldLevel || levelPercent != oldLevelPercent) {
        m_skillsLevel[skill] = level;
        m_skillsLevelPercent[skill] = levelPercent;

        callLuaField("onSkillChange", skill, level, levelPercent, oldLevel, oldLevelPercent);
    }
}

void LocalPlayer::setBaseSkill(Otc::Skill skill, int baseLevel)
{
    if (skill >= Otc::LastSkill) {
        g_logger.traceError("invalid skill");
        return;
    }

    const int oldBaseLevel = m_skillsBaseLevel[skill];
    if (baseLevel != oldBaseLevel) {
        m_skillsBaseLevel[skill] = baseLevel;

        callLuaField("onBaseSkillChange", skill, baseLevel, oldBaseLevel);
    }
}

void LocalPlayer::setHealth(double health, double maxHealth)
{
    if (m_health != health || m_maxHealth != maxHealth) {
        const double oldHealth = m_health;
        const double oldMaxHealth = m_maxHealth;
        m_health = health;
        m_maxHealth = maxHealth;

        callLuaField("onHealthChange", health, maxHealth, oldHealth, oldMaxHealth);

        // cannot walk while dying
        if (health == 0) {
            if (isPreWalking())
                stopWalk();
            lockWalk();
        }
    }
}

void LocalPlayer::setFreeCapacity(double freeCapacity)
{
    if (m_freeCapacity != freeCapacity) {
        const double oldFreeCapacity = m_freeCapacity;
        m_freeCapacity = freeCapacity;

        callLuaField("onFreeCapacityChange", freeCapacity, oldFreeCapacity);
    }
}

void LocalPlayer::setTotalCapacity(double totalCapacity)
{
    if (m_totalCapacity != totalCapacity) {
        const double oldTotalCapacity = m_totalCapacity;
        m_totalCapacity = totalCapacity;

        callLuaField("onTotalCapacityChange", totalCapacity, oldTotalCapacity);
    }
}

void LocalPlayer::setExperience(double experience)
{
    if (m_experience != experience) {
        const double oldExperience = m_experience;
        m_experience = experience;

        callLuaField("onExperienceChange", experience, oldExperience);
    }
}

void LocalPlayer::setLevel(double level, double levelPercent)
{
    if (m_level != level || m_levelPercent != levelPercent) {
        const double oldLevel = m_level;
        const double oldLevelPercent = m_levelPercent;
        m_level = level;
        m_levelPercent = levelPercent;

        callLuaField("onLevelChange", level, levelPercent, oldLevel, oldLevelPercent);
    }
}

void LocalPlayer::setMana(double mana, double maxMana)
{
    if (m_mana != mana || m_maxMana != maxMana) {
        const double oldMana = m_mana;
        const double oldMaxMana = m_maxMana;
        m_mana = mana;
        m_maxMana = maxMana;

        callLuaField("onManaChange", mana, maxMana, oldMana, oldMaxMana);
    }
}

void LocalPlayer::setMagicLevel(double magicLevel, double magicLevelPercent)
{
    if (m_magicLevel != magicLevel || m_magicLevelPercent != magicLevelPercent) {
        const double oldMagicLevel = m_magicLevel;
        const double oldMagicLevelPercent = m_magicLevelPercent;
        m_magicLevel = magicLevel;
        m_magicLevelPercent = magicLevelPercent;

        callLuaField("onMagicLevelChange", magicLevel, magicLevelPercent, oldMagicLevel, oldMagicLevelPercent);
    }
}

void LocalPlayer::setBaseMagicLevel(double baseMagicLevel)
{
    if (m_baseMagicLevel != baseMagicLevel) {
        const double oldBaseMagicLevel = m_baseMagicLevel;
        m_baseMagicLevel = baseMagicLevel;

        callLuaField("onBaseMagicLevelChange", baseMagicLevel, oldBaseMagicLevel);
    }
}

void LocalPlayer::setSoul(double soul)
{
    if (m_soul != soul) {
        const double oldSoul = m_soul;
        m_soul = soul;

        callLuaField("onSoulChange", soul, oldSoul);
    }
}

void LocalPlayer::setStamina(double stamina)
{
    if (m_stamina != stamina) {
        const double oldStamina = m_stamina;
        m_stamina = stamina;

        callLuaField("onStaminaChange", stamina, oldStamina);
    }
}

void LocalPlayer::setInventoryItem(Otc::InventorySlot inventory, const ItemPtr& item)
{
    if (inventory >= Otc::LastInventorySlot) {
        g_logger.traceError("invalid slot");
        return;
    }

    if (m_inventoryItems[inventory] != item) {
        const ItemPtr oldItem = m_inventoryItems[inventory];
        m_inventoryItems[inventory] = item;

        callLuaField("onInventoryChange", inventory, item, oldItem);
    }
}

void LocalPlayer::setVocation(int vocation)
{
    if (m_vocation != vocation) {
        const int oldVocation = m_vocation;
        m_vocation = vocation;

        callLuaField("onVocationChange", vocation, oldVocation);
    }
}

void LocalPlayer::setPremium(bool premium)
{
    if (m_premium != premium) {
        m_premium = premium;

        callLuaField("onPremiumChange", premium);
    }
}

void LocalPlayer::setRegenerationTime(double regenerationTime)
{
    if (m_regenerationTime != regenerationTime) {
        const double oldRegenerationTime = m_regenerationTime;
        m_regenerationTime = regenerationTime;

        callLuaField("onRegenerationChange", regenerationTime, oldRegenerationTime);
    }
}

void LocalPlayer::setOfflineTrainingTime(double offlineTrainingTime)
{
    if (m_offlineTrainingTime != offlineTrainingTime) {
        const double oldOfflineTrainingTime = m_offlineTrainingTime;
        m_offlineTrainingTime = offlineTrainingTime;

        callLuaField("onOfflineTrainingChange", offlineTrainingTime, oldOfflineTrainingTime);
    }
}

void LocalPlayer::setSpells(const std::vector<int>& spells)
{
    if (m_spells != spells) {
        const std::vector<int> oldSpells = m_spells;
        m_spells = spells;

        callLuaField("onSpellsChange", spells, oldSpells);
    }
}

void LocalPlayer::setBlessings(int blessings)
{
    if (blessings != m_blessings) {
        const int oldBlessings = m_blessings;
        m_blessings = blessings;

        callLuaField("onBlessingsChange", blessings, oldBlessings);
    }
}

void LocalPlayer::setResourceBalance(Otc::ResourceTypes_t type, uint64_t value)
{
    const uint64_t oldBalance = getResourceBalance(type);
    if (value != oldBalance) {
        m_resourcesBalance[type] = value;
        callLuaField("onResourcesBalanceChange", value, oldBalance);
    }
}

bool LocalPlayer::hasSight(const Position& pos)
{
    return m_position.isInRange(pos, g_map.getAwareRange().left - 1, g_map.getAwareRange().top - 1);
}
