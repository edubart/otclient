#include "animatedtexture.h"
#include "graphics.h"

#include <framework/platform/platform.h>
#include <framework/core/eventdispatcher.h>

#include <GL/gl.h>

AnimatedTexture::AnimatedTexture(int width, int height, int channels, int numFrames, uchar *framesPixels, int *framesDelay) :
    Texture(),
    m_numFrames(numFrames)
{
    m_size.setSize(width, height);

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

    AnimatedTexturePtr self = asAnimatedTexture();

    // continue to animate only if something still referencing this texture
    if(self.use_count() > 1)
        g_dispatcher.scheduleEvent(std::bind(&AnimatedTexture::processAnimation, self), m_framesDelay[m_currentFrame]);
}
