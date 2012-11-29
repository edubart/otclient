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

#include "lightview.h"
#include "mapview.h"
#include <framework/graphics/framebuffer.h>
#include <framework/graphics/framebuffermanager.h>
#include <framework/graphics/painter.h>
#include <framework/graphics/image.h>

LightView::LightView()
{
    m_lightbuffer = g_framebuffers.createFrameBuffer();
    generateLightBuble();
    reset();
}

void LightView::generateLightBuble()
{
    m_lightRadius = 128;
    int circleDiameter = m_lightRadius * 2;
    ImagePtr lightImage = ImagePtr(new Image(Size(circleDiameter, circleDiameter)));

    for(int x = 0; x < circleDiameter; x++) {
        for(int y = 0; y < circleDiameter; y++) {
            float radius = std::sqrt((m_lightRadius - x)*(m_lightRadius - x) + (m_lightRadius - y)*(m_lightRadius - y));
            float intensity = std::max((m_lightRadius-radius)/(float)m_lightRadius, 0.0f);

            // light intensity varies inversely with the square of the distance 
            intensity = intensity * intensity;
            uint8_t colorByte = intensity * 255;

            uint8_t pixel[4] = {colorByte,colorByte,colorByte,255};
            lightImage->setPixel(x, y, pixel);
        }
    }

    m_lightTexture = TexturePtr(new Texture(lightImage, true));
    m_lightTexture->setSmooth(true);
}

void LightView::reset()
{
    m_lightMap.clear();
}

void LightView::setGlobalLight(const Light& light)
{
    m_globalLight = light;
}

void LightView::addLightSource(const Point& center, float scaleFactor, const Light& light)
{
    int radius = light.intensity * m_lightRadius * scaleFactor * 0.25f;

    Color color = Color::from8bit(light.color);
    float brightness = 0.8f + (light.intensity/8.0f)*0.2f;

    color.setRed(color.rF() * brightness);
    color.setGreen(color.gF() * brightness);
    color.setBlue(color.bF() * brightness);

    LightSource source;
    source.center = center;
    source.color = color;
    source.radius = radius;
    m_lightMap.push_back(source);
}

void LightView::drawGlobalLight(const Light& light)
{
    Color color = Color::from8bit(light.color);
    float brightness = light.intensity / 256.0f;
    color.setRed(color.rF() * brightness);
    color.setGreen(color.gF() * brightness);
    color.setBlue(color.bF() * brightness);
    g_painter->setColor(color);
    g_painter->drawFilledRect(Rect(0,0,m_lightbuffer->getSize()));
}

void LightView::drawLightSource(const Point& center, const Color& color, int radius)
{
    Rect dest = Rect(center - Point(radius, radius), Size(radius*2,radius*2));
    g_painter->setColor(color);
    g_painter->setCompositionMode(Painter::CompositionMode_Add);
    g_painter->drawTexturedRect(dest, m_lightTexture);

    // debug draw
    /*
    radius = 8;
    g_painter->setColor(Color::black);
    g_painter->setCompositionMode(Painter::CompositionMode_Replace);
    g_painter->drawBoundingRect(Rect(center - Point(radius, radius), Size(radius*2,radius*2)), 4);
    */
}

void LightView::resize(const Size& size)
{
    m_lightbuffer->resize(size);
}

void LightView::draw(const Rect& dest, const Rect& src)
{
    g_painter->saveAndResetState();
    m_lightbuffer->bind();
    g_painter->setCompositionMode(Painter::CompositionMode_Replace);
    drawGlobalLight(m_globalLight);
    g_painter->setCompositionMode(Painter::CompositionMode_Add);
    for(const LightSource& source : m_lightMap)
        drawLightSource(source.center, source.color, source.radius);
    m_lightbuffer->release();

    g_painter->setCompositionMode(Painter::CompositionMode_Light);
    m_lightbuffer->draw(dest, src);
    g_painter->restoreSavedState();
}
