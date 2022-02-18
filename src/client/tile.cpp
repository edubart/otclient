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
#include <framework/core/eventdispatcher.h>
#include <framework/graphics/drawpool.h>
#include "effect.h"
#include "game.h"
#include "item.h"
#include "lightview.h"
#include "map.h"
#include "protocolgame.h"

Tile::Tile(const Position& position) : m_position(position), m_positionsAround(m_position.getPositionsAround()) {}

void Tile::drawThing(const ThingPtr& thing, const Point& dest, float scaleFactor, bool animate, LightView* lightView)
{
    if(thing->isEffect()) {
        thing->static_self_cast<Effect>()->drawEffect(dest, scaleFactor, lightView);
    } else {
        thing->draw(dest, scaleFactor, animate, m_highlight, TextureType::NONE, Color::white, lightView);

        m_drawElevation += thing->getElevation();
        if(m_drawElevation > MAX_ELEVATION)
            m_drawElevation = MAX_ELEVATION;
    }
}

void Tile::drawGround(const Point& dest, float scaleFactor, LightView* lightView)
{
    m_drawElevation = 0;

    for(const auto& ground : m_things) {
        if(ground->isSingleGround() || ground->isSingleGroundBorder()) {
            drawThing(ground, dest - m_drawElevation * scaleFactor, scaleFactor, true, lightView);
        }
    }
}

void Tile::drawSurface(const Point& dest, float scaleFactor, LightView* lightView)
{
    drawBottom(dest, scaleFactor, lightView);
    drawTop(dest, scaleFactor, lightView);
}

void Tile::drawCreature(const Point& dest, float scaleFactor, LightView* lightView)
{
#if RENDER_WALKING_CREATURES_BEHIND == 1
    for(const auto& creature : m_walkingCreatures) {
        drawThing(creature, Point(
            dest.x + ((creature->getPosition().x - m_position.x) * SPRITE_SIZE - m_drawElevation) * scaleFactor,
            dest.y + ((creature->getPosition().y - m_position.y) * SPRITE_SIZE - m_drawElevation) * scaleFactor
        ), scaleFactor, true, lightView);
    }

    if(hasCreature()) {
        for(auto it = m_things.rbegin(); it != m_things.rend(); ++it) {
            const auto& thing = *it;
            if(!thing->isCreature() || thing->static_self_cast<Creature>()->isWalking()) continue;

            drawThing(thing, dest - m_drawElevation * scaleFactor, scaleFactor, true, lightView);
        }
    }
#else
    if(hasCreature()) {
        for(const auto& thing : m_things) {
            if(!thing->isCreature() || thing->static_self_cast<Creature>()->isWalking()) continue;

            drawThing(thing, dest - m_drawElevation * scaleFactor, scaleFactor, true, lightView);
        }
    }

    for(const auto& creature : m_walkingCreatures) {
        drawThing(creature, Point(
            dest.x + ((creature->getPosition().x - m_position.x) * SPRITE_SIZE - m_drawElevation) * scaleFactor,
            dest.y + ((creature->getPosition().y - m_position.y) * SPRITE_SIZE - m_drawElevation) * scaleFactor
        ), scaleFactor, true, lightView);
    }
#endif
}

