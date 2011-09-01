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

#include "tile.h"
#include "item.h"
#include "thingstype.h"
#include "map.h"
#include "game.h"
#include "localplayer.h"
#include "effect.h"
#include <framework/graphics/fontmanager.h>

Tile::Tile(const Position& position)
{
    m_drawElevation = 0;
    m_position = position;
}

void Tile::draw(int x, int y)
{
    m_drawElevation = 0;

    // first bottom items
    for(const ThingPtr& thing : m_things) {
        const ThingType& type = thing->getType();
        if(!type.isGround && !type.isGroundClip && !type.isOnBottom)
            break;
        thing->draw(x - m_drawElevation, y - m_drawElevation);
        m_drawElevation += type.elevation;
        if(m_drawElevation > MAX_DRAW_ELEVATION)
            m_drawElevation = MAX_DRAW_ELEVATION;
    }

    // now common items
    for(auto it = m_things.rbegin(); it != m_things.rend(); ++it) {
        const ThingPtr& thing = *it;
        const ThingType& type = thing->getType();
        if(thing->asCreature() || type.isOnTop || type.isOnBottom || type.isGroundClip || type.isGround)
            break;
        thing->draw(x - m_drawElevation, y - m_drawElevation);
        m_drawElevation += type.elevation;
        if(m_drawElevation > MAX_DRAW_ELEVATION)
            m_drawElevation = MAX_DRAW_ELEVATION;
    }

    // we can render creatures in 3x3 range
    //TODO: this algorithm is slowing down render too much, but it could be cached to improve framerate
    for(int xi = -1; xi <= 1; ++xi) {
        for(int yi = -1; yi <= 1; ++yi) {
            for(CreaturePtr creature : g_map.getTile(m_position + Position(xi, yi, 0))->getCreatures()) {
                auto& type = creature->getType();
                Rect creatureRect(x + xi*32 + creature->getWalkOffsetX() - type.xDisplacement, y + yi*32 + creature->getWalkOffsetY() - type.yDisplacement, 32, 32);
                Rect thisTileRect(x, y, 32, 32);

                // only render creatures where bottom right is inside our rect
                if(thisTileRect.contains(creatureRect.bottomRight()))
                    creature->draw(x + xi*32 - m_drawElevation, y + yi*32 - m_drawElevation);
            }
        }
    }

    // effects
    for(const EffectPtr& effect : m_effects)
        effect->draw(x - m_drawElevation, y - m_drawElevation);

    // top items
    for(const ThingPtr& thing : m_things) {
        const ThingType& type = thing->getType();
        if(type.isOnTop)
            thing->draw(x - m_drawElevation, y - m_drawElevation);
    }
}

void Tile::clean()
{
    m_things.clear();
    m_effects.clear();
}

void Tile::addEffect(const EffectPtr& effect)
{
    m_effects.push_back(effect);
    effect->setPosition(m_position);
}

void Tile::removeEffect(const EffectPtr& effect)
{
    auto it = std::find(m_effects.begin(), m_effects.end(), effect);
    if(it != m_effects.end()) {
        m_effects.erase(it);
    }
}

ThingPtr Tile::addThing(const ThingPtr& thing, int stackPos)
{
    if(!thing)
        return nullptr;

    if(stackPos < 0) {
        stackPos = 0;
        int priority = thing->getStackPriority();
        for(stackPos = 0; stackPos < (int)m_things.size(); ++stackPos) {
            int otherPriority = m_things[stackPos]->getStackPriority();
            if(otherPriority > priority || (otherPriority == priority && otherPriority == 5))
                break;
        }
    } else if(stackPos > (int)m_things.size())
        stackPos = m_things.size();

    ThingPtr oldObject;
    if(stackPos < (int)m_things.size())
        oldObject = m_things[stackPos];
    m_things.insert(m_things.begin() + stackPos, thing);
    thing->setPosition(m_position);
    return oldObject;
}

ThingPtr Tile::getThing(int stackPos)
{
    if(stackPos >= 0 && stackPos < (int)m_things.size())
        return m_things[stackPos];
    return nullptr;
}

ThingPtr Tile::removeThing(int stackPos)
{
    ThingPtr oldObject;
    if(stackPos >= 0 && stackPos < (int)m_things.size()) {
        oldObject = m_things[stackPos];
        m_things.erase(m_things.begin() + stackPos);
    }
    return oldObject;
}

ThingPtr Tile::removeThing(const ThingPtr& thing)
{
    ThingPtr oldObject;
    auto it = std::find(m_things.begin(), m_things.end(), thing);
    if(it != m_things.end()) {
        oldObject = *it;
        m_things.erase(it);
    }
    return oldObject;
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
    const ThingType& type = firstObject->getType();
    if(type.isGround)
        return firstObject->asItem();
    return nullptr;
}

bool Tile::isFullyOpaque()
{
    ThingPtr firstObject = getThing(0);
    if(firstObject) {
        const ThingType& type = firstObject->getType();
        if(type.isFullGround)
            return true;
    }
    return false;
}
