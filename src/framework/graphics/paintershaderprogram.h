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

#ifndef PAINTERSHADER_H
#define PAINTERSHADER_H

#include "shaderprogram.h"

class PainterShaderProgram : public ShaderProgram
{
public:
    enum {
        VERTEX_COORDS_ATTR = 0,
        TEXTURE_COORDS_ATTR = 1,
        PROJECTION_MATRIX_UNIFORM = 0,
        TEXTURE_TRANSFORM_MATRIX_UNIFORM = 1,
        COLOR_UNIFORM = 2,
        OPACITY_UNIFORM = 3,
        TEXTURE_UNIFORM = 4
    };

    void setProjectionMatrix(GLfloat projectionMatrix[3][3]);
    void setColor(const Color& color);
    void setOpacity(GLfloat opacity);
    void setTexture(const TexturePtr& texture);
    void setVertexCoords(const GLfloat *vertices);
    void setTextureCoords(const GLfloat *textureCoords);

    void prepareForDraw();
    void drawTriangleStrip(int numVertices);
    void drawTriangles(int numVertices);
    void releaseFromDraw();

private:
    Boolean<false> m_mustDisableVertexArray;
    Boolean<false> m_mustDisableTexCoordsArray;
};

#endif
