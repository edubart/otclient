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

#include "lightview.h"
#include "mapview.h"
#include <framework/graphics/framebuffer.h>
#include <framework/graphics/framebuffermanager.h>
#include <framework/graphics/painter.h>
#include <framework/graphics/image.h>

enum {
    MAX_LIGHT_INTENSITY = 8,
    MAX_AMBIENT_LIGHT_INTENSITY = 255
};

LightView::LightView()
{
    m_lightbuffer = g_framebuffers.createFrameBuffer();
    m_lightTexture = generateLightBubble(0.1f);
    m_blendEquation = Painter::BlendEquation_Add;
    reset();
}

TexturePtr LightView::generateLightBubble(float centerFactor)
{
    int bubbleRadius = 256;
    int centerRadius = bubbleRadius * centerFactor;
    int bubbleDiameter = bubbleRadius * 2;
    ImagePtr lightImage = ImagePtr(new Image(Size(bubbleDiameter, bubbleDiameter)));

    for(int x = 0; x < bubbleDiameter; x++) {
        for(int y = 0; y < bubbleDiameter; y++) {
            float radius = std::sqrt((bubbleRadius - x)*(bubbleRadius - x) + (bubbleRadius - y)*(bubbleRadius - y));
            float intensity = stdext::clamp<float>((bubbleRadius - radius) / (float)(bubbleRadius - centerRadius), 0.0f, 1.0f);

            // light intensity varies inversely with the square of the distance
            intensity = intensity * intensity;
            uint8_t colorByte = intensity * 0xff;

            uint8_t pixel[4] = {colorByte,colorByte,colorByte,0xff};
            lightImage->setPixel(x, y, pixel);
        }
    }

    TexturePtr tex = TexturePtr(new Texture(lightImage, true));
    tex->setSmooth(true);
    return tex;
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
    int intensity = std::min<int>(light.intensity, MAX_LIGHT_INTENSITY);
    int radius = intensity * Otc::TILE_PIXELS * scaleFactor;

    Color color = Color::from8bit(light.color);
    float brightness = 0.5f + (intensity/(float)MAX_LIGHT_INTENSITY)*0.5f;

    color.setRed(color.rF() * brightness);
    color.setGreen(color.gF() * brightness);
    color.setBlue(color.bF() * brightness);

    if(m_blendEquation == Painter::BlendEquation_Add && !m_lightMap.empty()) {
        LightSource prevSource = m_lightMap.back();
        if(prevSource.center == center && prevSource.color == color && prevSource.radius == radius)
            return;
    }

    LightSource source;
    source.center = center;
    source.color = color;
    source.radius = radius;
    m_lightMap.push_back(source);
}

void LightView::drawGlobalLight(const Light& light)
{
    Color color = Color::from8bit(light.color);
    float brightness = light.intensity / (float)MAX_AMBIENT_LIGHT_INTENSITY;
    color.setRed(color.rF() * brightness);
    color.setGreen(color.gF() * brightness);
    color.setBlue(color.bF() * brightness);
    g_painter->setColor(color);
    g_painter->drawFilledRect(Rect(0,0,m_lightbuffer->getSize()));
}

void LightView::drawLightSource(const Point& center, const Color& color, int radius)
{
    // debug draw
    //radius /= 16;

    Rect dest = Rect(center - Point(radius, radius), Size(radius*2,radius*2));
    g_painter->setColor(color);
    g_painter->drawTexturedRect(dest, m_lightTexture);
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
    g_painter->setBlendEquation(m_blendEquation);
    g_painter->setCompositionMode(Painter::CompositionMode_Add);
    for(const LightSource& source : m_lightMap)
        drawLightSource(source.center, source.color, source.radius);
    m_lightbuffer->release();
    g_painter->setCompositionMode(Painter::CompositionMode_Light);
    m_lightbuffer->draw(dest, src);
    g_painter->restoreSavedState();
}
