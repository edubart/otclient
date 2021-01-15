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

#include "tile.h"
#include <framework/graphics/fontmanager.h>
#include "effect.h"
#include "game.h"
#include "item.h"
#include "lightview.h"
#include "localplayer.h"
#include "protocolgame.h"
#include "map.h"
#include "thingtypemanager.h"
#include <framework/core/eventdispatcher.h>

Tile::Tile(const Position& position) :
    m_position(position),
    m_drawElevation(0),
    m_minimapColor(0),
    m_flags(0),
    m_houseId(0),
    m_localPlayer(nullptr)
{
    m_positionsAround = position.getPositionsAround();
}

void Tile::onAddVisibleTileList(const MapViewPtr& mapView)
{
    m_borderShadowColor = Color::white;

    if(isWalkable(true) && m_position.z == g_map.getCentralPosition().z - 1) {
        for(const auto& position : m_positionsAround) {
            const TilePtr& tile = g_map.getTile(position);
            if(!tile || !tile->isFullyOpaque() && tile->isWalkable(true)) {
                m_borderShadowColor = Color(215, 1, 0.65f);
                break;
            }
        }
    }

    if(hasCreature()) {
        auto& visibleCreatures = mapView->getVisibleCreatures();
        for(const auto& creature : m_creatures) {
            if(creature->isLocalPlayer()) continue;

            const auto it = std::find(visibleCreatures.begin(), visibleCreatures.end(), creature);
            if(it == visibleCreatures.end())
                mapView->onTileUpdate(m_position, creature, Otc::OPERATION_ADD);
        }
    }
}

bool Tile::isCompletelyCovered(int firstFloor)
{
    bool covered = g_map.isCompletelyCovered(m_position, firstFloor);
    if(!(m_covered = covered)) {
        m_covered = g_map.isCovered(m_position, firstFloor);
    }

    return covered;
}

void Tile::drawStart(const MapViewPtr& mapView)
{
    m_drawElevation = 0;
    m_shadowColor = mapView->getLastFloorShadowingColor();

    if(m_highlight.update) {
        m_highlight.fadeLevel += 10 * (m_highlight.invertedColorSelection ? 1 : -1);
        m_highlight.update = false;
        m_highlight.rgbColor = Color(255, 255, 0, m_highlight.fadeLevel);

        if(m_highlight.invertedColorSelection ? m_highlight.fadeLevel > 120 : m_highlight.fadeLevel < 0) {
            m_highlight.invertedColorSelection = !m_highlight.invertedColorSelection;
        }
    }

    if(mapView->isDrawingLights() && mapView->isDrawingFloorShadowing() && hasBorderShadowColor()) {
        g_painter->setColor(m_borderShadowColor);
    }
}

void Tile::drawEnd(const MapViewPtr& mapView)
{
    // Reset Border Shadow Color
    if(mapView->isDrawingFloorShadowing() && hasBorderShadowColor()) {
        g_painter->setColor(m_shadowColor);
    }
}

void Tile::drawThing(const ThingPtr& thing, const Point& dest, float scaleFactor, bool animate, int frameFlag, LightView* lightView)
{
    if(isCovered()) {
        if(thing->isCreature() && !isWalkable(true)) {
            const auto& tile = thing->getTile();
            if(!tile || tile->isCovered()) lightView = nullptr;
        } else lightView = nullptr;
    }

    const auto putShadowColor = g_painter->getColor() == m_borderShadowColor && (!thing->isGroundBorder() && !thing->isTall());

    if(putShadowColor) {
        g_painter->setColor(m_shadowColor);
    }

    if(thing->isEffect()) {
        thing->static_self_cast<Effect>()->drawEffect(dest - m_drawElevation * scaleFactor, scaleFactor, frameFlag, lightView);
    } else {
        thing->draw(dest, scaleFactor, animate, m_highlight, frameFlag, lightView);

        m_drawElevation += thing->getElevation();
        if(m_drawElevation > Otc::MAX_ELEVATION)
            m_drawElevation = Otc::MAX_ELEVATION;
    }

    // Reset Border Shadow Color
    if(putShadowColor) {
        g_painter->setColor(m_borderShadowColor);
    }
}