void Tile::drawBottom(const Point& dest, float scaleFactor, LightView* lightView)
{
    if(!hasGround()) m_drawElevation = 0;

    if(hasTopGround()) {
        for(const auto& ground : m_things) {
            if(ground->isTopGround() || ground->isTopGroundBorder())
                drawThing(ground, dest - m_drawElevation * scaleFactor, scaleFactor, true, lightView);
        }
    }

    if(m_countFlag.hasBottomItem) {
        for(const auto& item : m_things) {
            if(!item->isOnBottom()) continue;
            drawThing(item, dest - m_drawElevation * scaleFactor, scaleFactor, true, lightView);
        }
    }

    uint8 redrawPreviousTopW = 0,
        redrawPreviousTopH = 0;

    if(m_countFlag.hasCommonItem) {
        for(auto it = m_things.rbegin(); it != m_things.rend(); ++it) {
            const auto& item = *it;
            if(!item->isCommon()) continue;

            drawThing(item, dest - m_drawElevation * scaleFactor, scaleFactor, true, lightView);

            if(item->isLyingCorpse()) {
                redrawPreviousTopW = std::max<int>(item->getWidth(), redrawPreviousTopW);
                redrawPreviousTopH = std::max<int>(item->getHeight(), redrawPreviousTopH);
            }
        }
    }

    // after we render 2x2 lying corpses, we must redraw previous creatures/ontop above them
    if(redrawPreviousTopH > 0 || redrawPreviousTopW > 0) {
        for(int x = -redrawPreviousTopW; x <= 0; ++x) {
            for(int y = -redrawPreviousTopH; y <= 0; ++y) {
                if(x == 0 && y == 0)
                    continue;
                const TilePtr& tile = g_map.getTile(m_position.translated(x, y));
                if(tile) {
                    const auto& newDest = dest + (Point(x, y) * SPRITE_SIZE) * scaleFactor;
                    tile->drawCreature(newDest, scaleFactor);
                    tile->drawTop(newDest, scaleFactor);
                }
            }
        }
    }

    drawCreature(dest, scaleFactor, lightView);
}

void Tile::drawTop(const Point& dest, float scaleFactor, LightView* lightView)
{
    if(!g_app.isDrawingEffectsOnTop()) {
        for(const auto& effect : m_effects) {
            drawThing(effect, dest - m_drawElevation * scaleFactor, scaleFactor, true, lightView);
        }
    }

    if(m_countFlag.hasTopItem) {
        for(const auto& item : m_things) {
            if(!item->isOnTop()) continue;
            drawThing(item, dest, scaleFactor, true, lightView);
        }
    }
}

void Tile::clean()
{
    m_countFlag.opaque = 0;
    while(!m_things.empty())
        removeThing(m_things.front());
}

void Tile::addWalkingCreature(const CreaturePtr& creature)
{
    m_walkingCreatures.push_back(creature);
    m_ignoreCompletelyCoveredCheck = true;
    analyzeThing(creature, true);
}

void Tile::removeWalkingCreature(const CreaturePtr& creature)
{
    const auto it = std::find(m_walkingCreatures.begin(), m_walkingCreatures.end(), creature);
    if(it != m_walkingCreatures.end()) {
        analyzeThing(creature, false);
        m_walkingCreatures.erase(it);
        m_ignoreCompletelyCoveredCheck = false;
        checkForDetachableThing();
    }
}

// TODO: Need refactoring
// Redo Stack Position System
void Tile::addThing(const ThingPtr& thing, int stackPos)
{
    if(!thing)
        return;

    if(thing->isEffect()) {
        const EffectPtr& newEffect = thing->static_self_cast<Effect>();

        const bool canOptimize = g_app.canOptimize() || g_app.isForcedEffectOptimization();

        for(const EffectPtr& prevEffect : m_effects) {
            if(!prevEffect->canDraw())
                continue;

            if(canOptimize && newEffect->getSize() > prevEffect->getSize()) {
                prevEffect->canDraw(false);
            } else if(canOptimize || newEffect->getId() == prevEffect->getId()) {
                newEffect->waitFor(prevEffect);
            }
        }

        if(newEffect->isTopEffect())
            m_effects.insert(m_effects.begin(), newEffect);
        else
            m_effects.push_back(newEffect);

        analyzeThing(thing, true);

        thing->setPosition(m_position);
        thing->onAppear();
        return;
    }

    const uint8_t size = m_things.size();

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

    if(thing->isGround()) m_ground = thing->static_self_cast<Item>();

    analyzeThing(thing, true);
    if(checkForDetachableThing() && m_highlight.enabled) {
        select();
    }

    if(size > MAX_THINGS)
        removeThing(m_things[MAX_THINGS]);

    thing->setPosition(m_position);
    thing->onAppear();

    if(thing->isTranslucent())
        checkTranslucentLight();
}

// TODO: Need refactoring
bool Tile::removeThing(const ThingPtr thing)
{
    if(!thing) return false;

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

    if(thing->isGround()) m_ground = nullptr;

    analyzeThing(thing, false);

    m_things.erase(it);

    checkForDetachableThing();

    thing->onDisappear();

    if(thing->isTranslucent())
        checkTranslucentLight();

    return true;
}

