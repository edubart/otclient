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
#include "texture.h"

class Painter
{
public:
    enum CompositionMode {
        CompositionMode_Normal,
        CompositionMode_Multiply,
        CompositionMode_Add,
        CompositionMode_Replace,
        CompositionMode_DestBlending
    };

    void init();
    void terminate();

    void drawProgram(const PainterShaderProgramPtr& program, CoordsBuffer& coordsBuffer, PainterShaderProgram::DrawMode drawMode = PainterShaderProgram::Triangles);
    void drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture);
    void drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawTexturedRect(const Rect& dest, const TexturePtr& texture) { drawTexturedRect(dest, texture, Rect(Point(0,0), texture->getSize())); }
    void drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawFilledRect(const Rect& dest);
    void drawBoundingRect(const Rect& dest, int innerLineWidth = 1);

    void setColor(const Color& color) { m_color = color; }
    Color getColor() { return m_color; }

    void setOpacity(float opacity) { m_opacity = opacity; }
    float getOpacity() { return m_opacity; }

    void setCustomProgram(PainterShaderProgramPtr program);
    void releaseCustomProgram() { m_customProgram = nullptr; }
    void setCompositionMode(CompositionMode compositionMode);
    void resetCompositionMode() { setCompositionMode(CompositionMode_Normal); }

    void setProjectionMatrix(const Matrix3& projectionMatrix) { m_projectionMatrix = projectionMatrix; }
    Matrix3 getProjectionMatrix() { return m_projectionMatrix; }

    void saveAndResetState();
    void restoreSavedState();

private:
    PainterShaderProgramPtr m_drawTexturedProgram;
    PainterShaderProgramPtr m_drawSolidColorProgram;
    PainterShaderProgramPtr m_customProgram;
    Matrix3 m_projectionMatrix;
    Color m_color;
    float m_opacity;
    CompositionMode m_compostionMode;
    CoordsBuffer m_coordsBuffer;

    PainterShaderProgramPtr m_oldCustomProgram;
    Matrix3 m_oldProjectionMatrix;
    Color m_oldColor;
    float m_oldOpacity;
    CompositionMode m_oldCompostionMode;
};

extern Painter g_painter;

#endif