void Tile::drawGround(const Point& dest, float scaleFactor, int frameFlags, LightView* lightView)
{
    for(const auto& ground : m_ground) {
        drawThing(ground, dest - m_drawElevation * scaleFactor, scaleFactor, true, frameFlags, lightView);
    }
}

void Tile::drawBottom(const Point& dest, float scaleFactor, int frameFlags, LightView* lightView)
{
    for(const auto& item : m_bottomItems) {
        drawThing(item, dest - m_drawElevation * scaleFactor, scaleFactor, true, frameFlags, lightView);
    }

    for(auto it = m_commonItems.rbegin(); it != m_commonItems.rend(); ++it) {
        const auto& item = *it;
        drawThing(item, dest - m_drawElevation * scaleFactor, scaleFactor, true, frameFlags, lightView);
    }

#if RENDER_CREATURE_BEHIND == 1
    for(const auto& creature : m_walkingCreatures) {
        drawThing(creature, Point(
            dest.x + ((creature->getPosition().x - m_position.x) * Otc::TILE_PIXELS - m_drawElevation) * scaleFactor,
            dest.y + ((creature->getPosition().y - m_position.y) * Otc::TILE_PIXELS - m_drawElevation) * scaleFactor
        ), scaleFactor, true, frameFlags, lightView);
    }

    for(auto it = m_creatures.rbegin(); it != m_creatures.rend(); ++it) {
        const auto& creature = *it;
        if(creature->isWalking()) continue;
        drawThing(creature, dest - m_drawElevation * scaleFactor, scaleFactor, true, frameFlags, lightView);
    }
#else
    for(const auto& creature : m_creatures) {
        if(creature->isWalking()) continue;
        drawThing(creature, dest - m_drawElevation * scaleFactor, scaleFactor, true, frameFlags, lightView);
    }

    for(const auto& creature : m_walkingCreatures) {
        drawThing(creature, Point(
            dest.x + ((creature->getPosition().x - m_position.x) * Otc::TILE_PIXELS - m_drawElevation) * scaleFactor,
            dest.y + ((creature->getPosition().y - m_position.y) * Otc::TILE_PIXELS - m_drawElevation) * scaleFactor
        ), scaleFactor, true, frameFlags, lightView);
    }
#endif    
}

void Tile::drawTop(const Point& dest, float scaleFactor, int frameFlags, LightView* lightView)
{
    for(const auto& effect : m_effects) {
        drawThing(effect, dest - m_drawElevation * scaleFactor, scaleFactor, true, frameFlags, lightView);
    }

    for(const auto& item : m_topItems) {
        drawThing(item, dest, scaleFactor, true, frameFlags, lightView);
    }
}

void Tile::draw(const Point& dest, float scaleFactor, int frameFlags, LightView* lightView)
{
    drawGround(dest, scaleFactor, frameFlags, lightView);
    drawBottom(dest, scaleFactor, frameFlags, lightView);
    drawTop(dest, scaleFactor, frameFlags, lightView);
}

void Tile::clean()
{
    m_bottomItems.clear();
    m_ground.clear();
    m_topItems.clear();
    m_commonItems.clear();
    m_creatures.clear();
    m_things.clear();

    cancelScheduledPainting();
}

void Tile::addWalkingCreature(const CreaturePtr& creature)
{
    m_walkingCreatures.push_back(creature);
}

void Tile::removeWalkingCreature(const CreaturePtr& creature)
{
    const auto it = std::find(m_walkingCreatures.begin(), m_walkingCreatures.end(), creature);
    if(it != m_walkingCreatures.end())
        m_walkingCreatures.erase(it);
}