ThingPtr Tile::getThing(int stackPos)
{
    if(stackPos >= 0 && stackPos < static_cast<int>(m_things.size()))
        return m_things[stackPos];

    return nullptr;
}

std::vector<CreaturePtr> Tile::getCreatures()
{
    std::vector<CreaturePtr> creatures;
    if(hasCreature()) {
        for(const ThingPtr& thing : m_things) {
            if(thing->isCreature())
                creatures.push_back(thing->static_self_cast<Creature>());
        }
    }

    return creatures;
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

    for(const ThingPtr& thing : m_things)
        if(thing->isCommon())
            return thing;

    return m_things[m_things.size() - 1];
}

std::vector<ItemPtr> Tile::getItems()
{
    std::vector<ItemPtr> items;
    for(const ThingPtr& thing : m_things) {
        if(!thing->isItem())
            continue;

        items.push_back(thing->static_self_cast<Item>());
    }
    return items;
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

    for(auto it = m_things.rbegin(); it != m_things.rend(); ++it) {
        const auto& thing = *it;
        if(thing->isCreature() || thing->isCommon())
            continue;

        const uint8 c = thing->getMinimapColor();
        if(c != 0) return c;
    }

    return 255;
}

ThingPtr Tile::getTopLookThing()
{
    if(isEmpty())
        return nullptr;

    for(const auto& thing : m_things) {
        if(!thing->isIgnoreLook() && (!thing->isGround() && !thing->isGroundBorder() && !thing->isOnBottom() && !thing->isOnTop()))
            return thing;
    }

    return m_things[0];
}

ThingPtr Tile::getTopUseThing()
{
    if(isEmpty())
        return nullptr;

    for(const auto& thing : m_things) {
        if(thing->isForceUse() || (!thing->isGround() && !thing->isGroundBorder() && !thing->isOnBottom() && !thing->isOnTop() && !thing->isCreature() && !thing->isSplash()))
            return thing;
    }

    for(const auto& thing : m_things) {
        if(!thing->isGround() && !thing->isGroundBorder() && !thing->isCreature() && !thing->isSplash())
            return thing;
    }

    return m_things[0];
}

