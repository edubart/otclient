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

#include "animatedtexture.h"
#include "graphics.h"

#include <framework/core/eventdispatcher.h>
/*
AnimatedTexture::AnimatedTexture(int width, int height, int channels, int numFrames, uchar *framesPixels, int *framesDelay) :
    Texture(),
    m_numFrames(numFrames)
{
    m_size.resize(width, height);

    m_framesTextureId.resize(numFrames);
    m_framesDelay.resize(numFrames);

    for(int i=0;i<numFrames;++i) {
        uchar *framePixels = framesPixels + (i*height*width* channels);
        uint id = internalLoadGLTexture(framePixels, channels, width, height);

        m_framesTextureId[i] = id;
        m_framesDelay[i] = framesDelay[i];
    }
    m_currentFrame = -1;
    g_dispatcher.scheduleEvent(std::bind(&AnimatedTexture::processAnimation, this), 0);
}

AnimatedTexture::~AnimatedTexture()
{
    glDeleteTextures(m_numFrames, &m_framesTextureId[0]);
    m_id = 0;
}

void AnimatedTexture::enableBilinearFilter()
{
    for(int i=0;i<m_numFrames;++i) {
        glBindTexture(GL_TEXTURE_2D, m_framesTextureId[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

void AnimatedTexture::processAnimation()
{
    m_currentFrame++;
    if(m_currentFrame >= m_numFrames)
        m_currentFrame = 0;
    m_id = m_framesTextureId[m_currentFrame];

    AnimatedTexturePtr self = asAnimatedTexture();

    // continue to animate only if something still referencing this texture
    if(self->ref_count() > 1)
        g_dispatcher.scheduleEvent(std::bind(&AnimatedTexture::processAnimation, self), m_framesDelay[m_currentFrame]);
}
*/