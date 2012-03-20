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

#include "painter.h"
#include "texture.h"
#include "paintershadersources.h"
#include "paintershaderprogram.h"
#include "shaderprogram.h"
#include "graphics.h"
#include "vertexarray.h"

Painter g_painter;

void Painter::init()
{
    setColor(Color::white);
    setOpacity(1.0f);
    setCompositionMode(CompositionMode_Normal);

    m_drawTexturedProgram = PainterShaderProgramPtr(new PainterShaderProgram);
    m_drawTexturedProgram->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
    m_drawTexturedProgram->addShaderFromSourceCode(Shader::Fragment, glslMainFragmentShader + glslTextureSrcFragmentShader);
    m_drawTexturedProgram->link();

    m_drawSolidColorProgram = PainterShaderProgramPtr(new PainterShaderProgram);
    m_drawSolidColorProgram->addShaderFromSourceCode(Shader::Vertex, glslMainVertexShader + glslPositionOnlyVertexShader);
    m_drawSolidColorProgram->addShaderFromSourceCode(Shader::Fragment, glslMainFragmentShader + glslSolidColorFragmentShader);
    m_drawSolidColorProgram->link();
}

void Painter::terminate()
{
    m_drawTexturedProgram.reset();
    m_drawSolidColorProgram.reset();
}

void Painter::drawProgram(const PainterShaderProgramPtr& program, CoordsBuffer& coordsBuffer, PainterShaderProgram::DrawMode drawMode)
{
    if(coordsBuffer.getVertexCount() == 0)
        return;

    program->setProjectionMatrix(m_projectionMatrix);
    program->setOpacity(m_opacity);
    program->setColor(m_color);
    program->draw(coordsBuffer, drawMode);
}

void Painter::drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture)
{
    PainterShaderProgramPtr& program = m_customProgram ? m_customProgram : m_drawTexturedProgram;
    program->setTexture(texture);
    drawProgram(program, coordsBuffer);
}

void Painter::drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || !texture->getId())
        return;

    m_coordsBuffer.clear();
    m_coordsBuffer.addRect(dest, src);
    drawTextureCoords(m_coordsBuffer, texture);
}

void Painter::drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || !texture->getId())
        return;

    m_coordsBuffer.clear();
    m_coordsBuffer.addRepeatedRects(dest, src);
    drawTextureCoords(m_coordsBuffer, texture);
}

void Painter::drawFilledRect(const Rect& dest)
{
    if(dest.isEmpty())
        return;

    m_coordsBuffer.clear();
    m_coordsBuffer.addRect(dest);
    drawProgram(m_customProgram ? m_customProgram : m_drawSolidColorProgram, m_coordsBuffer);
}

void Painter::drawBoundingRect(const Rect& dest, int innerLineWidth)
{
    if(dest.isEmpty() || innerLineWidth == 0)
        return;

    m_coordsBuffer.clear();
    m_coordsBuffer.addBoudingRect(dest, innerLineWidth);
    drawProgram(m_customProgram ? m_customProgram : m_drawSolidColorProgram, m_coordsBuffer);
}

void Painter::setCustomProgram(PainterShaderProgramPtr program)
{
    m_customProgram = program;
}

void Painter::setCompositionMode(Painter::CompositionMode compositionMode)
{
    switch(compositionMode) {
    case CompositionMode_Normal:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case CompositionMode_Multiply:
        glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case CompositionMode_Add:
        glBlendFunc(GL_ONE, GL_ONE);
        break;
    case CompositionMode_Replace:
        glBlendFunc(GL_ONE, GL_ZERO);
        break;
    case CompositionMode_DestBlending:
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
        break;
    }
    m_compostionMode = compositionMode;
}

void Painter::saveAndResetState()
{
    m_oldCustomProgram = m_customProgram;
    m_oldProjectionMatrix = m_projectionMatrix;
    m_oldColor = m_color;
    m_oldOpacity = m_opacity;
    m_oldCompostionMode = m_compostionMode;

    releaseCustomProgram();
    setColor(Color::white);
    setOpacity(1);
    setCompositionMode(CompositionMode_Normal);
}

void Painter::restoreSavedState()
{
    setCustomProgram(m_oldCustomProgram);
    setColor(m_oldColor);
    setOpacity(m_oldOpacity);
    setCompositionMode(m_oldCompostionMode);

    m_oldCustomProgram = nullptr;
    m_oldColor = Color::white;
    m_oldOpacity = 1;
    m_oldCompostionMode = CompositionMode_Normal;
}
