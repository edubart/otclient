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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "declarations.h"

class Graphics
{
public:
    /// Initialize default OpenGL states
    void init();

    /// Termiante graphics
    void terminate();

    /// Check if a GL extension is supported
    bool isExtensionSupported(const char *extension);

    /// Resizes OpenGL viewport
    void resize(const Size& size);

    /// Restore original viewport
    void restoreViewport();

    /// Called before every render
    void beginRender();

    /// Called after every render
    void endRender();

    void bindColor(const Color& color);
    void bindTexture(const TexturePtr& texture);
    void bindBlendFunc(Fw::BlendFunc blendType);

    // drawing API
    void drawTexturedRect(const Rect& screenCoords,
                          const TexturePtr& texture,
                          const Rect& textureCoords = Rect(),
                          bool upsideDown = false);

    void drawRepeatedTexturedRect(const Rect& screenCoords,
                                  const TexturePtr& texture,
                                  const Rect& textureCoords);

    void drawFilledRect(const Rect& screenCoords);

    void drawBoundingRect(const Rect& screenCoords,
                          int innerLineWidth = 1);

    const Size& getScreenSize() const { return m_screenSize; }

    void startDrawing();
    void stopDrawing();
    bool isDrawing() const { return m_drawing; }

    int getOpacity() const { return m_opacity; }
    void setOpacity(int opacity) { m_opacity = opacity; }
    TexturePtr getEmptyTexture() { return m_emptyTexture; }

private:
    bool m_drawing;
    int m_opacity;
    Size m_screenSize;
    TexturePtr m_emptyTexture;
};

extern Graphics g_graphics;

#endif
