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

#include "tile.h"
#include "item.h"
#include "thingtypemanager.h"
#include "map.h"
#include "game.h"
#include "localplayer.h"
#include "effect.h"
#include "protocolgame.h"
#include <framework/graphics/fontmanager.h>

Tile::Tile(const Position& position) :
    m_position(position),
    m_drawElevation(0),
    m_flags(0),
    m_minimapColorByte(0)
{
}

void Tile::draw(const Point& dest, float scaleFactor, int drawFlags)
{
    bool animate = drawFlags & Otc::DrawAnimations;

    // first bottom items
    if(drawFlags & (Otc::DrawGround | Otc::DrawGroundBorders | Otc::DrawOnBottom)) {
        m_drawElevation = 0;
        for(const ThingPtr& thing : m_things) {
            if(!thing->isGround() && !thing->isGroundBorder() && !thing->isOnBottom())
                break;

            if((thing->isGround() && drawFlags & Otc::DrawGround) ||
               (thing->isGroundBorder() && drawFlags & Otc::DrawGroundBorders) ||
               (thing->isOnBottom() && drawFlags & Otc::DrawOnBottom))
                thing->draw(dest - m_drawElevation*scaleFactor, scaleFactor, animate);

            m_drawElevation += thing->getElevation();
            if(m_drawElevation > Otc::MAX_ELEVATION)
                m_drawElevation = Otc::MAX_ELEVATION;
        }
    }

    int redrawPreviousTopW = 0;
    int redrawPreviousTopH = 0;

    if(drawFlags & Otc::DrawItems) {
        // now common items in reverse order
        for(auto it = m_things.rbegin(); it != m_things.rend(); ++it) {
            const ThingPtr& thing = *it;
            if(thing->isOnTop() || thing->isOnBottom() || thing->isGroundBorder() || thing->isGround() || thing->isCreature())
                break;
            thing->draw(dest - m_drawElevation*scaleFactor, scaleFactor, animate);

            if(thing->isLyingCorpse()) {
                redrawPreviousTopW = std::max(thing->getWidth(), redrawPreviousTopW);
                redrawPreviousTopH = std::max(thing->getHeight(), redrawPreviousTopH);
            }

            m_drawElevation += thing->getElevation();
            if(m_drawElevation > Otc::MAX_ELEVATION)
                m_drawElevation = Otc::MAX_ELEVATION;
        }
    }

    // after we render 2x2 lying corpses, we must redraw previous creatures/ontop above them
    if(redrawPreviousTopH > 0 || redrawPreviousTopW > 0) {
        int topRedrawFlags = drawFlags & (Otc::DrawCreatures | Otc::DrawEffects | Otc::DrawOnTop | Otc::DrawAnimations);
        if(topRedrawFlags) {
            for(int x=-redrawPreviousTopW;x<=0;++x) {
                for(int y=-redrawPreviousTopH;y<=0;++y) {
                    if(x == 0 && y == 0)
                        continue;
                    const TilePtr& tile = g_map.getTile(m_position.translated(x,y));
                    if(tile)
                        tile->draw(dest + Point(x*Otc::TILE_PIXELS, y*Otc::TILE_PIXELS)*scaleFactor, scaleFactor, topRedrawFlags);
                }
            }
        }
    }

    // creatures
    if(drawFlags & Otc::DrawCreatures) {
        if(animate) {
            for(const CreaturePtr& creature : m_walkingCreatures) {
                if(creature->isRemoved())
                    continue;
                creature->draw(Point(dest.x + ((creature->getPosition().x - m_position.x)*Otc::TILE_PIXELS - m_drawElevation)*scaleFactor,
                                     dest.y + ((creature->getPosition().y - m_position.y)*Otc::TILE_PIXELS - m_drawElevation)*scaleFactor), scaleFactor, animate);

            }
        }

        for(auto it = m_things.rbegin(); it != m_things.rend(); ++it) {
            const ThingPtr& thing = *it;
            if(!thing->isCreature())
                continue;
            CreaturePtr creature = thing->asCreature();
            if(creature && (!creature->isWalking() || !animate))
                creature->draw(dest - m_drawElevation*scaleFactor, scaleFactor, animate);
        }
    }

    // effects
    if(drawFlags & Otc::DrawEffects) {
        for(const EffectPtr& effect : m_effects)
            effect->draw(dest - m_drawElevation*scaleFactor, scaleFactor, animate);
    }

    // top items
    if(drawFlags & Otc::DrawOnTop) {
        for(const ThingPtr& thing : m_things) {
            if(thing->isOnTop())
                thing->draw(dest, scaleFactor, animate);
        }
    }
}

void Tile::clean()
{
    while(!m_things.empty())
        removeThing(m_things.front());
}

void Tile::addWalkingCreature(const CreaturePtr& creature)
{
    m_walkingCreatures.push_back(creature);
}

