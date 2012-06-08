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

#ifndef ANIMATEDTEXTURE_H
#define ANIMATEDTEXTURE_H

#include "texture.h"
/*
class AnimatedTexture : public Texture
{
public:
    AnimatedTexture(int width, int height, int channels, int numFrames, uchar *framesPixels, int *framesDelay);
    virtual ~AnimatedTexture();

    void enableBilinearFilter();
    void processAnimation();

    AnimatedTexturePtr asAnimatedTexture() { return std::static_pointer_cast<AnimatedTexture>(shared_from_this()); }

private:
    std::vector<uint> m_framesTextureId;
    std::vector<int> m_framesDelay;
    int m_numFrames;
    int m_currentFrame;
    ticks_t m_lastAnimCheckTicks;
};
*/
#endif
