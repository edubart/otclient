/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "prerequisites.h"
#include "rect.h"
#include "size.h"
#include "color.h"

class Texture;

class Graphics
{
public:
    Graphics() { }

    void init();
    void terminate();

    /// Check if a GL extension is supported
    bool isExtensionSupported(const char *extension);

    /// Called after every window resize
    void resize(int width, int height);

    /// Restore original viewport
    void restoreViewport();

    /// Called before every render
    void beginRender();

    /// Called after every render
    void endRender();

    const Size& getScreenSize() const { return m_screenSize; }

    void setColor(const Color& color);
    void resetColor();

    // high level rendering
    void drawTexturedRect(const Rect& screenCoords, const Texture *texture, const Rect& texCoords = Rect());
    void drawColoredRect(const Rect& screenCoords, const Color& color);
    void drawBoundingRect(const Rect& screenCoords, const Color& color, int innerLineWidth);

    // lower level rendering
    void _beginTextureRender(const Texture *texture);
    void _drawTexturedRect(const Rect& screenCoords, const Rect& textureCoords, const Size& textureSize);
    void _endTextureRender();

private:
    Size m_screenSize;
};

extern Graphics g_graphics;

#endif // GRAPHICS_H