void Tile::removeWalkingCreature(const CreaturePtr& creature)
{
    auto it = std::find(m_walkingCreatures.begin(), m_walkingCreatures.end(), creature);
    if(it != m_walkingCreatures.end())
        m_walkingCreatures.erase(it);
}

ThingPtr Tile::addThing(const ThingPtr& thing, int stackPos)
{
    if(!thing)
        return nullptr;

    if(EffectPtr effect = thing->asEffect()) {
        m_effects.push_back(effect);
        return nullptr;
    }

    // the items stackpos follows this order:
    // 0 - ground
    // 1 - ground borders
    // 2 - bottom (walls)
    // 3 - on top (doors)
    // 4 - creatures, from top to bottom
    // 5 - items, from top to bottom
    if(stackPos < 0) {
        int priority = thing->getStackPriority();
        bool prepend = (stackPos == -2 || priority <= 3);
        for(stackPos = 0; stackPos < (int)m_things.size(); ++stackPos) {
            int otherPriority = m_things[stackPos]->getStackPriority();
            if((prepend && otherPriority > priority) || (!prepend && otherPriority >= priority))
                break;
        }
    } else if(stackPos > (int)m_things.size())
        stackPos = m_things.size();

    ThingPtr oldObject;
    if(stackPos < (int)m_things.size())
        oldObject = m_things[stackPos];
    m_things.insert(m_things.begin() + stackPos, thing);

    if(m_things.size() > MAX_THINGS)
        removeThing(m_things[MAX_THINGS]);

    update();
    return oldObject;
}

bool Tile::removeThing(ThingPtr thing)
{
    if(!thing)
        return false;

    bool removed = false;

    if(EffectPtr effect = thing->asEffect()) {
        auto it = std::find(m_effects.begin(), m_effects.end(), effect);
        if(it != m_effects.end()) {
            m_effects.erase(it);
            removed = true;
        }
    } else {
        auto it = std::find(m_things.begin(), m_things.end(), thing);
        if(it != m_things.end()) {
            m_things.erase(it);
            removed = true;
        }
    }

    // reset values managed by this tile
    if(removed)
        update();

    return removed;
}

ThingPtr Tile::getThing(int stackPos)
{
    if(stackPos >= 0 && stackPos < (int)m_things.size())
        return m_things[stackPos];
    return nullptr;
}

EffectPtr Tile::getEffect(uint16 id)
{
    for(const EffectPtr& effect : m_effects)
        if(effect->getId() == id)
            return effect;
    return nullptr;
}

bool Tile::hasThing(const ThingPtr& thing)
{
    return std::find(m_things.begin(), m_things.end(), thing) != m_things.end();
}

int Tile::getThingStackpos(const ThingPtr& thing)
{
    for(uint stackpos = 0; stackpos < m_things.size(); ++stackpos)
        if(thing == m_things[stackpos])
            return stackpos;
    return -1;
}

ThingPtr Tile:: getTopThing()
{
    if(isEmpty())
        return nullptr;
    for(const ThingPtr& thing : m_things) {
        if(!thing->isGround() && !thing->isGroundBorder() && !thing->isOnBottom() && !thing->isOnTop() && !thing->isCreature())
            return thing;
    }
    return m_things[m_things.size() - 1];
}

std::vector<ItemPtr> Tile::getItems()
{
    std::vector<ItemPtr> items;
    for(const ThingPtr& thing : m_things) {
        if(ItemPtr item = thing->asItem())
            items.push_back(item);
    }
    return items;
}

std::vector<CreaturePtr> Tile::getCreatures()
{
    std::vector<CreaturePtr> creatures;
    for(const ThingPtr& thing : m_things) {
        if(CreaturePtr creature = thing->asCreature())
            creatures.push_back(creature);
    }
    return creatures;
}

ItemPtr Tile::getGround()
{
    ThingPtr firstObject = getThing(0);
    if(!firstObject)
        return nullptr;
    if(firstObject->isGround())
        return firstObject->asItem();
    return nullptr;
}

int Tile::getGroundSpeed()
{
    int groundSpeed = 100;
    if(ItemPtr ground = getGround())
        groundSpeed = ground->getGroundSpeed();
    return groundSpeed;
}

ThingPtr Tile::getTopLookThing()
{
    if(isEmpty())
        return nullptr;

    for(uint i = 0; i < m_things.size(); ++i) {
        ThingPtr thing = m_things[i];
        if(!thing->isIgnoreLook() && (!thing->isGround() && !thing->isGroundBorder() && !thing->isOnBottom() && !thing->isOnTop()))
            return thing;
    }

    return m_things[0];
}

ThingPtr Tile::getTopUseThing()
{
    if(isEmpty())
        return nullptr;

    for(uint i = 0; i < m_things.size(); ++i) {
        ThingPtr thing = m_things[i];
        if(thing->isForceUse() || (!thing->isGround() && !thing->isGroundBorder() && !thing->isOnBottom() && !thing->isOnTop() && !thing->isCreature()))
            return thing;
    }

    return m_things[0];
}

