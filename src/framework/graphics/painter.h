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

#ifndef PAINTER_H
#define PAINTER_H

#include "declarations.h"
#include <framework/util/databuffer.h>
#include "coordsbuffer.h"
#include "paintershaderprogram.h"

class Painter
{
public:
    enum CompositionMode {
        CompositionMode_Normal,
        CompositionMode_Multiply,
        CompositionMode_Add
    };

    void init();
    void terminate();

    void drawProgram(const PainterShaderProgramPtr& program, CoordsBuffer& coordsBuffer, PainterShaderProgram::DrawMode drawMode = PainterShaderProgram::Triangles);
    void drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture);
    void drawTexturedRect(const Rect& dest, const TexturePtr& texture);
    void drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawFilledRect(const Rect& dest);
    void drawBoundingRect(const Rect& dest, int innerLineWidth = 1);

    void setColor(const Color& color) { m_currentColor = color; }
    Color getColor() { return m_currentColor; }

    void setOpacity(float opacity) { m_currentOpacity = opacity; }
    float getOpacity() { return m_currentOpacity; }

    void setCustomProgram(PainterShaderProgramPtr program);
    void releaseCustomProgram() { m_customProgram = nullptr; }
    void setCompositionMode(CompositionMode compositionMode);
    void resetCompositionMode() { setCompositionMode(CompositionMode_Normal); }

    void setProjectionMatrix(const Matrix3& projectionMatrix) { m_projectionMatrix = projectionMatrix; }
    Matrix3 getProjectionMatrix() { return m_projectionMatrix; }

private:
    PainterShaderProgramPtr m_drawTexturedProgram;
    PainterShaderProgramPtr m_drawSolidColorProgram;
    PainterShaderProgramPtr m_customProgram;
    Matrix3 m_projectionMatrix;
    Color m_currentColor;
    float m_currentOpacity;
    CoordsBuffer m_coordsBuffer;
};

extern Painter g_painter;

#endif
