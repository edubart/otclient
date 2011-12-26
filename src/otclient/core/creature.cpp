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

#include "creature.h"
#include "thingstype.h"
#include "localplayer.h"
#include "map.h"
#include "tile.h"
#include "item.h"

#include <framework/graphics/graphics.h>
#include <framework/core/eventdispatcher.h>
#include <framework/core/clock.h>

#include <framework/graphics/paintershaderprogram.h>
#include <framework/graphics/paintershadersources.h>
#include "spritemanager.h"

Creature::Creature() : Thing()
{
    m_healthPercent = 0;
    m_showSquareColor = false;
    m_direction = Otc::South;
    m_walkTimePerPixel = 1000.0/32.0;

    m_walking = false;
    m_inverseWalking = true;

    m_informationFont = g_fonts.getFont("verdana-11px-rounded");
}

PainterShaderProgramPtr outfitProgram;
int HEAD_COLOR_UNIFORM = 10;
int BODY_COLOR_UNIFORM = 11;
int LEGS_COLOR_UNIFORM = 12;
int FEET_COLOR_UNIFORM = 13;
int MASK_TEXTURE_UNIFORM = 14;

void Creature::draw(const Point& p)
{
    // TODO: activate on attack, follow, discover how 'attacked' works
    if(m_showSquareColor) {
        g_painter.setColor(Outfit::getColor(m_squareColor));
        g_painter.drawBoundingRect(Rect(p + m_walkOffset - 8, Size(32, 32)), 2);
    }

    if(!outfitProgram) {
        outfitProgram = PainterShaderProgramPtr(new PainterShaderProgram);
        outfitProgram->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
        outfitProgram->addShaderFromSourceFile(Shader::Fragment, "/outfit.frag");
        assert(outfitProgram->link());
        outfitProgram->bindUniformLocation(HEAD_COLOR_UNIFORM, "headColor");
        outfitProgram->bindUniformLocation(BODY_COLOR_UNIFORM, "bodyColor");
        outfitProgram->bindUniformLocation(LEGS_COLOR_UNIFORM, "legsColor");
        outfitProgram->bindUniformLocation(FEET_COLOR_UNIFORM, "feetColor");
        outfitProgram->bindUniformLocation(MASK_TEXTURE_UNIFORM, "maskTexture");
    }

    // Render creature
    for(m_yPattern = 0; m_yPattern < m_type->dimensions[ThingType::PatternY]; m_yPattern++) {

        // continue if we dont have this addon.
        if(m_yPattern > 0 && !(m_outfit.getAddons() & (1 << (m_yPattern-1))))
            continue;

        g_painter.setCustomProgram(outfitProgram);

        outfitProgram->bind();
        outfitProgram->setUniformValue(HEAD_COLOR_UNIFORM, m_outfit.getHeadColor());
        outfitProgram->setUniformValue(BODY_COLOR_UNIFORM, m_outfit.getBodyColor());
        outfitProgram->setUniformValue(LEGS_COLOR_UNIFORM, m_outfit.getLegsColor());
        outfitProgram->setUniformValue(FEET_COLOR_UNIFORM, m_outfit.getFeetColor());

        for(int yi = 0; yi < m_type->dimensions[ThingType::Height]; yi++) {
            for(int xi = 0; xi < m_type->dimensions[ThingType::Width]; xi++) {
                int sprIndex = ((((((m_animation % m_type->dimensions[ThingType::AnimationPhases])
                                * m_type->dimensions[ThingType::PatternZ] + m_zPattern)
                                * m_type->dimensions[ThingType::PatternY] + m_yPattern)
                                * m_type->dimensions[ThingType::PatternX] + m_xPattern)
                                * m_type->dimensions[ThingType::Layers] + 0)
                                * m_type->dimensions[ThingType::Height] + yi)
                                * m_type->dimensions[ThingType::Width] + xi;
                if(m_type->dimensions[ThingType::Layers] > 1) {
                    int maskIndex = ((((((m_animation % m_type->dimensions[ThingType::AnimationPhases])
                                    * m_type->dimensions[ThingType::PatternZ] + m_zPattern)
                                    * m_type->dimensions[ThingType::PatternY] + m_yPattern)
                                    * m_type->dimensions[ThingType::PatternX] + m_xPattern)
                                    * m_type->dimensions[ThingType::Layers] + 1)
                                    * m_type->dimensions[ThingType::Height] + yi)
                                    * m_type->dimensions[ThingType::Width] + xi;
                    int spriteId = m_type->sprites[maskIndex];
                    if(!spriteId)
                        continue;
                    TexturePtr maskTex = g_sprites.getSpriteTexture(spriteId);
                    outfitProgram->setUniformTexture(MASK_TEXTURE_UNIFORM, maskTex, 1);
                }

                int spriteId = m_type->sprites[sprIndex];
                if(!spriteId)
                    continue;

                TexturePtr spriteTex = g_sprites.getSpriteTexture(spriteId);

                Rect drawRect(((p + m_walkOffset).x - xi*32) - m_type->parameters[ThingType::DisplacementX],
                            ((p + m_walkOffset).y - yi*32) - m_type->parameters[ThingType::DisplacementY],
                            32, 32);
                g_painter.drawTexturedRect(drawRect, spriteTex);
            }
        }

        g_painter.releaseCustomProgram();
    }
}

