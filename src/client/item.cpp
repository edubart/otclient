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
    m_clientId(0),
    m_serverId(0),
    m_countOrSubType(1),
    m_color(Color::alpha),
    m_async(true),
    m_phase(0),
    m_lastPhase(0)
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

std::string Item::getName()
{
    return g_things.findItemTypeByClientId(m_clientId)->getName();
}

void Item::draw(const Point& dest, float scaleFactor, bool animate, LightView *lightView)
{
    if(m_clientId == 0)
        return;

    // determine animation phase
    int animationPhase = calculateAnimationPhase(animate);

    // determine x,y,z patterns
    int xPattern = 0, yPattern = 0, zPattern = 0;
    calculatePatterns(xPattern, yPattern, zPattern);

    if(m_color != Color::alpha)
        g_painter->setColor(m_color);
    rawGetThingType()->draw(dest, scaleFactor, 0, xPattern, yPattern, zPattern, animationPhase, lightView);

    /// Sanity check
    /// This is just to ensure that we don't overwrite some color and
    /// screw up the whole rendering.
    if(m_color != Color::alpha)
        g_painter->resetColor();
}

void Item::setId(uint32 id)
{
    if(!g_things.isValidDatId(id, ThingCategoryItem))
        id = 0;
    m_serverId = g_things.findItemTypeByClientId(id)->getServerId();
    m_clientId = id;
}

void Item::setOtbId(uint16 id)
{
    if(!g_things.isValidOtbId(id))
        id = 0;
    auto itemType = g_things.getItemType(id);
    m_serverId = id;

    id = itemType->getClientId();
    if(!g_things.isValidDatId(id, ThingCategoryItem))
        id = 0;
    m_clientId = id;
}

bool Item::isValid()
{
    return g_things.isValidDatId(m_clientId, ThingCategoryItem);
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

void Item::serializeItem(const OutputBinaryTreePtr& out)
{
    out->startNode(OTBM_ITEM);
    out->addU16(getServerId());

    out->addU8(ATTR_COUNT);
    out->addU8(getCount());

    out->addU8(ATTR_CHARGES);
    out->addU16(getCountOrSubType());

    Position dest = m_attribs.get<Position>(ATTR_TELE_DEST);
    if(dest.isValid()) {
        out->addU8(ATTR_TELE_DEST);
        out->addPos(dest.x, dest.y, dest.z);
    }

    if(isDepot()) {
        out->addU8(ATTR_DEPOT_ID);
        out->addU16(getDepotId());
    }

    if(isHouseDoor()) {
        out->addU8(ATTR_HOUSEDOORID);
        out->addU8(getDoorId());
    }

    uint16 aid = m_attribs.get<uint16>(ATTR_ACTION_ID);
    uint16 uid = m_attribs.get<uint16>(ATTR_UNIQUE_ID);
    if(aid) {
        out->addU8(ATTR_ACTION_ID);
        out->addU16(aid);
    }

    if(uid) {
        out->addU8(ATTR_UNIQUE_ID);
        out->addU16(uid);
    }

    std::string text = getText();
    if(g_things.getItemType(m_serverId)->isWritable() && !text.empty()) {
        out->addU8(ATTR_TEXT);
        out->addString(text);
    }
    std::string desc = getDescription();
    if(!desc.empty()) {
        out->addU8(ATTR_DESC);
        out->addString(desc);
    }

    out->endNode();
    for(auto i : m_containerItems)
        i->serializeItem(out);
}

int Item::getSubType()
{
    if(isSplash() || isFluidContainer())
        return m_countOrSubType;
    if(g_game.getClientVersion() > 862)
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

bool Item::isGround()
{
    return rawGetThingType()->isGround();
}

ItemPtr Item::clone()
{
    ItemPtr item = ItemPtr(new Item);
    *(item.get()) = *this;
    return item;
}

void Item::calculatePatterns(int& xPattern, int& yPattern, int& zPattern)
{
    // Avoid crashes with invalid items
    if(!isValid())
        return;

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
        if(g_game.getFeature(Otc::GameNewFluids)) {
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
        } else
            color = m_countOrSubType;

        xPattern = (color % 4) % getNumPatternX();
        yPattern = (color / 4) % getNumPatternY();
    } else {
        xPattern = m_position.x % getNumPatternX();
        yPattern = m_position.y % getNumPatternY();
        zPattern = m_position.z % getNumPatternZ();
    }
}

int Item::calculateAnimationPhase(bool animate)
{
    if(getAnimationPhases() > 1) {
        if(animate) {
            if(getAnimator() != nullptr)
                return getAnimator()->getPhase();

            if(m_async)
                return (g_clock.millis() % (Otc::ITEM_TICKS_PER_FRAME * getAnimationPhases())) / Otc::ITEM_TICKS_PER_FRAME;
            else {
                if(g_clock.millis() - m_lastPhase >= Otc::ITEM_TICKS_PER_FRAME) {
                    m_phase = (m_phase + 1) % getAnimationPhases();
                    m_lastPhase = g_clock.millis();
                }
                return m_phase;
            }
        } else
            return getAnimationPhases()-1;
    }
    return 0;
}

int Item::getExactSize(int layer, int xPattern, int yPattern, int zPattern, int animationPhase)
{
    calculatePatterns(xPattern, yPattern, zPattern);
    animationPhase = calculateAnimationPhase(true);
    return Thing::getExactSize(layer, xPattern, yPattern, zPattern, animationPhase);
}

const ThingTypePtr& Item::getThingType()
{
    return g_things.getThingType(m_clientId, ThingCategoryItem);
}

ThingType* Item::rawGetThingType()
{
    return g_things.rawGetThingType(m_clientId, ThingCategoryItem);
}
/* vim: set ts=4 sw=4 et :*/
