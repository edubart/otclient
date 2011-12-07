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

#include "painter.h"
#include "texture.h"
#include "paintershadersources.h"
#include "paintershaderprogram.h"
#include "shaderprogram.h"
#include "graphics.h"

Painter g_painter;

void Painter::init()
{
    setColor(Fw::white);
    setOpacity(255);
    setCompositionMode(CompositionMode_SourceOver);

    PainterShaderProgramPtr program = PainterShaderProgramPtr(new PainterShaderProgram);
    program->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader);
    program->addShaderFromSourceCode(Shader::Vertex, glslPositionOnlyVertexShader);
    program->addShaderFromSourceCode(Shader::Fragment, glslMainFragmentShader);
    program->addShaderFromSourceCode(Shader::Fragment, glslTextureSrcFragmentShader);
    program->bindAttributeLocation(VERTEX_COORDS_ATTR, "vertexCoord");
    program->bindAttributeLocation(TEXTURE_COORDS_ATTR, "textureCoord");
    assert(program->link());
    program->bindUniformLocation(PainterShaderProgram::PROJECTION_MATRIX_UNIFORM, "projectionMatrix");
    program->bindUniformLocation(PainterShaderProgram::TEXTURE_TRANSFORM_MATRIX_UNIFORM, "textureTransformMatrix");
    program->bindUniformLocation(PainterShaderProgram::COLOR_UNIFORM, "color");
    program->bindUniformLocation(PainterShaderProgram::OPACITY_UNIFORM, "opacity");
    program->bindUniformLocation(PainterShaderProgram::TEXTURE_UNIFORM, "texture");
    m_drawTexturedProgram = program;

    program = PainterShaderProgramPtr(new PainterShaderProgram);
    program->addShaderFromSourceCode(Shader::Vertex, glslMainVertexShader);
    program->addShaderFromSourceCode(Shader::Vertex, glslPositionOnlyVertexShader);
    program->addShaderFromSourceCode(Shader::Fragment, glslMainFragmentShader);
    program->addShaderFromSourceCode(Shader::Fragment, glslSolidColorFragmentShader);
    program->bindAttributeLocation(VERTEX_COORDS_ATTR, "vertexCoord");
    assert(program->link());
    program->bindUniformLocation(PainterShaderProgram::PROJECTION_MATRIX_UNIFORM, "projectionMatrix");
    program->bindUniformLocation(PainterShaderProgram::COLOR_UNIFORM, "color");
    program->bindUniformLocation(PainterShaderProgram::OPACITY_UNIFORM, "opacity");
    m_drawSolidColorProgram = program;
}

void Painter::terminate()
{
    m_drawTexturedProgram.reset();
    m_drawSolidColorProgram.reset();
}

void Painter::updateProjectionMatrix(const Size& viewportSize, bool inverseYAxis)
{
    // The projection matrix converts from Painter's coordinate system to GL's coordinate system
    //    * GL's viewport is 2x2, Painter's is width x height
    //    * GL has +y -> -y going from bottom -> top, Painter is the other way round
    //    * GL has [0,0] in the center, Painter has it in the top-left
    //
    // This results in the Projection matrix below, which is multiplied by the painter's
    // transformation matrix, as shown below:
    //
    //                Projection Matrix                   Painter Coord   GL Coord
    // ------------------------------------------------     ---------     ---------
    // | 2.0 / width  |      0.0      |     -1.0      |     |   x   |     |   y'  |
    // |     0.0      | -2.0 / height |      1.0      |  *  |   y   |  =  |   x'  |
    // |     0.0      |      0.0      |      0.0      |     |   1   |     |   0   |
    // ------------------------------------------------     ---------     ---------
    float w = viewportSize.width();
    float h = viewportSize.height();
    if(inverseYAxis) {
        m_projectionMatrix[0][0] = 2.0f/w;  m_projectionMatrix[0][1] = 0.0f;    m_projectionMatrix[0][2] =-1.0f;
        m_projectionMatrix[1][0] = 0.0f;    m_projectionMatrix[1][1] = 2.0f/h;  m_projectionMatrix[1][2] =-1.0f;
        m_projectionMatrix[2][0] = 0.0f;    m_projectionMatrix[2][1] = 0.0f;    m_projectionMatrix[2][2] = 0.0f;
    } else {
        m_projectionMatrix[0][0] = 2.0f/w;  m_projectionMatrix[0][1] = 0.0f;    m_projectionMatrix[0][2] =-1.0f;
        m_projectionMatrix[1][0] = 0.0f;    m_projectionMatrix[1][1] =-2.0f/h;  m_projectionMatrix[1][2] = 1.0f;
        m_projectionMatrix[2][0] = 0.0f;    m_projectionMatrix[2][1] = 0.0f;    m_projectionMatrix[2][2] = 0.0f;
    }
}

