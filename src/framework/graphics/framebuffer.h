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

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "declarations.h"

class FrameBuffer
{
public:
    FrameBuffer(const Size& size);
    virtual ~FrameBuffer();

    void resize(const Size& size);
    void bind(bool clear = true);
    void release();
    void draw(const Rect& dest);

    void setClearColor(const Color& color) { m_clearColor = color; }

    TexturePtr getTexture() { return m_texture; }

private:
    void internalBind();
    void internalRelease();

    TexturePtr m_texture;
    TexturePtr m_screenBackup;
    Matrix3 m_oldProjectionMatrix;
    Size m_oldViewportSize;
    uint m_fbo;
    uint m_prevBoundFbo;
    Color m_clearColor;

    static uint boundFbo;
};

#endif
