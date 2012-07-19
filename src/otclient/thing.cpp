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

#include "thing.h"
#include "spritemanager.h"
#include "thingtypemanager.h"
#include <framework/graphics/graphics.h>
#include "map.h"
#include "tile.h"
#include "game.h"

Thing::Thing() :
    m_datId(0)
{
}

int Thing::getStackPriority()
{
    if(isGround())
        return 0;
    else if(isGroundBorder())
        return 1;
    else if(isOnBottom())
        return 2;
    else if(isOnTop())
        return 3;
    else if(asCreature())
        return 4;
    else // common items
        return 5;
}

const TilePtr& Thing::getTile()
{
    return g_map.getTile(m_position);
}

ContainerPtr Thing::getParentContainer()
{
    if(m_position.x == 0xffff && m_position.y & 0x40) {
        int containerId = m_position.y ^ 0x40;
        return g_game.getContainer(containerId);
    }
    return nullptr;
}

int Thing::getStackpos()
{
    if(m_position.x == 65535 && asItem()) // is inside a container
        return 0;
    else if(const TilePtr& tile = getTile())
        return tile->getThingStackpos(asThing());
    else {
        g_logger.traceError("got a thing with invalid stackpos");
        return -1;
    }
}

const ThingTypePtr& Thing::getThingType()
{
    return g_things.getNullThingType();
}

ThingType* Thing::rawGetThingType()
{
    return g_things.getNullThingType().get();
}
