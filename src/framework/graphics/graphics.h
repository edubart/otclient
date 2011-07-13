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

#include <global.h>
#include <graphics/textures.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

class Graphics
{
    enum DrawMode {
        DRAW_NONE = 0,
        DRAW_QUADS = 1,
        DRAW_TEXTURE = 2,
        DRAW_COLORED = 4,
        DRAW_COLOR_QUADS = DRAW_QUADS | DRAW_COLORED,
        DRAW_TEXTURE_QUADS = DRAW_QUADS | DRAW_TEXTURE | DRAW_COLORED
    };

public:
    Graphics() : m_drawMode(DRAW_NONE) { }

    /// Initialize graphics
    void init();

    /// Termiante graphics
    void terminate();

    /// Check if a GL extension is supported
    bool isExtensionSupported(const char *extension);

    /// Called after every window resize
    void resize(const Size& size);

    /// Restore original viewport
    void restoreViewport();

    /// Called before every render
    void beginRender();

    /// Called after every render
    void endRender();

    const Size& getScreenSize() const { return m_screenSize; }

    void disableDrawing();
    void enableDrawing();

    void drawTexturedRect(const Rect& screenCoords, const TexturePtr& texture, const Rect& textureCoords = Rect(), const Color& color = Color::white);
    void drawRepeatedTexturedRect(const Rect& screenCoords, const TexturePtr& texture, const Rect& textureCoords, const Color& color = Color::white);
    void drawFilledRect(const Rect& screenCoords, const Color& color);
    void drawBoundingRect(const Rect& screenCoords, const Color& color = Color::green, int innerLineWidth = 1);

private:
    void bindTexture(const TexturePtr& texture, const Color& color = Color::white);
    void bindColor(const Color& color);

    TexturePtr m_bindedTexture;
    Color m_bindedColor;
    Size m_screenSize;
    DrawMode m_drawMode;
};

extern Graphics g_graphics;

#endif // GRAPHICS_H
