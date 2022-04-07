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

#include "painterogl2.h"
#include "framework/graphics/texture.h"
#include "painterogl2_shadersources.h"
#include <framework/platform/platformwindow.h>

PainterOGL2* g_painterOGL2 = nullptr;

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
    const int vertexCount = coordsBuffer.getVertexCount();
    if (vertexCount == 0)
        return;

    const bool textured = coordsBuffer.getTextureCoordCount() > 0 && m_texture;

    // skip drawing of empty textures
    if (textured && m_texture->isEmpty())
        return;

    m_drawProgram = m_shaderProgram ? m_shaderProgram : textured ? m_drawTexturedProgram.get() : m_drawSolidColorProgram.get();

    // update shader with the current painter state
    m_drawProgram->bind();
    m_drawProgram->setTransformMatrix(m_transformMatrix);
    m_drawProgram->setProjectionMatrix(m_projectionMatrix);
    if (textured) {
        m_drawProgram->setTextureMatrix(m_textureMatrix);
        m_drawProgram->bindMultiTextures();
    }
    m_drawProgram->setOpacity(m_opacity);
    m_drawProgram->setColor(m_color);
    m_drawProgram->setResolution(m_resolution);
    m_drawProgram->updateTime();

    // only set texture coords arrays when needed
    if (textured) {
        m_drawProgram->setAttributeArray(PainterShaderProgram::TEXCOORD_ATTR, coordsBuffer.getTextureCoordArray(), 2);
    } else
        PainterShaderProgram::disableAttributeArray(PainterShaderProgram::TEXCOORD_ATTR);

    // set vertex array
    m_drawProgram->setAttributeArray(PainterShaderProgram::VERTEX_ATTR, coordsBuffer.getVertexArray(), 2);

    // draw the element in coords buffers
    glDrawArrays(static_cast<GLenum>(drawMode), 0, vertexCount);

    if (!textured)
        PainterShaderProgram::enableAttributeArray(PainterShaderProgram::TEXCOORD_ATTR);
}
