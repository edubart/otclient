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

#include "painterogl1.h"
#include "graphics.h"

PainterOGL1 *g_painterOGL1 = nullptr;

PainterOGL1::PainterOGL1()
{
    m_matrixMode = GL_PROJECTION;
    resetState();
}

void PainterOGL1::refreshState()
{
    Painter::refreshState();
    updateGlColor();
    updateGlMatrixMode();
    updateGlProjectionMatrix();
    updateGlTextureMatrix();
}

void PainterOGL1::bind()
{
    Painter::bind();

    // vertex and texture coord arrays are always enabled
    // to avoid massive enable/disables, thus improving frame rate
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
}

void PainterOGL1::unbind()
{
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void PainterOGL1::drawCoords(CoordsBuffer& coordsBuffer, DrawMode drawMode)
{
    int vertexCount = coordsBuffer.getVertexCount();
    if(vertexCount == 0)
        return;

    bool textured = coordsBuffer.getTextureCoordCount() != 0;
    if(textured)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);

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
        glDrawArrays(drawMode, 0, vertexCount);
    } else {
        int verticesSize = vertexCount*2;
        float *vertices = coordsBuffer.getVertexArray();
        float *texCoords = coordsBuffer.getTextureCoordArray();

        // use glBegin/glEnd, this is not available in OpenGL ES
        // and is considered much slower then glDrawArrays,
        // but this code is executed in really old graphics cards
        glBegin(drawMode);
        for(int i=0;i<verticesSize;i+=2) {
            if(textured)
                glTexCoord2f(texCoords[i], texCoords[i+1]);
            glVertex2f(vertices[i], vertices[i+1]);
        }
        glEnd();
    }
}

void PainterOGL1::drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture)
{
    if(!texture->getId())
        return;

    setTexture(texture.get());
    drawCoords(coordsBuffer);
}

void PainterOGL1::drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || !texture->getId())
        return;

    m_coordsBuffer.clear();
    m_coordsBuffer.addQuad(dest, src);

    setTexture(texture.get());
    drawCoords(m_coordsBuffer, TriangleStrip);
}

void PainterOGL1::drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || !texture->getId())
        return;

    m_coordsBuffer.clear();
    m_coordsBuffer.addRepeatedRects(dest, src);

    setTexture(texture.get());
    drawCoords(m_coordsBuffer);
}

void PainterOGL1::drawFilledRect(const Rect& dest)
{
    if(dest.isEmpty())
        return;

    m_coordsBuffer.clear();
    m_coordsBuffer.addRect(dest);

    setTexture(nullptr);
    drawCoords(m_coordsBuffer);
}

void PainterOGL1::drawBoundingRect(const Rect& dest, int innerLineWidth)
{
    if(dest.isEmpty() || innerLineWidth == 0)
        return;

    m_coordsBuffer.clear();
    m_coordsBuffer.addBoudingRect(dest, innerLineWidth);

    setTexture(nullptr);
    drawCoords(m_coordsBuffer);
}

void PainterOGL1::setMatrixMode(PainterOGL1::MatrixMode matrixMode)
{
    if(m_matrixMode == matrixMode)
        return;
    m_matrixMode = matrixMode;
    updateGlMatrixMode();
}

void PainterOGL1::setProjectionMatrix(const Matrix3& projectionMatrix)
{
    m_projectionMatrix = projectionMatrix;
    updateGlProjectionMatrix();
}

void PainterOGL1::setTextureMatrix(const Matrix2& textureMatrix)
{
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

void PainterOGL1::updateGlProjectionMatrix()
{
    float glProjectionMatrix[] = {
        m_projectionMatrix(1,1), m_projectionMatrix(1,2),                    0.0f, m_projectionMatrix(1,3),
        m_projectionMatrix(2,1), m_projectionMatrix(2,2),                    0.0f, m_projectionMatrix(2,3),
                         0.0f,                      0.0f,                    1.0f,                    0.0f,
        m_projectionMatrix(3,1), m_projectionMatrix(3,2),                    0.0f, m_projectionMatrix(3,3),
    };

    setMatrixMode(MatrixProjection);
    glLoadMatrixf(glProjectionMatrix);
}

void PainterOGL1::updateGlTextureMatrix()
{
    float glTextureMatrix[] = {
        m_textureMatrix(1,1), m_textureMatrix(1,2),             0.0f,               0.0f,
        m_textureMatrix(2,1), m_textureMatrix(2,2),             0.0f,               0.0f,
                        0.0f,                 0.0f,             1.0f,               0.0f,
                        0.0f,                 0.0f,             0.0f,               1.0f,
    };

    setMatrixMode(MatrixTexture);
    glLoadMatrixf(glTextureMatrix);
}
