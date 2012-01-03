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
#include "thingstype.h"
#include "map.h"
#include "game.h"
#include "localplayer.h"
#include "effect.h"
#include <otclient/net/protocolgame.h>
#include <framework/graphics/fontmanager.h>

Tile::Tile(const Position& position)
{
    m_drawElevation = 0;
    m_position = position;
}

void Tile::draw(const Point& p)
{
    m_drawElevation = 0;

    // first bottom items
    for(const ThingPtr& thing : m_things) {
        ThingType *type = thing->getType();
        if(!type->properties[ThingType::IsGround] && !type->properties[ThingType::IsGroundBorder] && !type->properties[ThingType::IsOnBottom])
            break;
        thing->draw(p - m_drawElevation);
        m_drawElevation += type->parameters[ThingType::Elevation];
        if(m_drawElevation > MAX_DRAW_ELEVATION)
            m_drawElevation = MAX_DRAW_ELEVATION;
    }

    // now common items
    for(auto it = m_things.rbegin(); it != m_things.rend(); ++it) {
        const ThingPtr& thing = *it;
        ThingType *type = thing->getType();
        if(thing->asCreature() || type->properties[ThingType::IsOnTop] || type->properties[ThingType::IsOnBottom] || type->properties[ThingType::IsGroundBorder] || type->properties[ThingType::IsGround])
            break;
        thing->draw(p - m_drawElevation);
        m_drawElevation += type->parameters[ThingType::Elevation];
        if(m_drawElevation > MAX_DRAW_ELEVATION)
            m_drawElevation = MAX_DRAW_ELEVATION;
    }

    // we can render creatures in 3x3 range
    //TODO: this algorithm is slowing down render too much, but it could be cached to improve framerate
    for(int xi = -1; xi <= 1; ++xi) {
        for(int yi = -1; yi <= 1; ++yi) {
            for(CreaturePtr creature : g_map.getTile(m_position + Position(xi, yi, 0))->getCreatures()) {
                ThingType *type = creature->getType();
                Rect creatureRect(p.x + xi*32 + creature->getWalkOffset().x - type->parameters[ThingType::DisplacementX], p.y + yi*32 + creature->getWalkOffset().y - type->parameters[ThingType::DisplacementY], 32, 32);
                Rect thisTileRect(p.x, p.y, 32, 32);

                // only render creatures where bottom right is inside our rect
                if(thisTileRect.contains(creatureRect.bottomRight())) {
                    creature->draw(Point(p.x + xi*32 - m_drawElevation, p.y + yi*32 - m_drawElevation));
                }
            }
        }
    }

    // effects
    for(const EffectPtr& effect : m_effects)
        effect->draw(p - m_drawElevation);

    // top items
    for(const ThingPtr& thing : m_things) {
        ThingType *type = thing->getType();
        if(type->properties[ThingType::IsOnTop])
            thing->draw(p);
    }
}

void Tile::clean()
{
    m_things.clear();
    m_effects.clear();
}

ThingPtr Tile::addThing(const ThingPtr& thing, int stackPos)
{
    if(!thing)
        return nullptr;

    if(EffectPtr effect = thing->asEffect()) {
        m_effects.push_back(effect);
        return nullptr;
    }

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
    return oldObject;
}

ThingPtr Tile::getThing(int stackPos)
{
    if(stackPos >= 0 && stackPos < (int)m_things.size())
        return m_things[stackPos];
    return nullptr;
}

ThingPtr Tile::getTopThing()
{
    if(isEmpty())
        return nullptr;

    return m_things[m_things.size() - 1];
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
    if(EffectPtr effect = thing->asEffect()) {
        auto it = std::find(m_effects.begin(), m_effects.end(), effect);
        if(it != m_effects.end())
            m_effects.erase(it);
        return thing;
    }
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
    ThingType *type = firstObject->getType();
    if(type->properties[ThingType::IsGround])
        return firstObject->asItem();
    return nullptr;
}

int Tile::getLookStackpos()
{
    for(int i = m_things.size() - 1; i >= 0; --i) {
        ThingType *type = m_things[i]->getType();
        if(!type->properties[ThingType::IgnoreLook] &&
           (type->properties[ThingType::IsGround] || type->properties[ThingType::IsGroundBorder] || type->properties[ThingType::IsOnBottom] || type->properties[ThingType::IsOnTop]))
            return i;
    }
    return -1;
}

bool Tile::isWalkable()
{
    if(!getGround())
        return false;

    for(const ThingPtr& thing : m_things) {
        ThingType *type = thing->getType();
        if(type->properties[ThingType::NotWalkable])
            return false;

        if(CreaturePtr creature = thing->asCreature()) {
            if(!creature->getPassable())
                return false;
        }
    }
    return true;
}

bool Tile::isFullGround()
{
    ThingPtr ground = getThing(0);
    if(!ground)
        return false;
    ThingType *type = ground->getType();
    if(type->properties[ThingType::IsGround] && type->properties[ThingType::IsFullGround])
        return true;
    return false;
}

bool Tile::isFullyOpaque()
{
    ThingPtr firstObject = getThing(0);
    if(firstObject) {
        ThingType *type = firstObject->getType();
        if(type->properties[ThingType::IsFullGround])
            return true;
    }
    return false;
}

bool Tile::isLookPossible()
{
    for(const ThingPtr& thing : m_things) {
        ThingType *type = thing->getType();
        if(type->properties[ThingType::BlockProjectile])
            return false;
    }
    return true;
}

bool Tile::hasCreature()
{
    for(const ThingPtr& thing : m_things)
        if(thing->asCreature())
            return true;
    return false;
}

bool Tile::isEmpty()
{
    return m_things.size() == 0;
}

/*bool Tile::canAttack()
{
    return hasCreature();
}

bool Tile::canFollow()
{
    return hasCreature();
}

bool Tile::canCopyName()
{
    return hasCreature();
}*/

// TODO:
/*

  Get menu options


  if creature:
    Look
    -----
    Attack
    Follow
    -----
    Copy Name

  if item:
    Look
    Use (if not container)
    Open (if container)
    Use with ... (if multiuse?)
    Rotate (if rotable)
    -----
    Trade with ... (if pickupable?)

  if player:
    Look
    -----
    Attack
    Follow
    -----
    Message to NAME
    Add to VIP list
    Ignore NAME
    Invite to Party
    -----
    Report Offense
    -----
    Copy Name

  if localplayer:
    Look
    -----
    Set Outfit
    -----
    Copy Name
*/

void Tile::useItem()
{
    // Get top item of stack priority 2 (do a function to do this later)
    ThingPtr thing;
    int lastStackpos = -1;
    for(int stackPos = 0; stackPos < (int)m_things.size(); ++stackPos) {
        int otherPriority = m_things[stackPos]->getStackPriority();
        if(otherPriority == 2) {
            thing = m_things[stackPos];
            lastStackpos = stackPos;
        }
    }

    if(lastStackpos != -1) {
        // use this
        g_game.getProtocolGame()->sendUseItem(m_position, thing->getId(), lastStackpos, 0); // 0 has something to do with container
    }
}