// TODO: Need refactoring
// Redo Stack Position System
void Tile::addThing(const ThingPtr& thing, int stackPos)
{
    if(!thing)
        return;

    if(thing->isEffect()) {
        const EffectPtr& effect = thing->static_self_cast<Effect>();

        // find the first effect equal and wait for it to finish.
        for(const EffectPtr& firstEffect : m_effects) {
            if(effect->getId() == firstEffect->getId()) {
                effect->waitFor(firstEffect);
            }
        }

        if(effect->isTopEffect())
            m_effects.insert(m_effects.begin(), effect);
        else
            m_effects.push_back(effect);

        analyzeThing(thing, true);

        thing->setPosition(m_position);
        thing->onAppear();
        return;
    }

    const uint8_t size = m_things.size();
    uint8_t originalStack = stackPos;

    // priority                                    854
    // 0 - ground,                        -->      -->
    // 1 - ground borders                 -->      -->
    // 2 - bottom (walls),                -->      -->
    // 3 - on top (doors)                 -->      -->
    // 4 - creatures, from top to bottom  <--      -->
    // 5 - items, from top to bottom      <--      <--
    if(stackPos < 0 || stackPos == 255) {
        const int priority = thing->getStackPriority();

        // -1 or 255 => auto detect position
        // -2        => append

        bool append;
        if(stackPos == -2)
            append = true;
        else {
            append = priority <= 3;

            // newer protocols does not store creatures in reverse order
            if(g_game.getClientVersion() >= 854 && priority == 4)
                append = !append;
        }

        for(stackPos = 0; stackPos < size; ++stackPos) {
            const int otherPriority = m_things[stackPos]->getStackPriority();
            if((append && otherPriority > priority) || (!append && otherPriority >= priority))
                break;
        }
    } else if(stackPos > static_cast<int>(size))
        stackPos = size;

    m_things.insert(m_things.begin() + stackPos, thing);

    if(thing->isCreature()) {
        const CreaturePtr& creature = thing->static_self_cast<Creature>();
        m_creatures.push_back(creature);
        if(thing->isLocalPlayer()) m_localPlayer = creature;
    } else {
        const auto& item = thing->static_self_cast<Item>();

        if(item->hasAnimationPhases()) m_animatedItems.push_back(item);

        if(thing->isGround() && hasGroundToDraw()) {
            m_ground.insert(m_ground.begin(), item);
        } else if(thing->isGroundBorder() || thing->isGround()) {
            m_ground.push_back(item);
        } else if(thing->isOnTop()) {
            m_topItems.push_back(item);
        } else if(thing->isOnBottom()) {
            m_bottomItems.push_back(item);
        } else {
            originalStack -= m_ground.size() + m_bottomItems.size() + m_creatures.size();

            if(originalStack > m_commonItems.size()) {
                m_commonItems.push_back(item);
            } else {
                m_commonItems.insert(m_commonItems.begin() + originalStack, item);
            }
        }
    }

    analyzeThing(thing, true);
    checkForDetachableThing();

    if(m_things.size() > MAX_THINGS)
        removeThing(m_things[MAX_THINGS]);

    thing->setPosition(m_position);
    thing->onAppear();

    if(thing->isTranslucent())
        checkTranslucentLight();
}