CreaturePtr Tile::getTopCreature()
{
    CreaturePtr creature;
    for(uint i = 0; i < m_things.size(); ++i) {
        ThingPtr thing = m_things[i];
        if(thing->asLocalPlayer()) // return local player if there is no other creature
            creature = thing->asCreature();
        else if(thing->isCreature() && !thing->isLocalPlayer())
            return thing->asCreature();
    }
    if(!creature && !m_walkingCreatures.empty())
        creature = m_walkingCreatures.back();
    return creature;
}

ThingPtr Tile::getTopMoveThing()
{
    if(isEmpty())
        return nullptr;

    for(uint i = 0; i < m_things.size(); ++i) {
        ThingPtr thing = m_things[i];
        if(!thing->isGround() && !thing->isGroundBorder() && !thing->isOnBottom() && !thing->isOnTop() && !thing->isCreature()) {
            if(i > 0 && thing->isNotMoveable())
                return m_things[i-1];
            return thing;
        }
    }

    return m_things[0];
}

ThingPtr Tile::getTopMultiUseThing()
{
    // this is related to classic controls, getting top item, forceuse or creature
    if(isEmpty())
        return nullptr;

    for(uint i = 0; i < m_things.size(); ++i) {
        ThingPtr thing = m_things[i];
        if(thing->isForceUse() || (!thing->isGround() && !thing->isGroundBorder() && !thing->isOnBottom() && !thing->isOnTop())) {
            if(i > 0 && thing->isSplash())
                return m_things[i-1];
            return thing;
        }
    }

    for(uint i = 0; i < m_things.size(); ++i) {
        ThingPtr thing = m_things[i];
        if(!thing->isGround() && !thing->isGroundBorder() && !thing->isOnTop())
            return thing;
    }

    return m_things[0];
}

bool Tile::isWalkable()
{
    if(!getGround())
        return false;

    for(const ThingPtr& thing : m_things) {
        if(thing->isNotWalkable())
            return false;

        if(CreaturePtr creature = thing->asCreature()) {
            if(!creature->isPassable())
                return false;
        }
    }
    return true;
}

bool Tile::isPathable()
{
    if(!isWalkable())
        return false;

    for(const ThingPtr& thing : m_things) {
        if(thing->isNotPathable() || thing->isCreature())
            return false;
    }

    return true;
}

bool Tile::isFullGround()
{
    ItemPtr ground = getGround();
    if(ground && ground->isFullGround())
        return true;
    return false;
}

bool Tile::isFullyOpaque()
{
    ThingPtr firstObject = getThing(0);
    return firstObject && firstObject->isFullGround();
}

bool Tile::isLookPossible()
{
    for(const ThingPtr& thing : m_things) {
        if(thing->blockProjectile())
            return false;
    }
    return true;
}

bool Tile::isClickable()
{
    bool hasGround = false;
    bool hasOnBottom = false;
    bool hasIgnoreLook = false;
    for(const ThingPtr& thing : m_things) {
        if(thing->isGround())
            hasGround = true;
        if(thing->isOnBottom())
            hasOnBottom = true;
        if(thing->isIgnoreLook())
            hasIgnoreLook = true;

        if((hasGround || hasOnBottom) && !hasIgnoreLook)
            return true;
    }
    return false;
}

bool Tile::isEmpty()
{
    return m_things.size() == 0;
}

bool Tile::mustHookEast()
{
    for(const ThingPtr& thing : m_things)
        if(thing->isHookEast())
            return true;
    return false;
}

bool Tile::mustHookSouth()
{
    for(const ThingPtr& thing : m_things)
        if(thing->isHookSouth())
            return true;
    return false;
}

bool Tile::hasCreature()
{
    for(const ThingPtr& thing : m_things)
        if(thing->asCreature())
            return true;
    return false;
}

bool Tile::limitsFloorsView()
{
    // ground and walls limits the view
    ThingPtr firstThing = getThing(0);
    if(firstThing && !firstThing->isDontHide() && (firstThing->isGround() || firstThing->isOnBottom()))
        return true;
    return false;
}

bool Tile::canErase()
{
    return m_walkingCreatures.empty() && m_effects.empty() && m_things.empty();
}

void Tile::update()
{
    m_minimapColorByte = 0;
    for(const ThingPtr& thing : m_things) {
        if(!thing->isGround() && !thing->isGroundBorder() && !thing->isOnBottom() && !thing->isOnTop())
            break;
        uint8 c = thing->getMinimapColor();
        if(c != 0)
            m_minimapColorByte = c;
    }

    // check stack priorities
    // this code exists to find stackpos bugs faster
    int lastPriority = 0;
    for(const ThingPtr& thing : m_things) {
        int priority = thing->getStackPriority();
        assert(lastPriority <= priority);
        lastPriority = priority;
    }
}