CreaturePtr Tile::getTopCreature(const bool checkAround)
{
    if(!hasCreature()) return nullptr;

    CreaturePtr creature;
    for(const auto& thing : m_things) {
        if(thing->isLocalPlayer()) // return local player if there is no other creature
            creature = thing->static_self_cast<Creature>();
        else if(thing->isCreature())
            return thing->static_self_cast<Creature>();
    }

    if(creature)
        return creature;

    if(!m_walkingCreatures.empty())
        return m_walkingCreatures.back();

    // check for walking creatures in tiles around
    if(checkAround) {
        for(const auto& pos : m_positionsAround) {
            const TilePtr& tile = g_map.getTile(pos);
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

    for(int8 i = -1, s = m_things.size(); ++i < s;) {
        const ThingPtr& thing = m_things[i];
        if(thing->isCommon()) {
            if(i > 0 && thing->isNotMoveable())
                return m_things[i - 1];

            return thing;
        }
    }

    for(const ThingPtr& thing : m_things) {
        if(thing->isCreature())
            return thing;
    }

    return m_things[0];
}

ThingPtr Tile::getTopMultiUseThing()
{
    if(isEmpty())
        return nullptr;

    if(const CreaturePtr& topCreature = getTopCreature())
        return topCreature;

    for(const auto& thing : m_things) {
        if(thing->isForceUse())
            return thing;
    }

    for(int8 i = -1, s = m_things.size(); ++i < s;) {
        const ThingPtr& thing = m_things[i];
        if(!thing->isGround() && !thing->isGroundBorder() && !thing->isOnBottom() && !thing->isOnTop()) {
            if(i > 0 && thing->isSplash())
                return m_things[i - 1];

            return thing;
        }
    }

    for(const auto& thing : m_things) {
        if(!thing->isGround() && !thing->isOnTop())
            return thing;
    }

    return m_things[0];
}

bool Tile::isWalkable(bool ignoreCreatures)
{
    if(m_countFlag.notWalkable > 0 || !getGround()) {
        return false;
    }

    if(!ignoreCreatures && hasCreature()) {
        for(const auto& thing : m_things) {
            if(!thing->isCreature()) continue;

            const auto& creature = thing->static_self_cast<Creature>();
            if(!creature->isPassable() && creature->canBeSeen())
                return false;
        }
    }

    return true;
}

bool Tile::isCompletelyCovered(int8 firstFloor)
{
    if(m_ignoreCompletelyCoveredCheck || hasLight())
        return false;

    if(firstFloor > -1) {
        m_completelyCovered = g_map.isCompletelyCovered(m_position, firstFloor);
        if((m_covered = m_completelyCovered) == false) {
            m_covered = g_map.isCovered(m_position, firstFloor);
        }
    }

    return m_completelyCovered;
}

bool Tile::isClickable()
{
    bool hasGround = false;
    bool hasOnBottom = false;
    bool hasIgnoreLook = false;
    for(const ThingPtr& thing : m_things) {
        if(thing->isGround())
            hasGround = true;
        else if(thing->isOnBottom())
            hasOnBottom = true;

        if(thing->isIgnoreLook())
            hasIgnoreLook = true;

        if((hasGround || hasOnBottom) && !hasIgnoreLook)
            return true;
    }

    return false;
}

bool Tile::canShade(const MapViewPtr& mapView)
{
    for(auto dir : { Otc::North, Otc::NorthWest, Otc::West }) {
        const auto& pos = m_position.translatedToDirection(dir);
        const auto& tile = g_map.getTile(pos);

        if(!tile && mapView->isInRangeEx(pos, true) || tile && !tile->isFullyOpaque() && !tile->isFullGround() && !tile->hasTopGround(true))
            return false;
    }

    return isFullyOpaque() || hasTopGround(true) || isFullGround();
}

bool Tile::hasBlockingCreature()
{
    for(const ThingPtr& thing : m_things)
        if(thing->isCreature() && !thing->static_self_cast<Creature>()->isPassable() && !thing->isLocalPlayer())
            return true;
    return false;
}

bool Tile::limitsFloorsView(bool isFreeView)
{
    // ground and walls limits the view
    const ThingPtr& firstThing = getThing(0);
    return firstThing && (firstThing->isGround() || (isFreeView ? firstThing->isOnBottom() && firstThing->blockProjectile() : firstThing->isOnBottom()));
}

void Tile::checkTranslucentLight()
{
    if(m_position.z != SEA_FLOOR)
        return;

    Position downPos = m_position;
    if(!downPos.down()) return;

    const TilePtr tile = g_map.getOrCreateTile(downPos);
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

bool Tile::checkForDetachableThing()
{
    if(m_highlight.thing = getTopCreature())
        return true;

    if(m_highlightWithoutFilter) {
        for(auto it = m_things.rbegin(); it != m_things.rend(); ++it) {
            const auto& item = *it;
            if(!item->canDraw()) continue;

            m_highlight.thing = item;
            return true;
        }

        return false;
    }

    if(m_countFlag.hasCommonItem) {
        for(const auto& item : m_things) {
            if((!item->isCommon() || !item->canDraw() || item->isIgnoreLook() || item->isCloth()) && (!item->isUsable()) && (!item->hasLight())) {
                continue;
            }

            m_highlight.thing = item;
            return true;
        }
    }

    if(m_countFlag.hasBottomItem) {
        for(auto it = m_things.rbegin(); it != m_things.rend(); ++it) {
            const auto& item = *it;
            if(!item->isOnBottom() || !item->canDraw() || item->isIgnoreLook() || item->isFluidContainer()) continue;
            m_highlight.thing = item;
            return true;
        }
    }

    if(m_countFlag.hasTopItem) {
        for(auto it = m_things.rbegin(); it != m_things.rend(); ++it) {
            const auto& item = *it;
            if(!item->isOnTop()) break;
            if(!item->canDraw() || item->isIgnoreLook()) continue;

            if(item->hasLensHelp()) {
                m_highlight.thing = item;
                return true;
            }
        }
    }

    return false;
}

void Tile::analyzeThing(const ThingPtr& thing, bool add)
{
    const int value = add ? 1 : -1;

    if(thing->hasLight())
        m_countFlag.hasLight += value;

    if(thing->hasDisplacement())
        m_countFlag.hasDisplacement += value;

    if(thing->isEffect()) return;

    if(thing->isCommon())
        m_countFlag.hasCommonItem += value;

    if(thing->isOnTop())
        m_countFlag.hasTopItem += value;

    if(thing->isCreature())
        m_countFlag.hasCreature += value;

    if(thing->isSingleGroundBorder())
        m_countFlag.hasGroundBorder += value;

    if(thing->isTopGroundBorder())
        m_countFlag.hasTopGroundBorder += value;

    // Creatures and items
    if(thing->isOnBottom()) {
        m_countFlag.hasBottomItem += value;

        if(thing->isHookSouth())
            m_countFlag.hasHookSouth += value;

        if(thing->isHookEast())
            m_countFlag.hasHookEast += value;
    }

    // best option to have something more real, but in some cases as a custom project,
    // the developers are not defining crop size
    //if(thing->getRealSize() > SPRITE_SIZE)
    if(!thing->isSingleDimension() || thing->hasElevation() || thing->hasDisplacement())
        m_countFlag.notSingleDimension += value;

    if(thing->getHeight() > 1)
        m_countFlag.hasTallThings += value;

    if(thing->getWidth() > 1)
        m_countFlag.hasWideThings += value;

    if(!thing->isItem()) return;

    if(thing->getHeight() > 1)
        m_countFlag.hasTallItems += value;

    if(thing->getWidth() > 1)
        m_countFlag.hasWideItems += value;

    if(thing->getWidth() > 1 && thing->getHeight() > 1)
        m_countFlag.hasWall += value;

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

    if(thing->isOpaque()) {
        m_countFlag.opaque = std::max<int>(m_countFlag.opaque + value, 0);
    }

    if(thing->isGroundBorder() && thing->isNotWalkable())
        m_countFlag.hasNoWalkableEdge += value;
}

void Tile::select(const bool noFilter)
{
    unselect();
    if(noFilter != m_highlightWithoutFilter) {
        m_highlightWithoutFilter = noFilter;
        checkForDetachableThing();
    }

    if(!m_highlight.thing) return;

    m_highlight.enabled = true;
    m_highlight.invertedColorSelection = false;
    m_highlight.fadeLevel = HIGHTLIGHT_FADE_START;

    const auto self = this->static_self_cast<Tile>();
    m_highlight.listeningEvent = g_dispatcher.cycleEvent([self]() {
        auto& highLight = self->m_highlight;

        highLight.fadeLevel += 10 * (highLight.invertedColorSelection ? 1 : -1);
        highLight.rgbColor = Color(static_cast<uint8>(255), static_cast<uint8>(255), static_cast<uint8>(0), static_cast<uint8>(highLight.fadeLevel));

        if(highLight.invertedColorSelection ? highLight.fadeLevel > HIGHTLIGHT_FADE_END : highLight.fadeLevel < HIGHTLIGHT_FADE_START) {
            highLight.invertedColorSelection = !highLight.invertedColorSelection;
        }
    }, 40);
}

void Tile::unselect()
{
    m_highlight.enabled = false;
    if(m_highlight.listeningEvent) {
        m_highlight.listeningEvent->cancel();
        m_highlight.listeningEvent = nullptr;
    }
}

bool Tile::canRender(const bool drawViewportEdge, const Position& cameraPosition, const AwareRange viewPort, LightView* lightView)
{
    if(drawViewportEdge || (lightView && lightView->isDark() && hasLight())) return true;

    const int8 dz = m_position.z - cameraPosition.z;
    const Position checkPos = m_position.translated(dz, dz);

    // Check for non-visible tiles on the screen and ignore them
    {
        if((cameraPosition.x - checkPos.x >= viewPort.left) || (checkPos.x - cameraPosition.x == viewPort.right && !hasWideThings() && !hasDisplacement() && m_walkingCreatures.empty()))
            return false;

        if((cameraPosition.y - checkPos.y >= viewPort.top) || (checkPos.y - cameraPosition.y == viewPort.bottom && !hasTallThings() && !hasDisplacement() && m_walkingCreatures.empty()))
            return false;

        if((checkPos.x - cameraPosition.x > viewPort.right && (!hasWideThings() || !hasDisplacement())) || (checkPos.y - cameraPosition.y > viewPort.bottom))
            return false;
    }

    return true;
}