void Painter::drawTexturedRect(const Rect& dest, const TexturePtr& texture)
{
    drawTexturedRect(dest, texture, Rect(Point(0,0), texture->getSize()));
}

void Painter::drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || !texture->getId())
        return;

    GLfloat vertexCoords[] = { (float)dest.left(), (float)dest.top(),
                               (float)dest.right()+1, (float)dest.top(),
                               (float)dest.left(), (float)dest.bottom()+1,
                               (float)dest.right()+1, (float)dest.bottom()+1 };

    GLfloat textureCoords[] = { (float)src.left(), (float)src.top(),
                                (float)src.right()+1, (float)src.top(),
                                (float)src.left(), (float)src.bottom()+1,
                                (float)src.right()+1, (float)src.bottom()+1 };

    m_drawTexturedProgram->prepareForDraw();
    m_drawTexturedProgram->setProjectionMatrix(m_projectionMatrix);
    m_drawTexturedProgram->setOpacity(m_currentOpacity);
    m_drawTexturedProgram->setColor(m_currentColor);
    m_drawTexturedProgram->setTexture(texture);
    m_drawTexturedProgram->setVertexCoords(vertexCoords);
    m_drawTexturedProgram->setTextureCoords(textureCoords);
    m_drawTexturedProgram->drawTriangleStrip(4);
    m_drawTexturedProgram->releaseFromDraw();
}

void Painter::drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || !texture->getId())
        return;

    //TODO: use vertex arrays..
    Rect virtualDest(0, 0, dest.size());
    for(int y = 0; y <= virtualDest.height(); y += src.height()) {
        for(int x = 0; x <= virtualDest.width(); x += src.width()) {
            Rect partialDest(x, y, src.size());
            Rect partialSrc = src;

            // partialCoords to screenCoords bottomRight
            if(partialDest.bottom() > virtualDest.bottom()) {
                partialSrc.setBottom(partialSrc.bottom() + (virtualDest.bottom() - partialDest.bottom()));
                partialDest.setBottom(virtualDest.bottom());
            }
            if(partialDest.right() > virtualDest.right()) {
                partialSrc.setRight(partialSrc.right() + (virtualDest.right() - partialDest.right()));
                partialDest.setRight(virtualDest.right());
            }

            partialDest.translate(dest.topLeft());
            drawTexturedRect(partialDest, texture, partialSrc);
        }
    }
}

void Painter::drawFilledRect(const Rect& dest)
{
    if(dest.isEmpty())
        return;

    GLfloat right = dest.right()+1;
    GLfloat bottom = dest.bottom()+1;
    GLfloat top = dest.top();
    GLfloat left = dest.left();

    GLfloat vertexCoords[] = { left, top,
                               right, top,
                               left, bottom,
                               right, bottom };

    m_drawSolidColorProgram->prepareForDraw();
    m_drawSolidColorProgram->setProjectionMatrix(m_projectionMatrix);
    m_drawSolidColorProgram->setOpacity(m_currentOpacity);
    m_drawSolidColorProgram->setColor(m_currentColor);
    m_drawSolidColorProgram->setVertexCoords(vertexCoords);
    m_drawSolidColorProgram->drawTriangleStrip(4);
    m_drawSolidColorProgram->releaseFromDraw();
}

void Painter::drawBoundingRect(const Rect& dest, int innerLineWidth)
{
    if(dest.isEmpty() || innerLineWidth == 0)
        return;

    GLfloat right = dest.right()+1;
    GLfloat bottom = dest.bottom()+1;
    GLfloat top = dest.top();
    GLfloat left = dest.left();
    GLfloat w = innerLineWidth;

    GLfloat vertexCoords[] = { left, top,
                               right, top,
                               left, top+w,
                               right, top+w,
                               right-w, top+w,
                               right, bottom,
                               right-w, bottom,
                               right-w, bottom-w,
                               left, bottom,
                               left, bottom-w,
                               left+w, bottom-w,
                               left, top+w,
                               left+w, top+w };

    m_drawSolidColorProgram->prepareForDraw();
    m_drawSolidColorProgram->setProjectionMatrix(m_projectionMatrix);
    m_drawSolidColorProgram->setOpacity(m_currentOpacity);
    m_drawSolidColorProgram->setColor(m_currentColor);
    m_drawSolidColorProgram->setVertexCoords(vertexCoords);
    m_drawSolidColorProgram->drawTriangleStrip(13);
    m_drawSolidColorProgram->releaseFromDraw();
}

void Painter::setCompositionMode(Painter::CompositionMode compositionMode)
{
    switch(compositionMode) {
    case CompositionMode_SourceOver:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case CompositionMode_ColorizeDest:
        glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
        break;
    }
}