void Creature::drawInformation(int x, int y, bool useGray, const Rect& rect)
{
    Color fillColor = Color(96, 96, 96);

    if(!useGray)
        fillColor = m_informationColor;

    // calculate main rects
    Rect backgroundRect = Rect(x-(13.5), y, 27, 4);
    backgroundRect.bound(rect);

    Rect textRect = Rect(x - m_nameSize.width() / 2.0, y-15, m_nameSize);
    textRect.bound(rect);

    // distance them
    if(textRect.top() == rect.top())
        backgroundRect.moveTop(textRect.top() + 15);
    if(backgroundRect.bottom() == rect.bottom())
        textRect.moveTop(backgroundRect.top() - 15);

    // health rect is based on background rect, so no worries
    Rect healthRect = backgroundRect.expanded(-1);
    healthRect.setWidth((m_healthPercent / 100.0) * 25);

    // draw
    g_painter.setColor(Fw::black);
    g_painter.drawFilledRect(backgroundRect);

    g_painter.setColor(fillColor);
    g_painter.drawFilledRect(healthRect);

    m_informationFont->renderText(m_name, textRect, Fw::AlignTopCenter, fillColor);
}

void Creature::walk(const Position& position, bool inverse)
{
    // set walking state
    bool sameWalk = m_walking && !m_inverseWalking && inverse;
    m_walking = true;
    m_inverseWalking = inverse;
    int walkTimeFactor = 1;

    // set new direction
    if(m_position.isInRange(position, 1, 1, 0)) {
        Otc::Direction direction = m_position.getDirectionFromPosition(position);
        setDirection(direction);

        if(direction == Otc::NorthWest) {
            m_walkOffset.x = 32;
            m_walkOffset.y = 32;
            walkTimeFactor = 2;
        }
        else if(direction == Otc::North) {
            m_walkOffset.y = 32;
        }
        else if(direction == Otc::NorthEast) {
            m_walkOffset.x = -32;
            m_walkOffset.y = 32;
            walkTimeFactor = 2;
        }
        else if(direction == Otc::East) {
            m_walkOffset.x = -32;
        }
        else if(direction == Otc::SouthEast) {
            m_walkOffset.x = -32;
            m_walkOffset.y = -32;
            walkTimeFactor = 2;
        }
        else if(direction == Otc::South) {
            m_walkOffset.y = -32;
        }
        else if(direction == Otc::SouthWest) {
            m_walkOffset.x = 32;
            m_walkOffset.y = -32;
            walkTimeFactor = 2;
        }
        else if(direction == Otc::West) {
            m_walkOffset.x = 32;
        }
    }
    else { // Teleport
        // we teleported, dont walk or change direction
        m_walking = false;
        m_walkOffset.x = 0;
        m_walkOffset.y = 0;
        m_animation = 0;
    }

    if(!m_inverseWalking) {
        m_walkOffset.x = 0;
        m_walkOffset.y = 0;
    }

    if(m_walking) {
        // get walk speed
        int groundSpeed = 100;

        ItemPtr ground = g_map.getTile(position)->getGround();
        if(ground)
            groundSpeed = ground->getType()->parameters[ThingType::GroundSpeed];

        float walkTime = walkTimeFactor * 1000.0 * (float)groundSpeed / m_speed;
        walkTime = (walkTime == 0) ? 1000 : walkTime;

        m_walkTimePerPixel = walkTime / 32.0;
        if(!sameWalk)
            m_walkStartTicks = g_clock.ticks();
        updateWalk();
    }
}

