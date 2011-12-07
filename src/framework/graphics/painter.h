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

#ifndef PAINTER_H
#define PAINTER_H

#include "declarations.h"
#include <framework/util/databuffer.h>
#include "coordsbuffer.h"

class Painter
{
public:
    enum CompositionMode {
        CompositionMode_SourceOver,
        CompositionMode_ColorizeDest
    };

    void init();
    void terminate();

    void updateProjectionMatrix(const Size& viewportSize, bool inverseYAxis = false);

    void drawCoords(CoordsBuffer& coordsBuffer);
    void drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture);

    void drawTexturedRect(const Rect& dest, const TexturePtr& texture);
    void drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void drawFilledRect(const Rect& dest);
    void drawBoundingRect(const Rect& dest, int innerLineWidth = 1);

    void setColor(const Color& color) { m_currentColor = color; }
    Color getColor() { return m_currentColor; }

    void setOpacity(int opacity) { m_currentOpacity = opacity / 255.0f; }
    int getOpacity() { return m_currentOpacity * 255.0f; }

    void setCompositionMode(CompositionMode compositionMode);

private:
    PainterShaderProgramPtr m_drawTexturedProgram;
    PainterShaderProgramPtr m_drawSolidColorProgram;
    GLfloat m_projectionMatrix[3][3];
    Color m_currentColor;
    GLfloat m_currentOpacity;
    CoordsBuffer m_coordsBuffer;
};

extern Painter g_painter;

#endif