// TODO: Need refactoring
bool Tile::removeThing(const ThingPtr& thing)
{
    if(!thing) return false;

    ThingPtr temporaryReference = thing;

    if(thing->isEffect()) {
        const EffectPtr& effect = thing->static_self_cast<Effect>();
        const auto it = std::find(m_effects.begin(), m_effects.end(), effect);
        if(it == m_effects.end())
            return false;

        analyzeThing(thing, false);

        m_effects.erase(it);
        return true;
    }

    const auto it = std::find(m_things.begin(), m_things.end(), thing);
    if(it == m_things.end())
        return false;

    analyzeThing(thing, false);

    if(thing->isCreature()) {
        const auto subIt = std::find(m_creatures.begin(), m_creatures.end(), thing->static_self_cast<Creature>());
        if(subIt != m_creatures.end()) {
            if(thing->isLocalPlayer()) m_localPlayer = nullptr;
            m_creatures.erase(subIt);
        }
    } else {
        const ItemPtr& item = thing->static_self_cast<Item>();

        if(item->hasAnimationPhases()) {
            const auto& subIt = std::find(m_animatedItems.begin(), m_animatedItems.end(), item);
            if(subIt != m_animatedItems.end()) {
                m_animatedItems.erase(subIt);
            }
        }

        if(thing->isGroundBorder() || thing->isGround()) {
            const auto& subIt = std::find(m_ground.begin(), m_ground.end(), item);
            if(subIt != m_ground.end()) m_ground.erase(subIt);
        } else if(thing->isOnTop()) {
            const auto& subIt = std::find(m_topItems.begin(), m_topItems.end(), item);
            if(subIt != m_topItems.end()) m_topItems.erase(subIt);
        } else if(thing->isOnBottom()) {
            const auto& subIt = std::find(m_bottomItems.begin(), m_bottomItems.end(), item);
            if(subIt != m_bottomItems.end()) m_bottomItems.erase(subIt);
        } else {
            const auto& subIt = std::find(m_commonItems.begin(), m_commonItems.end(), item);
            if(subIt != m_commonItems.end()) m_commonItems.erase(subIt);
        }
    }

    m_things.erase(it);

    checkForDetachableThing();

    thing->onDisappear();

    if(thing->isTranslucent())
        checkTranslucentLight();

    temporaryReference = nullptr;
    return true;
}

ThingPtr Tile::getThing(int stackPos)
{
    if(stackPos >= 0 && stackPos < static_cast<int>(m_things.size()))
        return m_things[stackPos];

    return nullptr;
}

int Tile::getThingStackPos(const ThingPtr& thing)
{
    for(int stackpos = -1, s = m_things.size(); ++stackpos < s;) {
        if(thing == m_things[stackpos]) return stackpos;
    }

    return -1;
}

bool Tile::hasThing(const ThingPtr& thing)
{
    return std::find(m_things.begin(), m_things.end(), thing) != m_things.end();
}

ThingPtr Tile::getTopThing()
{
    if(isEmpty())
        return nullptr;

    if(!m_commonItems.empty()) return m_commonItems.front();

    return m_things[m_things.size() - 1];
}

std::vector<ItemPtr> Tile::getItems()
{
    std::vector<ItemPtr> items;

    for(const ThingPtr& thing : m_things) {
        if(thing->isItem())
            items.push_back(thing->static_self_cast<Item>());
    }

    return items;
}

ItemPtr Tile::getGround()
{
    if(m_ground.empty()) return nullptr;

    const auto& firstGround = m_ground.front();
    return firstGround->isGround() ? firstGround : nullptr;
}

EffectPtr Tile::getEffect(uint16 id)
{
    for(const EffectPtr& effect : m_effects)
        if(effect->getId() == id)
            return effect;

    return nullptr;
}

int Tile::getGroundSpeed()
{
    if(const ItemPtr& ground = getGround())
        return ground->getGroundSpeed();

    return 100;
}

uint8 Tile::getMinimapColorByte()
{
    if(m_minimapColor != 0)
        return m_minimapColor;

    if(!m_topItems.empty()) {
        const uint8 c = m_topItems.back()->getMinimapColor();
        if(c != 0) return c;
    }

    if(!m_bottomItems.empty()) {
        const uint8 c = m_bottomItems.back()->getMinimapColor();
        if(c != 0) return c;
    }

    const auto& ground = getGround();
    if(ground) {
        const uint8 c = ground->getMinimapColor();
        if(c != 0) return c;
    }

    return 255; // alpha
}

