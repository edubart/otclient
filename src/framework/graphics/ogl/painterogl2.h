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

#ifndef PAINTEROGL2_H
#define PAINTEROGL2_H

#define PAINTER_OGL2

#include "painterogl.h"

/**
 * Painter using OpenGL 2.0 programmable rendering pipeline,
 * compatible with OpenGL ES 2.0. Only recent cards support
 * this painter engine.
 */
class PainterOGL2 : public PainterOGL
{
public:
    PainterOGL2();

    void bind();
    void unbind();

    void drawCoords(CoordsBuffer& coordsBuffer, DrawMode drawMode = Triangles);
    void drawFillCoords(CoordsBuffer& coordsBuffer);
    void drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture);
    void drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawFilledRect(const Rect& dest);
    void drawFilledTriangle(const Point& a, const Point& b, const Point& c);
    void drawBoundingRect(const Rect& dest, int innerLineWidth = 1);

    void setDrawProgram(PainterShaderProgram *drawProgram) { m_drawProgram = drawProgram; }

    bool hasShaders() { return true; }

private:
    PainterShaderProgram *m_drawProgram;
    PainterShaderProgramPtr m_drawTexturedProgram;
    PainterShaderProgramPtr m_drawSolidColorProgram;
};

extern PainterOGL2 *g_painterOGL2;

#endif
