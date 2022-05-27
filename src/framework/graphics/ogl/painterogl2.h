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

#pragma once

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

    void bind() override;
    void unbind() override;

    void drawCoords(CoordsBuffer& coordsBuffer, DrawMode drawMode = DrawMode::Triangles) override;

    void setDrawProgram(PainterShaderProgram* drawProgram) { m_drawProgram = drawProgram; }

    bool hasShaders() override { return true; }

private:
    PainterShaderProgram* m_drawProgram;
    PainterShaderProgramPtr m_drawTexturedProgram;
    PainterShaderProgramPtr m_drawSolidColorProgram;
};

extern PainterOGL2* g_painterOGL2;