ThingPtr Tile::getTopLookThing()
{
    if(isEmpty()) return nullptr;

    for(auto it = m_topItems.rbegin(); it != m_topItems.rend(); ++it) {
        const ItemPtr& item = *it;
        if(!item->isIgnoreLook()) return item;
    }

    if(!m_creatures.empty()) return m_creatures.back();

    for(const auto& item : m_commonItems) {
        if(!item->isIgnoreLook()) return item;
    }

    for(auto it = m_bottomItems.rbegin(); it != m_bottomItems.rend(); ++it) {
        const ItemPtr& item = *it;
        if(!item->isIgnoreLook()) return item;
    }

    for(auto it = m_ground.rbegin(); it != m_ground.rend(); ++it) {
        const ItemPtr& item = *it;
        if(!item->isIgnoreLook()) return item;
    }

    return nullptr;
}

ThingPtr Tile::getTopUseThing()
{
    if(isEmpty()) return nullptr;

    for(const auto& item : m_commonItems) {
        if(item->isForceUse()) return item;
    }

    for(auto it = m_bottomItems.rbegin(); it != m_bottomItems.rend(); ++it) {
        const ItemPtr& item = *it;
        if(item->isForceUse()) return item;
    }

    for(auto it = m_ground.rbegin(); it != m_ground.rend(); ++it) {
        const ItemPtr& item = *it;
        if(item->isForceUse()) return item;
    }

    if(!m_topItems.empty()) return m_topItems.back();
    if(!m_commonItems.empty()) return m_commonItems.front();
    if(!m_bottomItems.empty()) return m_bottomItems.back();

    return m_ground.front();
}

CreaturePtr Tile::getTopCreature(const bool checkAround)
{
    if(!m_creatures.empty())
        return m_creatures.back();

    if(!m_walkingCreatures.empty()) {
        const CreaturePtr& creature = m_walkingCreatures.back();
        if(creature->getTile() == this)
            return creature;
    }

    // check for walking creatures in tiles around
    if(checkAround) {
        for(const auto& position : m_positionsAround) {
            const TilePtr& tile = g_map.getTile(position);
            if(!tile) continue;

            for(const CreaturePtr& c : tile->getCreatures()) {
                if(c->isWalking() && c->getLastStepFromPosition() == m_position && c->getStepProgress() < .75f) {
                    return c;
                }
            }
        }
    }

    return nullptr;
}

ThingPtr Tile::getTopMoveThing()
{
    if(isEmpty())
        return nullptr;

    for(const ThingPtr& thing : m_commonItems) {
        if(!thing->isNotMoveable()) return thing;
    }

    if(hasCreature()) return m_creatures.back();

    return nullptr;
}

ThingPtr Tile::getTopMultiUseThing()
{
    if(isEmpty()) return nullptr;

    if(CreaturePtr topCreature = getTopCreature())
        return topCreature;

    for(const ThingPtr& thing : m_commonItems) {
        if(thing->isForceUse()) return thing;
    }

    for(auto it = m_bottomItems.rbegin(); it != m_bottomItems.rend(); ++it) {
        const ItemPtr& thing = *it;
        if(thing->isForceUse()) return thing;
    }

    if(!m_commonItems.empty()) return m_commonItems.front();
    if(!m_bottomItems.empty()) return m_bottomItems.back();

    return m_ground.front();
}

bool Tile::isWalkable(bool ignoreCreatures)
{
    if(m_countFlag.notWalkable > 0 || m_ground.empty()) {
        return false;
    }

    if(!ignoreCreatures) {
        for(const CreaturePtr& creature : m_creatures) {
            if(!creature->isPassable() && creature->canBeSeen())
                return false;
        }
    }

    return true;
}

bool Tile::isPathable()
{
    return m_countFlag.notPathable == 0;
}

bool Tile::isFullGround()
{
    return m_countFlag.fullGround > 0;
}

bool Tile::isFullyOpaque()
{
    return isFullGround() || m_countFlag.opaque > 0;
}

bool Tile::isSingleDimension()
{
    return m_countFlag.notSingleDimension == 0 && m_walkingCreatures.empty();
}

