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

#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>
#include <framework/graphics/graphics.h>
#include <framework/core/filestream.h>
#include <framework/core/binarytree.h>

Item::Item() :
    m_id(0),
    m_countOrSubType(1),
    m_actionId(0),
    m_uniqueId(0),
    m_shaderProgram(g_shaders.getDefaultItemShader()),
    m_otbType(g_things.getNullOtbType())
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
    } else if(isFluid() || isFluidContainer()) {
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

    bool useShader = g_painter->hasShaders() && m_shaderProgram;
    if(useShader) {
        m_shaderProgram->bind();
        m_shaderProgram->setUniformValue(ShaderManager::ITEM_ID_UNIFORM, (int)m_id);

        g_painter->setShaderProgram(m_shaderProgram);
    }

    m_datType->draw(dest, scaleFactor, 0, xPattern, yPattern, zPattern, animationPhase);

    if(useShader)
        g_painter->resetShaderProgram();
}

void Item::setId(uint32 id)
{
    m_datType = g_things.getDatType(id, DatItemCategory);
    m_otbType = g_things.findOtbForClientId(id);
    m_id = m_datType->getId();
}

void Item::setOtbId(uint16 id)
{
    m_otbType = g_things.getOtbType(id);
    m_datType = g_things.getDatType(m_otbType->getClientId(), DatItemCategory);
    m_id = m_datType->getId();
}

bool Item::isValid()
{
    return g_things.isValidDatId(m_id, DatItemCategory);
}

void Item::unserializeItem(const BinaryTreePtr &in)
{
    while (in->canRead()) {
        uint8 attrType = in->getU8();
        if(attrType == 0)
            break;

        // fugly switch yes?
        switch ((AttrTypes_t)attrType) {
            case ATTR_COUNT:
                setSubType(in->getU8());
                break;
            case ATTR_CHARGES:
                setSubType(in->getU16());
                break;
            case ATTR_ACTION_ID:
                setActionId(in->getU16());
                break;
            case ATTR_UNIQUE_ID:
                setUniqueId(in->getU16());
                break;
            case ATTR_NAME:
                setName(in->getString());
                break;
            case ATTR_TEXT:
                setText(in->getString());
                break;
            case ATTR_DESC:
                m_description = in->getString();
                break;
            case ATTR_CONTAINER_ITEMS:
                m_isContainer = true;
                in->skip(4);
                break;
            case ATTR_HOUSEDOORID:
                m_isDoor = true;
                m_doorId = in->getU8();
                break;
            case ATTR_DEPOT_ID:
                m_depotId = in->getU16();
                break;
            case ATTR_TELE_DEST: {
                m_teleportDestination.x = in->getU16();
                m_teleportDestination.y = in->getU16();
                m_teleportDestination.z = in->getU8();
                break;
            }
            case ATTR_ARTICLE:
            case ATTR_WRITTENBY:
                in->getString();
                break;
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
                in->skip(4);
                break;
            case ATTR_SCRIPTPROTECTED:
            case ATTR_DUALWIELD:
            case ATTR_DECAYING_STATE:
            case ATTR_RUNE_CHARGES:
                in->skip(1);
                break;
            default:
                stdext::throw_exception(stdext::format("invalid item attribute %d", (int)attrType));
        }
    }
}

bool Item::isMoveable()
{
    if(m_datType)
        return !m_datType->isNotMoveable();

    g_logger.warning(stdext::format(
                         "Invalid dat type for item %d", m_id));
    return false;
}
