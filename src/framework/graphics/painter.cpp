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

#include "painter.h"
#include "graphics.h"

Painter *g_painter = nullptr;

Painter::Painter()
{
    m_glTextureId = 0;
    m_oldStateIndex = 0;
    m_color = Color::white;
    m_opacity = 1.0f;
    m_compositionMode = CompositionMode_Normal;
    m_shaderProgram = nullptr;
    m_texture = nullptr;
    m_alphaWriting = false;
}

void Painter::resetState()
{
    resetColor();
    resetOpacity();
    resetCompositionMode();
    resetClipRect();
    resetShaderProgram();
    resetTexture();
    resetAlphaWriting();
}

void Painter::refreshState()
{
    updateGlCompositionMode();
    updateGlClipRect();
    updateGlTexture();
    updateGlAlphaWriting();
}

void Painter::saveState()
{
    assert(m_oldStateIndex<10);
    m_olderStates[m_oldStateIndex].projectionMatrix = m_projectionMatrix;
    m_olderStates[m_oldStateIndex].textureMatrix = m_textureMatrix;
    m_olderStates[m_oldStateIndex].color = m_color;
    m_olderStates[m_oldStateIndex].opacity = m_opacity;
    m_olderStates[m_oldStateIndex].compositionMode = m_compositionMode;
    m_olderStates[m_oldStateIndex].clipRect = m_clipRect;
    m_olderStates[m_oldStateIndex].shaderProgram = m_shaderProgram;
    m_olderStates[m_oldStateIndex].texture = m_texture;
    m_olderStates[m_oldStateIndex].alphaWriting = m_alphaWriting;
    m_oldStateIndex++;
}

void Painter::saveAndResetState()
{
    saveState();
    resetState();
}

void Painter::restoreSavedState()
{
    m_oldStateIndex--;
    setProjectionMatrix(m_olderStates[m_oldStateIndex].projectionMatrix);
    setTextureMatrix(m_olderStates[m_oldStateIndex].textureMatrix);
    setColor(m_olderStates[m_oldStateIndex].color);
    setOpacity(m_olderStates[m_oldStateIndex].opacity);
    setCompositionMode(m_olderStates[m_oldStateIndex].compositionMode);
    setClipRect(m_olderStates[m_oldStateIndex].clipRect);
    setShaderProgram(m_olderStates[m_oldStateIndex].shaderProgram);
    setTexture(m_olderStates[m_oldStateIndex].texture);
}

void Painter::clear(const Color& color)
{
    glClearColor(color.rF(), color.gF(), color.bF(), color.aF());
    glClear(GL_COLOR_BUFFER_BIT);
}

void Painter::clearRect(const Color& color, const Rect& rect)
{
    Rect oldClipRect = m_clipRect;
    setClipRect(rect);
    glClearColor(color.rF(), color.gF(), color.bF(), color.aF());
    glClear(GL_COLOR_BUFFER_BIT);
    setClipRect(oldClipRect);
}

void Painter::setCompositionMode(Painter::CompositionMode compositionMode)
{
    if(m_compositionMode == compositionMode)
        return;
    m_compositionMode = compositionMode;
    updateGlCompositionMode();
}

void Painter::setClipRect(const Rect& clipRect)
{
    if(m_clipRect == clipRect)
        return;
    m_clipRect = clipRect;
    updateGlClipRect();
}

void Painter::setTexture(Texture* texture)
{
    if(m_texture == texture)
        return;

    m_texture = texture;

    GLuint glTextureId;
    if(texture) {
        setTextureMatrix(texture->getTransformMatrix());
        glTextureId = texture->getId();
    } else
        glTextureId = 0;

    if(m_glTextureId != glTextureId) {
        m_glTextureId = glTextureId;
        updateGlTexture();
    }
}

void Painter::setAlphaWriting(bool enable)
{
    if(m_alphaWriting == enable)
        return;

    m_alphaWriting = enable;
    updateGlAlphaWriting();
}

void Painter::updateGlTexture()
{
    if(m_glTextureId != 0)
        glBindTexture(GL_TEXTURE_2D, m_glTextureId);
}

void Painter::updateGlCompositionMode()
{
    switch(m_compositionMode) {
        case CompositionMode_Normal:
            if(g_graphics.canUseBlendFuncSeparate())
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
            else {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            break;
        case CompositionMode_Multiply:
            glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case CompositionMode_Add:
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        case CompositionMode_Replace:
            glBlendFunc(GL_ONE, GL_ZERO);
            break;
        case CompositionMode_DestBlending:
            glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
            break;
    }
}

void Painter::updateGlClipRect()
{
    if(m_clipRect.isValid()) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(m_clipRect.left(), g_graphics.getViewportSize().height() - m_clipRect.bottom() - 1, m_clipRect.width(), m_clipRect.height());
    } else {
        glDisable(GL_SCISSOR_TEST);
    }
}

void Painter::updateGlAlphaWriting()
{
    if(m_alphaWriting)
        glColorMask(1,1,1,1);
    else
        glColorMask(1,1,1,0);
}
