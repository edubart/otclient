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

#include "item.h"
#include "thingtypemanager.h"
#include "spritemanager.h"
#include "thing.h"
#include "tile.h"
#include "shadermanager.h"
#include "container.h"
#include "map.h"
#include "houses.h"
#include "game.h"

#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>
#include <framework/graphics/graphics.h>
#include <framework/core/filestream.h>
#include <framework/core/binarytree.h>

Item::Item() :
    m_id(0),
    m_otbId(0),
    m_countOrSubType(1)
{
}

ItemPtr Item::create(int id)
{
    ItemPtr item(new Item);
    item->setId(id);
    return item;
}

ItemPtr Item::createFromOtb(int id)
{
    ItemPtr item(new Item);
    item->setOtbId(id);
    return item;
}

void Item::draw(const Point& dest, float scaleFactor, bool animate)
{
    if(m_id == 0)
        return;

    // determine animation phase
    int animationPhase = 0;
    if(getAnimationPhases() > 1) {
        if(animate)
            animationPhase = (g_clock.millis() % (Otc::ITEM_TICKS_PER_FRAME * getAnimationPhases())) / Otc::ITEM_TICKS_PER_FRAME;
        else
            animationPhase = getAnimationPhases()-1;
    }

    // determine x,y,z patterns
    int xPattern = 0, yPattern = 0, zPattern = 0;
    if(isStackable() && getNumPatternX() == 4 && getNumPatternY() == 2) {
        if(m_countOrSubType <= 0) {
            xPattern = 0;
            yPattern = 0;
        } else if(m_countOrSubType < 5) {
            xPattern = m_countOrSubType-1;
            yPattern = 0;
        } else if(m_countOrSubType < 10) {
            xPattern = 0;
            yPattern = 1;
        } else if(m_countOrSubType < 25) {
            xPattern = 1;
            yPattern = 1;
        } else if(m_countOrSubType < 50) {
            xPattern = 2;
            yPattern = 1;
        } else {
            xPattern = 3;
            yPattern = 1;
        }
    } else if(isHangable()) {
        const TilePtr& tile = getTile();
        if(tile) {
            if(tile->mustHookSouth())
                xPattern = getNumPatternX() >= 2 ? 1 : 0;
            else if(tile->mustHookEast())
                xPattern = getNumPatternX() >= 3 ? 2 : 0;
        }
    } else if(isSplash() || isFluidContainer()) {
        int color = Otc::FluidTransparent;
        switch(m_countOrSubType) {
            case Otc::FluidNone:
                color = Otc::FluidTransparent;
                break;
            case Otc::FluidWater:
                color = Otc::FluidBlue;
                break;
            case Otc::FluidMana:
                color = Otc::FluidPurple;
                break;
            case Otc::FluidBeer:
                color = Otc::FluidBrown;
                break;
            case Otc::FluidOil:
                color = Otc::FluidBrown;
                break;
            case Otc::FluidBlood:
                color = Otc::FluidRed;
                break;
            case Otc::FluidSlime:
                color = Otc::FluidGreen;
                break;
            case Otc::FluidMud:
                color = Otc::FluidBrown;
                break;
            case Otc::FluidLemonade:
                color = Otc::FluidYellow;
                break;
            case Otc::FluidMilk:
                color = Otc::FluidWhite;
                break;
            case Otc::FluidWine:
                color = Otc::FluidPurple;
                break;
            case Otc::FluidHealth:
                color = Otc::FluidRed;
                break;
            case Otc::FluidUrine:
                color = Otc::FluidYellow;
                break;
            case Otc::FluidRum:
                color = Otc::FluidBrown;
                break;
            case Otc::FluidFruidJuice:
                color = Otc::FluidYellow;
                break;
            case Otc::FluidCoconutMilk:
                color = Otc::FluidWhite;
                break;
            case Otc::FluidTea:
                color = Otc::FluidBrown;
                break;
            case Otc::FluidMead:
                color = Otc::FluidBrown;
                break;
            default:
                color = Otc::FluidTransparent;
                break;
        }

        xPattern = (color % 4) % getNumPatternX();
        yPattern = (color / 4) % getNumPatternY();
    } else if(isGround() || isOnBottom()) {
        xPattern = m_position.x % getNumPatternX();
        yPattern = m_position.y % getNumPatternY();
        zPattern = m_position.z % getNumPatternZ();
    }

    rawGetThingType()->draw(dest, scaleFactor, 0, xPattern, yPattern, zPattern, animationPhase);
}