bool Tile::hasTallThings()
{
    return m_countFlag.hasTallThings > 0;
}

bool Tile::hasWideThings()
{
    return m_countFlag.hasWideThings > 0;
}

bool Tile::isLookPossible()
{
    return m_countFlag.blockProjectile == 0;
}

bool Tile::isClickable()
{
    return getTopLookThing() && (!m_ground.empty() || !m_bottomItems.empty());
}

bool Tile::isEmpty()
{
    return m_things.empty();
}

bool Tile::canErase()
{
    return m_walkingCreatures.empty() && m_effects.empty() && isEmpty() && m_flags == 0 && m_minimapColor == 0;
}

bool Tile::isDrawable()
{
    return !isEmpty() || !m_walkingCreatures.empty() || !m_effects.empty();
}

bool Tile::mustHookEast()
{
    return m_countFlag.hasHookEast > 0;
}

bool Tile::mustHookSouth()
{
    return m_countFlag.hasHookSouth > 0;
}

bool Tile::hasCreature()
{
    return !m_creatures.empty();
}

bool Tile::limitsFloorsView(bool isFreeView)
{
    // ground and walls limits the view
    const ThingPtr& firstThing = getThing(0);
    return firstThing && (firstThing->isGround() || (isFreeView ? firstThing->isOnBottom() && firstThing->blockProjectile() : firstThing->isOnBottom()));
}

int Tile::getElevation() const
{
    return m_countFlag.elevation;
}

bool Tile::hasElevation(int elevation)
{
    return m_countFlag.elevation >= elevation;
}

bool Tile::hasLight()
{
    return m_countFlag.hasLight > 0;
}

void Tile::checkTranslucentLight()
{
    if(m_position.z != Otc::SEA_FLOOR)
        return;

    Position downPos = m_position;
    if(!downPos.down()) return;

    TilePtr tile = g_map.getOrCreateTile(downPos);
    if(!tile)
        return;

    for(const ThingPtr& thing : m_things) {
        if(thing->isTranslucent() || thing->hasLensHelp()) {
            tile->m_flags |= TILESTATE_TRANSLUECENT_LIGHT;
            return;
        }
    }

    tile->m_flags &= ~TILESTATE_TRANSLUECENT_LIGHT;
}

void Tile::cancelScheduledPainting()
{
    if(m_animatedItems.empty()) return;

    for(const ItemPtr& item : m_animatedItems)
        item->cancelScheduledPainting();

    m_animatedItems.clear();
}

void Tile::checkForDetachableThing()
{
    m_highlight.thing = nullptr;

    for(const ItemPtr& item : m_commonItems) {
        if((item->canDraw()) && (item->hasAction() || item->hasLensHelp() || item->isUsable() || item->isForceUse() || !item->isNotMoveable() || item->isContainer())) {
            m_highlight.thing = item;
            return;
        }
    }

    for(auto it = m_bottomItems.rbegin(); it != m_bottomItems.rend(); ++it) {
        const ItemPtr& item = *it;
        if((item->canDraw()) && (item->hasAction() || item->hasLensHelp() || item->isUsable() || item->isForceUse() || item->isContainer())) {
            m_highlight.thing = item;
            return;
        }
    }

    for(auto it = m_ground.rbegin(); it != m_ground.rend(); ++it) {
        const ItemPtr& ground = *it;
        if((ground->canDraw()) && (ground->hasAction() || ground->hasLensHelp() || ground->isUsable() || ground->isForceUse() || ground->isContainer() || ground->isTranslucent())) {
            m_highlight.thing = ground;
            return;
        }
    }

    for(auto it = m_topItems.rbegin(); it != m_topItems.rend(); ++it) {
        const ItemPtr& item = *it;
        if(item->canDraw() && item->hasLensHelp()) {
            m_highlight.thing = item;
            return;
        }
    }

    m_highlight.thing = getTopCreature();

    if(!m_highlight.thing) unselect();
}

