/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#include "framework/graphics/texture.h"
#if !defined(OPENGL_ES) || OPENGL_ES==1

#include "painterogl1.h"
#include <framework/graphics/graphics.h>

PainterOGL1* g_painterOGL1 = nullptr;

void PainterOGL1::refreshState()
{
    PainterOGL::refreshState();
    updateGlColor();
    updateGlMatrixMode();
    updateGlTransformMatrix();
    updateGlProjectionMatrix();
    updateGlTextureMatrix();
    updateGlTextureState();
}

void PainterOGL1::bind()
{
    PainterOGL::bind();

    // vertex and texture coord arrays are always enabled
    // to avoid massive enable/disables, thus improving frame rate
    if (g_graphics.canUseDrawArrays())
        glEnableClientState(GL_VERTEX_ARRAY);
}

void PainterOGL1::unbind()
{
    if (g_graphics.canUseDrawArrays())
        glDisableClientState(GL_VERTEX_ARRAY);
}

void PainterOGL1::drawCoords(CoordsBuffer& coordsBuffer, DrawMode drawMode)
{
    const int vertexCount = coordsBuffer.getVertexCount();
    if (vertexCount == 0)
        return;

    const bool textured = coordsBuffer.getTextureCoordCount() != 0 && m_texture;

    // skip drawing of empty textures
    if (textured && m_texture->isEmpty())
        return;

    if (textured != m_textureEnabled) {
        m_textureEnabled = textured;
        updateGlTextureState();
    }

    // GDI Generic driver has this bug
    if (g_graphics.hasScissorBug())
        updateGlClipRect();

    // use vertex arrays if possible, much faster
    if (g_graphics.canUseDrawArrays()) {
        // only set texture coords arrays when needed
        if (textured) {
            glTexCoordPointer(2, GL_FLOAT, 0, coordsBuffer.getTextureCoordArray());
        }

        // set vertex array
        glVertexPointer(2, GL_FLOAT, 0, coordsBuffer.getVertexArray());

        // draw the element in coords buffers
        glDrawArrays(static_cast<GLenum>(drawMode), 0, vertexCount);
    }
#ifndef OPENGL_ES
    else {
        const int verticesSize = vertexCount * 2;
        const float* vertices = coordsBuffer.getVertexArray();
        const float* texCoords = coordsBuffer.getTextureCoordArray();

        // use glBegin/glEnd, this is not available in OpenGL ES
        // and is considered much slower then glDrawArrays,
        // but this code is executed in really old graphics cards

        glBegin(static_cast<GLenum>(drawMode));
        for (int i = 0; i < verticesSize; i += 2) {
            if (textured)
                glTexCoord2f(texCoords[i], texCoords[i + 1]);
            glVertex2f(vertices[i], vertices[i + 1]);
        }
        glEnd();
    }
#endif
}

void PainterOGL1::setMatrixMode(MatrixMode matrixMode)
{
    if (m_matrixMode == static_cast<GLenum>(matrixMode))
        return;

    m_matrixMode = static_cast<GLenum>(matrixMode);
    updateGlMatrixMode();
}

void PainterOGL1::setTransformMatrix(const Matrix3& transformMatrix)
{
    m_transformMatrix = transformMatrix;
    if (g_painter == this)
        updateGlTransformMatrix();
}

void PainterOGL1::setProjectionMatrix(const Matrix3& projectionMatrix)
{
    m_projectionMatrix = projectionMatrix;
    if (g_painter == this)
        updateGlProjectionMatrix();
}

void PainterOGL1::setTextureMatrix(const Matrix3& textureMatrix)
{
    // avoid re-updating texture matrix
    if (m_textureMatrix == textureMatrix)
        return;
    m_textureMatrix = textureMatrix;
    updateGlTextureMatrix();
}

void PainterOGL1::setColor(const Color& color)
{
    if (m_color == color)
        return;
    m_color = color;
    updateGlColor();
}

void PainterOGL1::setOpacity(float opacity)
{
    if (m_opacity == opacity)
        return;
    m_opacity = opacity;
    updateGlColor();
}

void PainterOGL1::updateGlColor()
{
    glColor4f(m_color.rF(), m_color.gF(), m_color.bF(), m_color.aF() * m_opacity);
}

void PainterOGL1::updateGlMatrixMode()
{
    glMatrixMode(m_matrixMode);
}

void PainterOGL1::updateGlTransformMatrix()
{
    const float glTransformMatrix[] = {
            m_transformMatrix(1,1), m_transformMatrix(1,2),                    0.0f, m_transformMatrix(1,3),
            m_transformMatrix(2,1), m_transformMatrix(2,2),                    0.0f, m_transformMatrix(2,3),
                              0.0f,                   0.0f,                    1.0f,                   0.0f,
            m_transformMatrix(3,1), m_transformMatrix(3,2),                    0.0f, m_transformMatrix(3,3),
    };

    setMatrixMode(MatrixMode::TRANSFORM);
    glLoadMatrixf(glTransformMatrix);
}

void PainterOGL1::updateGlProjectionMatrix()
{
    const float glProjectionMatrix[] = {
            m_projectionMatrix(1,1), m_projectionMatrix(1,2),                    0.0f, m_projectionMatrix(1,3),
            m_projectionMatrix(2,1), m_projectionMatrix(2,2),                    0.0f, m_projectionMatrix(2,3),
                               0.0f,                    0.0f,                    1.0f,                    0.0f,
            m_projectionMatrix(3,1), m_projectionMatrix(3,2),                    0.0f, m_projectionMatrix(3,3),
    };

    setMatrixMode(MatrixMode::PROJECTION);
    glLoadMatrixf(glProjectionMatrix);
}

void PainterOGL1::updateGlTextureMatrix()
{
    const float glTextureMatrix[] = {
            m_textureMatrix(1,1), m_textureMatrix(1,2),             0.0f,                 0.0f,
            m_textureMatrix(2,1), m_textureMatrix(2,2),             0.0f,                 0.0f,
                            0.0f,                 0.0f,             1.0f,                 0.0f,
            m_textureMatrix(3,1), m_textureMatrix(3,2),             0.0f, m_textureMatrix(3,3),
    };

    setMatrixMode(MatrixMode::TEXTURE);
    glLoadMatrixf(glTextureMatrix);
}

void PainterOGL1::updateGlTextureState()
{
    if (m_textureEnabled) {
        glEnable(GL_TEXTURE_2D);
        if (g_graphics.canUseDrawArrays())
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    } else {
        glDisable(GL_TEXTURE_2D);
        if (g_graphics.canUseDrawArrays())
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

#endif
