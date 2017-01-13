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

#ifndef PAINTEROGL1_H
#define PAINTEROGL1_H

#define PAINTER_OGL1

#include "painterogl.h"

/**
 * Painter using OpenGL 1.1 fixed-function rendering pipeline,
 * compatible with OpenGL ES 1.0 and intended to be used on
 * older graphics cards. Shaders are not available
 * for this painter engine.
 */
class PainterOGL1 : public PainterOGL
{
public:
    enum MatrixMode {
        MatrixProjection = 0x1701, //GL_PROJECTION
        MatrixTexture = 0x1702, //GL_TEXTURE
        MatrixTransform = 0x1700 // GL_MODELVIEW
    };

    PainterOGL1();

    void bind();
    void unbind();

    void refreshState();

    void drawCoords(CoordsBuffer& coordsBuffer, DrawMode drawMode = Triangles);
    void drawFillCoords(CoordsBuffer& coordsBuffer);
    void drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture);
    void drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawFilledRect(const Rect& dest);
    void drawFilledTriangle(const Point& a, const Point& b, const Point& c);
    void drawBoundingRect(const Rect& dest, int innerLineWidth);

    void setMatrixMode(MatrixMode matrixMode);
    void setTransformMatrix(const Matrix3& projectionMatrix);
    void setProjectionMatrix(const Matrix3& projectionMatrix);
    void setTextureMatrix(const Matrix3& textureMatrix);
    void setColor(const Color& color);
    void setOpacity(float opacity);

    bool hasShaders() { return false; }

private:
    void updateGlColor();
    void updateGlMatrixMode();
    void updateGlProjectionMatrix();
    void updateGlTransformMatrix();
    void updateGlTextureMatrix();
    void updateGlTextureState();

    GLenum m_matrixMode;
    stdext::boolean<false> m_textureEnabled;
};

extern PainterOGL1 *g_painterOGL1;

#endif
