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

#if !defined(OPENGL_ES) || OPENGL_ES==1

#include "painterogl1.h"
#include <framework/graphics/graphics.h>

PainterOGL1 *g_painterOGL1 = nullptr;

PainterOGL1::PainterOGL1()
{
    m_matrixMode = GL_PROJECTION;
    resetState();
}

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
    if(g_graphics.canUseDrawArrays())
        glEnableClientState(GL_VERTEX_ARRAY);
}

void PainterOGL1::unbind()
{
    if(g_graphics.canUseDrawArrays())
        glDisableClientState(GL_VERTEX_ARRAY);
}

void PainterOGL1::drawCoords(CoordsBuffer& coordsBuffer, DrawMode drawMode)
{
    int vertexCount = coordsBuffer.getVertexCount();
    if(vertexCount == 0)
        return;

    bool textured = coordsBuffer.getTextureCoordCount() != 0 && m_texture;

    // skip drawing of empty textures
    if(textured && m_texture->isEmpty())
        return;

    if(textured != m_textureEnabled) {
        m_textureEnabled = textured;
        updateGlTextureState();
    }

    // GDI Generic driver has this bug
    if(g_graphics.hasScissorBug())
        updateGlClipRect();

    // use vertex arrays if possible, much faster
    if(g_graphics.canUseDrawArrays()) {
        // update coords buffer hardware caches if enabled
        coordsBuffer.updateCaches();
        bool hardwareCached = coordsBuffer.isHardwareCached();

        // only set texture coords arrays when needed
        if(textured) {
            if(hardwareCached) {
                coordsBuffer.getHardwareTextureCoordArray()->bind();
                glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
            } else
                glTexCoordPointer(2, GL_FLOAT, 0, coordsBuffer.getTextureCoordArray());
        }

        // set vertex array
        if(hardwareCached) {
            coordsBuffer.getHardwareVertexArray()->bind();
            glVertexPointer(2, GL_FLOAT, 0, nullptr);
        } else
            glVertexPointer(2, GL_FLOAT, 0, coordsBuffer.getVertexArray());

        if(hardwareCached)
            HardwareBuffer::unbind(HardwareBuffer::VertexBuffer);

        // draw the element in coords buffers
        if(drawMode == Triangles)
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        else if(drawMode == TriangleStrip)
            glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
    }
#ifndef OPENGL_ES
    else {
        int verticesSize = vertexCount*2;
        float *vertices = coordsBuffer.getVertexArray();
        float *texCoords = coordsBuffer.getTextureCoordArray();

        // use glBegin/glEnd, this is not available in OpenGL ES
        // and is considered much slower then glDrawArrays,
        // but this code is executed in really old graphics cards
        if(drawMode == Triangles)
            glBegin(GL_TRIANGLES);
        else if(drawMode == TriangleStrip)
            glBegin(GL_TRIANGLE_STRIP);
        for(int i=0;i<verticesSize;i+=2) {
            if(textured)
                glTexCoord2f(texCoords[i], texCoords[i+1]);
            glVertex2f(vertices[i], vertices[i+1]);
        }
        glEnd();
    }
#endif
}

void PainterOGL1::drawFillCoords(CoordsBuffer& coordsBuffer)
{
    setTexture(nullptr);
    drawCoords(coordsBuffer);
}

void PainterOGL1::drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture)
{
    if(texture->isEmpty())
        return;

    setTexture(texture.get());
    drawCoords(coordsBuffer);
}

void PainterOGL1::drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
        return;

    setTexture(texture.get());

    m_coordsBuffer.clear();
    m_coordsBuffer.addQuad(dest, src);
    drawCoords(m_coordsBuffer, TriangleStrip);
}

void PainterOGL1::drawUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
        return;

    setTexture(texture.get());

    m_coordsBuffer.clear();
    m_coordsBuffer.addUpsideDownQuad(dest, src);
    drawCoords(m_coordsBuffer, TriangleStrip);
}

void PainterOGL1::drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
        return;

    setTexture(texture.get());

    m_coordsBuffer.clear();
    m_coordsBuffer.addRepeatedRects(dest, src);
    drawCoords(m_coordsBuffer);
}