void Creature::updateWalk()
{
    if(m_walking) {
        int elapsedTicks = g_clock.ticksElapsed(m_walkStartTicks);
        int totalPixelsWalked = std::min((int)round(elapsedTicks / m_walkTimePerPixel), 32);

        if(m_inverseWalking) {
            if(m_direction == Otc::North || m_direction == Otc::NorthEast || m_direction == Otc::NorthWest)
                m_walkOffset.y = 32 - totalPixelsWalked;
            else if(m_direction == Otc::South || m_direction == Otc::SouthEast || m_direction == Otc::SouthWest)
                m_walkOffset.y = totalPixelsWalked - 32;

            if(m_direction == Otc::East || m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
                m_walkOffset.x = totalPixelsWalked - 32;
            else if(m_direction == Otc::West || m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
                m_walkOffset.x = 32 - totalPixelsWalked;
        }
        else {
            if(m_direction == Otc::North || m_direction == Otc::NorthEast || m_direction == Otc::NorthWest)
                m_walkOffset.y = -totalPixelsWalked;
            else if(m_direction == Otc::South || m_direction == Otc::SouthEast || m_direction == Otc::SouthWest)
                m_walkOffset.y = totalPixelsWalked;

            if(m_direction == Otc::East || m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
                m_walkOffset.x = totalPixelsWalked;
            else if(m_direction == Otc::West || m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
                m_walkOffset.x = -totalPixelsWalked;
        }

        int totalWalkTileTicks = (int)m_walkTimePerPixel*32 * 0.5;
        if(m_type->dimensions[ThingType::AnimationPhases] > 0)
            m_animation = (g_clock.ticks() % totalWalkTileTicks) / (totalWalkTileTicks / (m_type->dimensions[ThingType::AnimationPhases] - 1)) + 1;
        else
            m_animation = 0;


        if(totalPixelsWalked == 32)
            cancelWalk(m_direction);
        else
            g_dispatcher.scheduleEvent(std::bind(&Creature::updateWalk, asCreature()), m_walkTimePerPixel);
    }
}

void Creature::cancelWalk(Otc::Direction direction)
{
    if(m_walking) {
        auto self = asCreature();
        g_dispatcher.scheduleEvent([=]() {
            if(!self->m_walking)
                self->m_animation = 0;
        }, m_walkTimePerPixel * 2);
    }

    m_walking = false;
    m_walkStartTicks = 0;
    m_walkOffset.x = 0;
    m_walkOffset.y = 0;
    m_direction = direction;
}

void Creature::setName(const std::string& name)
{
    m_nameSize = m_informationFont->calculateTextRectSize(name);
    m_name = name;
}

void Creature::setHealthPercent(uint8 healthPercent)
{
    m_informationColor = Fw::black;

    if(healthPercent > 92) {
        m_informationColor.setGreen(188);
    }
    else if(healthPercent > 60) {
        m_informationColor.setRed(80);
        m_informationColor.setGreen(161);
        m_informationColor.setBlue(80);
    }
    else if(healthPercent > 30) {
        m_informationColor.setRed(161);
        m_informationColor.setGreen(161);
    }
    else if(healthPercent > 8) {
        m_informationColor.setRed(160);
        m_informationColor.setGreen(39);
        m_informationColor.setBlue(39);
    }
    else if(healthPercent > 3) {
        m_informationColor.setRed(160);
    }
    else {
        m_informationColor.setRed(79);
    }

    m_healthPercent = healthPercent;
}

void Creature::setDirection(Otc::Direction direction)
{
    if(direction == Otc::NorthEast || direction == Otc::SouthEast)
        m_xPattern = Otc::East;
    else if(direction == Otc::NorthWest || direction == Otc::SouthWest)
        m_xPattern = Otc::West;
    else
        m_xPattern = direction;

    m_direction = direction;
}

void Creature::setOutfit(const Outfit& outfit)
{
    m_outfit = outfit;
    m_type = getType();
}

ThingType *Creature::getType()
{
    return g_thingsType.getThingType(m_outfit.getType(), ThingsType::Creature);
}
