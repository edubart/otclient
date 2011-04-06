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


#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "prerequisites.h"

class FrameBuffer
{
public:
    FrameBuffer(int width, int height);
    virtual ~FrameBuffer();

    /// Bind the framebuffer, everything rendered will be draw on it
    void bind();

    /// Unbind the framebuffer, render on back buffer again
    void unbind();

    /// Draw framebuffer
    void draw(int x, int y, int width, int height);

private:
    uint m_fboTexture;
    uint m_fbo;
    bool m_fallbackOldImp;
    int m_width;
    int m_height;
};

#endif // FRAMEBUFFER_H