void PainterOGL1::drawFilledRect(const Rect& dest)
{
    if(dest.isEmpty())
        return;

    setTexture(nullptr);

    m_coordsBuffer.clear();
    m_coordsBuffer.addRect(dest);
    drawCoords(m_coordsBuffer);
}

void PainterOGL1::drawFilledTriangle(const Point& a, const Point& b, const Point& c)
{
    if(a == b || a == c || b == c)
        return;

    setTexture(nullptr);

    m_coordsBuffer.clear();
    m_coordsBuffer.addTriangle(a, b, c);
    drawCoords(m_coordsBuffer);
}

void PainterOGL1::drawBoundingRect(const Rect& dest, int innerLineWidth)
{
    if(dest.isEmpty() || innerLineWidth == 0)
        return;

    setTexture(nullptr);

    m_coordsBuffer.clear();
    m_coordsBuffer.addBoudingRect(dest, innerLineWidth);
    drawCoords(m_coordsBuffer);
}

void PainterOGL1::setMatrixMode(PainterOGL1::MatrixMode matrixMode)
{
    if(m_matrixMode == matrixMode)
        return;
    m_matrixMode = matrixMode;
    updateGlMatrixMode();
}

void PainterOGL1::setTransformMatrix(const Matrix3& transformMatrix)
{
    m_transformMatrix = transformMatrix;
    if(g_painter == this)
        updateGlTransformMatrix();
}

void PainterOGL1::setProjectionMatrix(const Matrix3& projectionMatrix)
{
    m_projectionMatrix = projectionMatrix;
    if(g_painter == this)
        updateGlProjectionMatrix();
}

void PainterOGL1::setTextureMatrix(const Matrix3& textureMatrix)
{
    // avoid re-updating texture matrix
    if(m_textureMatrix == textureMatrix)
        return;
    m_textureMatrix = textureMatrix;
    updateGlTextureMatrix();
}

void PainterOGL1::setColor(const Color& color)
{
    if(m_color == color)
        return;
    m_color = color;
    updateGlColor();
}

void PainterOGL1::setOpacity(float opacity)
{
    if(m_opacity == opacity)
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
    float glTransformMatrix[] = {
        m_transformMatrix(1,1), m_transformMatrix(1,2),                    0.0f, m_transformMatrix(1,3),
        m_transformMatrix(2,1), m_transformMatrix(2,2),                    0.0f, m_transformMatrix(2,3),
                          0.0f,                   0.0f,                    1.0f,                   0.0f,
        m_transformMatrix(3,1), m_transformMatrix(3,2),                    0.0f, m_transformMatrix(3,3),
    };

    setMatrixMode(MatrixTransform);
    glLoadMatrixf(glTransformMatrix);
}

void PainterOGL1::updateGlProjectionMatrix()
{
    float glProjectionMatrix[] = {
        m_projectionMatrix(1,1), m_projectionMatrix(1,2),                    0.0f, m_projectionMatrix(1,3),
        m_projectionMatrix(2,1), m_projectionMatrix(2,2),                    0.0f, m_projectionMatrix(2,3),
                           0.0f,                    0.0f,                    1.0f,                    0.0f,
        m_projectionMatrix(3,1), m_projectionMatrix(3,2),                    0.0f, m_projectionMatrix(3,3),
    };

    setMatrixMode(MatrixProjection);
    glLoadMatrixf(glProjectionMatrix);
}

void PainterOGL1::updateGlTextureMatrix()
{
    float glTextureMatrix[] = {
        m_textureMatrix(1,1), m_textureMatrix(1,2),             0.0f,                 0.0f,
        m_textureMatrix(2,1), m_textureMatrix(2,2),             0.0f,                 0.0f,
                        0.0f,                 0.0f,             1.0f,                 0.0f,
        m_textureMatrix(3,1), m_textureMatrix(3,2),             0.0f, m_textureMatrix(3,3),
    };

    setMatrixMode(MatrixTexture);
    glLoadMatrixf(glTextureMatrix);
}

void PainterOGL1::updateGlTextureState()
{
    if(m_textureEnabled) {
        glEnable(GL_TEXTURE_2D);
        if(g_graphics.canUseDrawArrays())
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    } else {
        glDisable(GL_TEXTURE_2D);
        if(g_graphics.canUseDrawArrays())
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

#endif