void Tile::analyzeThing(const ThingPtr& thing, bool add)
{
    const int value = add ? 1 : -1;

    if(thing->hasLight())
        m_countFlag.hasLight += value;

    if(thing->hasDisplacement())
        m_countFlag.hasDisplacement += value;

    if(thing->isEffect()) return;

    // Creatures and items

    if(thing->isOnBottom()) {
        if(thing->isHookSouth())
            m_countFlag.hasHookSouth += value;

        if(thing->isHookEast())
            m_countFlag.hasHookEast += value;
    }

    // best option to have something more real, but in some cases as a custom project,
    // the developers are not defining crop size
    //if(thing->getRealSize() > Otc::TILE_PIXELS)
    if(thing->getHeight() != 1 || thing->getWidth() != 1)
        m_countFlag.notSingleDimension += value;

    if(thing->getHeight() > 1)
        m_countFlag.hasTallThings += value;

    if(thing->getWidth() > 1)
        m_countFlag.hasWideThings += value;

    if(thing->hasLight())
        m_countFlag.hasLight += value;

    if(!thing->isItem()) return;

    if(thing->isNotWalkable())
        m_countFlag.notWalkable += value;

    if(thing->isNotPathable())
        m_countFlag.notPathable += value;

    if(thing->blockProjectile())
        m_countFlag.blockProjectile += value;

    m_countFlag.totalElevation += thing->getElevation() * value;

    if(thing->isFullGround())
        m_countFlag.fullGround += value;

    if(thing->hasElevation())
        m_countFlag.elevation += value;

    if(thing->isOpaque())
        m_countFlag.opaque += value;

    if(thing->isTopGround())
        m_countFlag.hasTopGround += value;

    // Check that the item is opaque, so that it does not draw anything that is less than or equal below it.
    if(thing->isOpaque() && !thing->isOnTop() && !thing->isGround() && !thing->isGroundBorder()) {
        const int commonSize = m_commonItems.size();
        if(m_countFlag.elevation > (add ? 3 : 2) && commonSize > 2) {
            const ItemPtr& subItem = m_commonItems[1];
            subItem->canDraw(!add);
        } else {
            const ItemPtr& item = thing->static_self_cast<Item>();

            if(!thing->isOnBottom()) {
                for(const ItemPtr& subItem : m_commonItems) {
                    if(subItem != item) {
                        if(subItem->hasElevation() || subItem->isOpaque()) return;

                        if(subItem->getWidth() == 1 && subItem->getHeight() == 1) {
                            subItem->canDraw(!add);
                        }
                    }
                }
            }

            for(auto it = m_bottomItems.rbegin(); it != m_bottomItems.rend(); ++it) {
                const ItemPtr& subItem = *it;
                if(subItem != item) {
                    if(subItem->hasElevation() || subItem->isOpaque()) return;

                    if(subItem->getWidth() == 1 && subItem->getHeight() == 1) {
                        subItem->canDraw(!add);
                    }
                }
            }

            for(const ItemPtr& subItem : m_ground) {
                if(subItem->hasElevation()) return;

                if(subItem->getWidth() == 1 && subItem->getHeight() == 1) {
                    subItem->canDraw(!add);
                }
            }
        }
    }
}

void Tile::select()
{
    if(!m_highlight.thing) return;

    m_highlight.enabled = true;
    m_highlight.invertedColorSelection = false;
    m_highlight.fadeLevel = 0;

    m_highlight.listeningEvent = g_dispatcher.cycleEvent([=]() {
        m_highlight.update = true;
        g_map.schedulePainting(Otc::FUpdateThing);
    }, 30);
}

void Tile::unselect()
{
    if(!m_highlight.listeningEvent) return;

    m_highlight.enabled = false;
    m_highlight.listeningEvent->cancel();
    m_highlight.listeningEvent = nullptr;

    g_map.schedulePainting(Otc::FUpdateThing);
}