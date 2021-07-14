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

#include "framebuffer.h"
#include "graphics.h"
#include "texture.h"

#include <framework/core/eventdispatcher.h>
#include <framework/platform/platformwindow.h>
#include <framework/core/application.h>
#include <framework/graphics/drawpool.h>

uint FrameBuffer::boundFbo = 0;

FrameBuffer::FrameBuffer(const bool useAlphaWriting)
{
    internalCreate();
    m_useAlphaWriting = useAlphaWriting;
}

void FrameBuffer::internalCreate()
{
    m_prevBoundFbo = 0;
    m_fbo = 0;
    if(g_graphics.canUseFBO()) {
        glGenFramebuffers(1, &m_fbo);
        if(!m_fbo)
            g_logger.fatal("Unable to create framebuffer object");
    }
}

FrameBuffer::~FrameBuffer()
{
#ifndef NDEBUG
    assert(!g_app.isTerminated());
#endif
    if(g_graphics.ok() && m_fbo != 0)
        glDeleteFramebuffers(1, &m_fbo);
}

void FrameBuffer::clear(const Color color)
{
    if(m_useAlphaWriting) {
        g_painter->clear(Color::alpha);
    } else {
        g_painter->setColor(color);
        g_painter->drawFilledRect(Rect(0, 0, getSize()));
        g_painter->resetColor();
    }
}

void FrameBuffer::resize(const Size& size)
{
    assert(size.isValid());

    if(m_texture && m_texture->getSize() == size)
        return;

    m_texture = TexturePtr(new Texture(size));
    m_texture->setSmooth(m_smooth);
    m_texture->setUpsideDown(true);

    if(m_fbo) {
        internalBind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getId(), 0);

        const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status != GL_FRAMEBUFFER_COMPLETE)
            g_logger.fatal("Unable to setup framebuffer object");
        internalRelease();
    } else {
        if(m_backuping) {
            m_screenBackup = TexturePtr(new Texture(size));
            m_screenBackup->setUpsideDown(true);
        }
    }
}

void FrameBuffer::bind(const bool autoClear)
{
    g_painter->saveAndResetState();
    internalBind();
    g_painter->setResolution(m_texture->getSize());
    g_painter->setAlphaWriting(m_useAlphaWriting);

    if(autoClear) clear(m_colorClear);
}

void FrameBuffer::release()
{
    internalRelease();
    g_painter->restoreSavedState();
}

void FrameBuffer::draw(const Rect& dest, const Rect& src)
{
    Rect _dest(0, 0, getSize()), _src = _dest;
    if(dest.isValid()) _dest = dest;
    if(src.isValid()) _src = src;

    if(m_disableBlend) glDisable(GL_BLEND);
    g_painter->setCompositionMode(m_compositeMode);
    g_painter->drawTexturedRect(_dest, m_texture, _src);
    g_painter->resetCompositionMode();
    if(m_disableBlend) glEnable(GL_BLEND);
}

void FrameBuffer::internalBind()
{
    if(m_fbo) {
        assert(boundFbo != m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        m_prevBoundFbo = boundFbo;
        boundFbo = m_fbo;
    } else if(m_backuping) {
        // backup screen color buffer into a texture
        m_screenBackup->copyFromScreen(Rect(0, 0, getSize()));
    }
}

void FrameBuffer::internalRelease()
{
    if(m_fbo) {
        assert(boundFbo == m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_prevBoundFbo);
        boundFbo = m_prevBoundFbo;
    } else {
        Rect screenRect(0, 0, getSize());

        // copy the drawn color buffer into the framebuffer texture
        m_texture->copyFromScreen(screenRect);

        // restore screen original content
        if(m_backuping) {
            glDisable(GL_BLEND);
            g_painter->resetColor();
            g_painter->drawTexturedRect(screenRect, m_screenBackup, screenRect);
            glEnable(GL_BLEND);
        }
    }
}

Size FrameBuffer::getSize()
{
    if(m_fbo == 0) {
        // the buffer size is limited by the window size
        return Size(std::min<int>(m_texture->getWidth(), g_window.getWidth()),
                    std::min<int>(m_texture->getHeight(), g_window.getHeight()));
    }

    return m_texture->getSize();
}

size_t FrameBuffer::updateHash(const TexturePtr& texture, const DrawMethod& method)
{
    const auto& currentState = g_painter->getCurrentState();

    size_t hash = 0;

    if(texture)
        boost::hash_combine(hash, HASH_INT(texture->getId()));

    if(currentState.opacity < 1.f)
        boost::hash_combine(hash, HASH_INT(currentState.opacity));

    if(currentState.color != Color::white)
        boost::hash_combine(hash, HASH_INT(currentState.color.rgba()));

    if(currentState.compositionMode != Painter::CompositionMode_Normal)
        boost::hash_combine(hash, HASH_INT(currentState.compositionMode));

    if(currentState.shaderProgram)
        boost::hash_combine(hash, HASH_INT(currentState.shaderProgram->getProgramId()));

    if(currentState.clipRect.isValid()) {
        boost::hash_combine(hash, HASH_INT(currentState.clipRect.x()));
        boost::hash_combine(hash, HASH_INT(currentState.clipRect.y()));
    }

    if(method.rects.first.isValid()) {
        boost::hash_combine(hash, HASH_INT(method.rects.first.x()));
        boost::hash_combine(hash, HASH_INT(method.rects.first.y()));
    }
    if(method.rects.second.isValid()) {
        boost::hash_combine(hash, HASH_INT(method.rects.second.x()));
        boost::hash_combine(hash, HASH_INT(method.rects.second.y()));
    }

    const auto& a = std::get<0>(method.points),
        b = std::get<1>(method.points),
        c = std::get<2>(method.points);

    if(!a.isNull()) {
        boost::hash_combine(hash, HASH_INT(a.x));
        boost::hash_combine(hash, HASH_INT(a.y));
    }
    if(!b.isNull()) {
        boost::hash_combine(hash, HASH_INT(b.x));
        boost::hash_combine(hash, HASH_INT(b.y));
    }
    if(!c.isNull()) {
        boost::hash_combine(hash, HASH_INT(c.x));
        boost::hash_combine(hash, HASH_INT(c.y));
    }

    if(method.intValue != 0)
        boost::hash_combine(hash, HASH_INT(method.intValue));

    if(method.floatValue != 0)
        boost::hash_combine(hash, HASH_FLOAT(method.floatValue));

    boost::hash_combine(m_status.second, hash);

    return hash;
}
