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

#include "painterogl2.h"
#include "texture.h"
#include "paintershadermanager.h"

PainterOGL2 *g_painterOGL2 = nullptr;

PainterOGL2::PainterOGL2()
{
    m_drawProgram = nullptr;
    resetState();

    g_shaders.init();
}

PainterOGL2::~PainterOGL2()
{
    g_shaders.terminate();
}

void PainterOGL2::bind()
{
    Painter::bind();

    // vertex and texture coord attributes are always enabled
    // to avoid massive enable/disables, thus improving frame rate
    PainterShaderProgram::enableAttributeArray(PainterShaderProgram::VERTEX_ATTR);
    PainterShaderProgram::enableAttributeArray(PainterShaderProgram::TEXCOORD_ATTR);
}

void PainterOGL2::unbind()
{
    PainterShaderProgram::disableAttributeArray(PainterShaderProgram::VERTEX_ATTR);
    PainterShaderProgram::disableAttributeArray(PainterShaderProgram::TEXCOORD_ATTR);
    PainterShaderProgram::release();
}

void PainterOGL2::drawCoords(CoordsBuffer& coordsBuffer, DrawMode drawMode)
{
    int vertexCount = coordsBuffer.getVertexCount();
    if(vertexCount == 0)
        return;

    bool textured = coordsBuffer.getTextureCoordCount() > 0;

    // update shader with the current painter state
    m_drawProgram->bind();
    m_drawProgram->setProjectionMatrix(m_projectionMatrix);
    if(textured)
        m_drawProgram->setTextureMatrix(m_textureMatrix);
    m_drawProgram->setOpacity(m_opacity);
    m_drawProgram->setColor(m_color);
    m_drawProgram->updateTime();

    // update coords buffer hardware caches if enabled
    coordsBuffer.updateCaches();
    bool hardwareCached = coordsBuffer.isHardwareCached();

    // only set texture coords arrays when needed
    if(textured) {
        if(hardwareCached) {
            coordsBuffer.getHardwareTextureCoordArray()->bind();
            m_drawProgram->setAttributeArray(PainterShaderProgram::TEXCOORD_ATTR, nullptr, 2);
        } else
            m_drawProgram->setAttributeArray(PainterShaderProgram::TEXCOORD_ATTR, coordsBuffer.getTextureCoordArray(), 2);
    }

    // set vertex array
    if(hardwareCached) {
        coordsBuffer.getHardwareVertexArray()->bind();
        m_drawProgram->setAttributeArray(PainterShaderProgram::VERTEX_ATTR, nullptr, 2);
        HardwareBuffer::unbind(HardwareBuffer::VertexBuffer);
    } else
        m_drawProgram->setAttributeArray(PainterShaderProgram::VERTEX_ATTR, coordsBuffer.getVertexArray(), 2);

    // draw the element in coords buffers
    glDrawArrays(drawMode, 0, vertexCount);
}

void PainterOGL2::drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture)
{
    if(!texture->getId())
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : g_shaders.getDrawTexturedProgram().get());
    setTexture(texture);
    drawCoords(coordsBuffer);
}

void PainterOGL2::drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || !texture->getId())
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : g_shaders.getDrawTexturedProgram().get());
    setTexture(texture);

    m_coordsBuffer.clear();
    m_coordsBuffer.addQuad(dest, src);
    drawCoords(m_coordsBuffer, TriangleStrip);
}

void PainterOGL2::drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || !texture->getId())
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : g_shaders.getDrawTexturedProgram().get());
    setTexture(texture);

    m_coordsBuffer.clear();
    m_coordsBuffer.addRepeatedRects(dest, src);
    drawCoords(m_coordsBuffer);
}

void PainterOGL2::drawFilledRect(const Rect& dest)
{
    if(dest.isEmpty())
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : g_shaders.getDrawSolidColorProgram().get());

    m_coordsBuffer.clear();
    m_coordsBuffer.addRect(dest);
    drawCoords(m_coordsBuffer);
}

void PainterOGL2::drawBoundingRect(const Rect& dest, int innerLineWidth)
{
    if(dest.isEmpty() || innerLineWidth == 0)
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : g_shaders.getDrawSolidColorProgram().get());

    m_coordsBuffer.clear();
    m_coordsBuffer.addBoudingRect(dest, innerLineWidth);
    drawCoords(m_coordsBuffer);
}
