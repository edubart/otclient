/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

AnimatedTexture::AnimatedTexture(const Size& size, std::vector<ImagePtr> frames, std::vector<int> framesDelay, bool buildMipmaps, bool compress)
{
    if(!setupSize(size, buildMipmaps))
        return;

    for(const auto &frame: frames) {
        m_frames.push_back(new Texture(frame, buildMipmaps, compress));
    }

    m_framesDelay = framesDelay;
    m_hasMipmaps = buildMipmaps;
    m_id = m_frames[0]->getId();
    m_currentFrame = 0;
    m_animTimer.restart();
}

AnimatedTexture::~AnimatedTexture()
{

}

bool AnimatedTexture::buildHardwareMipmaps()
{
    if(!g_graphics.canUseHardwareMipmaps())
        return false;
    for(const TexturePtr& frame : m_frames)
        frame->buildHardwareMipmaps();
    m_hasMipmaps = true;
    return true;
}

void AnimatedTexture::setSmooth(bool smooth)
{
    for(const TexturePtr& frame : m_frames)
        frame->setSmooth(smooth);
    m_smooth = smooth;
}

void AnimatedTexture::setRepeat(bool repeat)
{
    for(const TexturePtr& frame : m_frames)
        frame->setRepeat(repeat);
    m_repeat = repeat;
}

void AnimatedTexture::updateAnimation()
{
    if(m_animTimer.ticksElapsed() < m_framesDelay[m_currentFrame])
        return;

    m_animTimer.restart();
    m_currentFrame++;
    if(m_currentFrame >= m_frames.size())
        m_currentFrame = 0;
    m_id = m_frames[m_currentFrame]->getId();
}
