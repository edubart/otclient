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


#include <graphics/animatedtexture.h>

#include <GL/gl.h>
#include <core/engine.h>
#include <core/dispatcher.h>

AnimatedTexture::AnimatedTexture(int width, int height, int components, int numFrames, uchar *framesPixels, int *framesDelay) :
    Texture(),
    m_numFrames(numFrames)
{
    m_size.setSize(width, height);

    m_framesTextureId = new uint[numFrames];
    m_framesDelay = new int[numFrames];

    GLenum format = 0;
    switch(components) {
        case 4:
            format = GL_RGBA;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 2:
            format = GL_LUMINANCE_ALPHA;
            break;
        case 1:
            format = GL_LUMINANCE;
            break;
    }

    glGenTextures(numFrames, m_framesTextureId);

    for(int i=0;i<numFrames;++i) {
        glBindTexture(GL_TEXTURE_2D, m_framesTextureId[i]);

        // load the pixels into opengl memory
        uchar *framePixels = framesPixels + (i*height*width*components);
        glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, format, GL_UNSIGNED_BYTE, framePixels);

        // disable texture border
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // nearest filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        m_framesDelay[i] = framesDelay[i];
    }

    m_currentFrame = -1;
    g_dispatcher.scheduleTask(boost::bind(&AnimatedTexture::processAnimation, this), 0);
}

AnimatedTexture::~AnimatedTexture()
{
    glDeleteTextures(m_numFrames, m_framesTextureId);
    delete[] m_framesTextureId;
    delete[] m_framesDelay;
    m_textureId = 0;
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
    m_textureId = m_framesTextureId[m_currentFrame];
    AnimatedTexturePtr me = boost::static_pointer_cast<AnimatedTexture>(shared_from_this());
    if(me.use_count() > 1)
        g_dispatcher.scheduleTask(boost::bind(&AnimatedTexture::processAnimation, me), m_framesDelay[m_currentFrame]);
}
