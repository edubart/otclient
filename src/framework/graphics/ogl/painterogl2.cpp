/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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
#include "painterogl2_shadersources.h"
#include <framework/platform/platformwindow.h>

PainterOGL2 *g_painterOGL2 = nullptr;

PainterOGL2::PainterOGL2()
{
    m_drawProgram = nullptr;
    resetState();

    m_drawTexturedProgram = PainterShaderProgramPtr(new PainterShaderProgram);
    assert(m_drawTexturedProgram);
    m_drawTexturedProgram->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
    m_drawTexturedProgram->addShaderFromSourceCode(Shader::Fragment, glslMainFragmentShader + glslTextureSrcFragmentShader);
    m_drawTexturedProgram->link();

    m_drawSolidColorProgram = PainterShaderProgramPtr(new PainterShaderProgram);
    assert(m_drawSolidColorProgram);
    m_drawSolidColorProgram->addShaderFromSourceCode(Shader::Vertex, glslMainVertexShader + glslPositionOnlyVertexShader);
    m_drawSolidColorProgram->addShaderFromSourceCode(Shader::Fragment, glslMainFragmentShader + glslSolidColorFragmentShader);
    m_drawSolidColorProgram->link();

    PainterShaderProgram::release();
}

void PainterOGL2::bind()
{
    PainterOGL::bind();

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

    bool textured = coordsBuffer.getTextureCoordCount() > 0 && m_texture;

    // skip drawing of empty textures
    if(textured && m_texture->isEmpty())
        return;

    // update shader with the current painter state
    m_drawProgram->bind();
    m_drawProgram->setTransformMatrix(m_transformMatrix);
    m_drawProgram->setProjectionMatrix(m_projectionMatrix);
    if(textured) {
        m_drawProgram->setTextureMatrix(m_textureMatrix);
        m_drawProgram->bindMultiTextures();
    }
    m_drawProgram->setOpacity(m_opacity);
    m_drawProgram->setColor(m_color);
    m_drawProgram->setResolution(m_resolution);
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
    } else
        PainterShaderProgram::disableAttributeArray(PainterShaderProgram::TEXCOORD_ATTR);

    // set vertex array
    if(hardwareCached) {
        coordsBuffer.getHardwareVertexArray()->bind();
        m_drawProgram->setAttributeArray(PainterShaderProgram::VERTEX_ATTR, nullptr, 2);
        HardwareBuffer::unbind(HardwareBuffer::VertexBuffer);
    } else
        m_drawProgram->setAttributeArray(PainterShaderProgram::VERTEX_ATTR, coordsBuffer.getVertexArray(), 2);

    // draw the element in coords buffers
    if(drawMode == Triangles)
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    else if(drawMode == TriangleStrip)
        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);

    if(!textured)
        PainterShaderProgram::enableAttributeArray(PainterShaderProgram::TEXCOORD_ATTR);
}

void PainterOGL2::drawFillCoords(CoordsBuffer& coordsBuffer)
{
    setDrawProgram(m_shaderProgram ? m_shaderProgram : m_drawSolidColorProgram.get());
    setTexture(nullptr);
    drawCoords(coordsBuffer);
}

void PainterOGL2::drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture)
{
    if(texture && texture->isEmpty())
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : m_drawTexturedProgram.get());
    setTexture(texture);
    drawCoords(coordsBuffer);
}

void PainterOGL2::drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : m_drawTexturedProgram.get());
    setTexture(texture);

    m_coordsBuffer.clear();
    m_coordsBuffer.addQuad(dest, src);
    drawCoords(m_coordsBuffer, TriangleStrip);
}

void PainterOGL2::drawUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : m_drawTexturedProgram.get());
    setTexture(texture);

    m_coordsBuffer.clear();
    m_coordsBuffer.addUpsideDownQuad(dest, src);
    drawCoords(m_coordsBuffer, TriangleStrip);
}

void PainterOGL2::drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : m_drawTexturedProgram.get());
    setTexture(texture);

    m_coordsBuffer.clear();
    m_coordsBuffer.addRepeatedRects(dest, src);
    drawCoords(m_coordsBuffer);
}

void PainterOGL2::drawFilledRect(const Rect& dest)
{
    if(dest.isEmpty())
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : m_drawSolidColorProgram.get());

    m_coordsBuffer.clear();
    m_coordsBuffer.addRect(dest);
    drawCoords(m_coordsBuffer);
}

void PainterOGL2::drawFilledTriangle(const Point& a, const Point& b, const Point& c)
{
    if(a == b || a == c || b == c)
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : m_drawSolidColorProgram.get());

    m_coordsBuffer.clear();
    m_coordsBuffer.addTriangle(a, b, c);
    drawCoords(m_coordsBuffer);
}

void PainterOGL2::drawBoundingRect(const Rect& dest, int innerLineWidth)
{
    if(dest.isEmpty() || innerLineWidth == 0)
        return;

    setDrawProgram(m_shaderProgram ? m_shaderProgram : m_drawSolidColorProgram.get());

    m_coordsBuffer.clear();
    m_coordsBuffer.addBoudingRect(dest, innerLineWidth);
    drawCoords(m_coordsBuffer);
}
