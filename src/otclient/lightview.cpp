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
    m_lightbuffer = g_framebuffers.getTemporaryFrameBuffer();
    generateLightBuble();
    reset();
}

void LightView::generateLightBuble()
{
    const int size = 256;
    uint8_t dest[size*size*4];

    for(int x = 0; x < size; x++){
        for(int y = 0; y < size; y++){
            int norm = std::sqrt((size/2 - x)*(size/2 - x) + (size/2 - y)*(size/2 - y));
            float p = 128*norm/(size/2);

            float color = (p <= 128 ? 255 * (128-p)*(128-p)/(128*128) : 0);
            if(color < 0)
                color = 0;

            int line = y;

            dest[4*x + 4*size*line] = (uint8_t)color;
            dest[4*x + 4*size*line + 1] = (uint8_t)color;
            dest[4*x + 4*size*line + 2]  = (uint8_t)color;
            dest[4*x + 4*size*line + 3]  = 255;
        }
    }

    ImagePtr light = ImagePtr(new Image(Size(size,size), 4, dest));
    m_lightTexture = TexturePtr(new Texture(light, false));
    m_lightTexture->setSmooth(true);
}

void LightView::reset()
{
    m_numLights = 0;
}

void LightView::setGlobalLight(const Light& light)
{
    m_globalLight = light;
}

void LightView::addLightSource(const Point& center, float scaleFactor, const Light& light)
{
    if(m_numLights > MAX_LIGHTS)
        return;

    int radius = light.intensity * 64 * scaleFactor;

    m_lightMap[m_numLights].center = center;
    m_lightMap[m_numLights].color = Color::from8bit(light.color);
    m_lightMap[m_numLights].radius = radius;
    m_numLights++;
}

void LightView::drawGlobalLight(const Light& light)
{
    Color color = Color::from8bit(light.color);
    float factor = 0;//std::max<int>(256 - light.intensity, 0) / 256.0f;
    color.setRed(color.rF() * factor);
    color.setGreen(color.gF() * factor);
    color.setBlue(color.bF() * factor);
    color.setAlpha(1.0f);
    g_painter->clear(color);
}

void LightView::drawLightSource(const Point& center, const Color& color, int radius)
{
    Rect dest = Rect(center - Point(radius, radius), Size(radius*2,radius*2));
    g_painter->setColor(color);
    g_painter->drawTexturedRect(dest, m_lightTexture);

    // debug draw
    //g_painter->drawBoundingRect(dest);
}

void LightView::draw(Size size)
{
    m_lightbuffer->resize(size);
    m_lightbuffer->bind();
    drawGlobalLight(m_globalLight);
    g_painter->setCompositionMode(Painter::CompositionMode_Add);
    for(int i=0;i<m_numLights;++i)
        drawLightSource(m_lightMap[i].center, m_lightMap[i].color, m_lightMap[i].radius);
    m_lightbuffer->release();

    g_painter->setCompositionMode(Painter::CompositionMode_Light);
    m_lightbuffer->draw();
    g_painter->setCompositionMode(Painter::CompositionMode_Normal);
}