void Item::setId(uint32 id)
{
    if(!g_things.isValidDatId(id, ThingCategoryItem))
        id = 0;
    //m_otbId = g_things.findItemTypeByClientId(id)->getServerId();
    m_id = id;
    m_otbId = 0;
}

void Item::setOtbId(uint16 id)
{
    if(!g_things.isValidOtbId(id))
        id = 0;
    auto itemType = g_things.getItemType(id);
    m_otbId = id;

    id = itemType->getClientId();
    if(!g_things.isValidDatId(id, ThingCategoryItem))
        id = 0;
    m_id = id;
}

bool Item::isValid()
{
    return g_things.isValidDatId(m_id, ThingCategoryItem);
}

void Item::unserializeItem(const BinaryTreePtr &in)
{
    try {
        while(in->canRead()) {
            int attrib = in->getU8();
            if(attrib == 0)
                break;

            switch(attrib) {
                case ATTR_COUNT:
                case ATTR_RUNE_CHARGES:
                    setCount(in->getU8());
                    break;
                case ATTR_CHARGES:
                    setCount(in->getU16());
                    break;
                case ATTR_HOUSEDOORID:
                case ATTR_SCRIPTPROTECTED:
                case ATTR_DUALWIELD:
                case ATTR_DECAYING_STATE:
                    m_attribs.set(attrib, in->getU8());
                    break;
                case ATTR_ACTION_ID:
                case ATTR_UNIQUE_ID:
                case ATTR_DEPOT_ID:
                    m_attribs.set(attrib, in->getU16());
                    break;
                case ATTR_CONTAINER_ITEMS:
                case ATTR_ATTACK:
                case ATTR_EXTRAATTACK:
                case ATTR_DEFENSE:
                case ATTR_EXTRADEFENSE:
                case ATTR_ARMOR:
                case ATTR_ATTACKSPEED:
                case ATTR_HITCHANCE:
                case ATTR_DURATION:
                case ATTR_WRITTENDATE:
                case ATTR_SLEEPERGUID:
                case ATTR_SLEEPSTART:
                case ATTR_ATTRIBUTE_MAP:
                    m_attribs.set(attrib, in->getU32());
                    break;
                case ATTR_TELE_DEST: {
                    Position pos;
                    pos.x = in->getU16();
                    pos.y = in->getU16();
                    pos.z = in->getU8();
                    m_attribs.set(attrib, pos);
                    break;
                }
                case ATTR_NAME:
                case ATTR_TEXT:
                case ATTR_DESC:
                case ATTR_ARTICLE:
                case ATTR_WRITTENBY:
                    m_attribs.set(attrib, in->getString());
                    break;
                default:
                    stdext::throw_exception(stdext::format("invalid item attribute %d", attrib));
            }
        }
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("Failed to unserialize OTBM item: %s", e.what()));
    }
}

void Item::serializeItem(const BinaryWriteTreePtr& out)
{
    out->writeU8(ATTR_COUNT);
    out->writeU8(getCount());

    out->writeU8(ATTR_CHARGES);
    out->writeU16(getCountOrSubType());

    Position dest = m_attribs.get<Position>(ATTR_TELE_DEST);
    if(dest.isValid()) {
        out->writeU8(ATTR_TELE_DEST);
        out->writePos(dest);
    }

    if(isDepot()) {
        out->writeU8(ATTR_DEPOT_ID);
        out->writeU16(getDepotId());
    }

    uint16 aid = m_attribs.get<uint16>(ATTR_ACTION_ID);
    uint16 uid = m_attribs.get<uint16>(ATTR_UNIQUE_ID);
    if(aid) {
        out->writeU8(ATTR_ACTION_ID);
        out->writeU16(aid);
    }

    if(uid) {
        out->writeU8(ATTR_UNIQUE_ID);
        out->writeU16(uid);
    }
}

int Item::getSubType()
{
    if(isSplash() || isFluidContainer())
        return m_countOrSubType;
    if(g_game.getClientVersion() >= 900)
        return 0;
    return 1;
}

int Item::getCount()
{
    if(isStackable())
        return m_countOrSubType;
    return 1;
}

bool Item::isMoveable()
{
    return !rawGetThingType()->isNotMoveable();
}

ItemPtr Item::clone()
{
    ItemPtr item = ItemPtr(new Item);
    *(item.get()) = *this;
    return item;
}

const ThingTypePtr& Item::getThingType()
{
    return g_things.getThingType(m_id, ThingCategoryItem);
}

ThingType* Item::rawGetThingType()
{
    return g_things.rawGetThingType(m_id, ThingCategoryItem);
}
