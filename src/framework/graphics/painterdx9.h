/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

#ifndef PAINTERDX9_H
#define PAINTERDX9_H

#define PAINTER_DX9

#include "painter.h"

/**
 * Painter using DirectX9 fixed-function rendering pipeline.
 */
class PainterDX9 : public Painter
{
public:
    PainterDX9();

    void bind() {}
    void unbind() {}

    void refreshState() {}

    void drawCoords(CoordsBuffer& coordsBuffer, DrawMode drawMode = Triangles) {}
    void drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture) {}
    void drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src) {}
    void drawUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src) {}
    void drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src) {}
    void drawFilledRect(const Rect& dest) {}
    void drawFilledTriangle(const Point& a, const Point& b, const Point& c) {}
    void drawBoundingRect(const Rect& dest, int innerLineWidth) {}

    bool hasShaders() { return false; }

};

extern PainterDX9 *g_painterDX9;

#endif
