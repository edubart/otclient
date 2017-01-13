/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#include "painterogl.h"
#include <framework/graphics/graphics.h>
#include <framework/platform/platformwindow.h>

PainterOGL::PainterOGL()
{
    m_glTextureId = 0;
    m_oldStateIndex = 0;
    m_color = Color::white;
    m_opacity = 1.0f;
    m_compositionMode = CompositionMode_Normal;
    m_blendEquation = BlendEquation_Add;
    m_shaderProgram = nullptr;
    m_texture = nullptr;
    m_alphaWriting = false;
    setResolution(g_window.getSize());
}

void PainterOGL::resetState()
{
    resetColor();
    resetOpacity();
    resetCompositionMode();
    resetBlendEquation();
    resetClipRect();
    resetShaderProgram();
    resetTexture();
    resetAlphaWriting();
    resetTransformMatrix();
}

void PainterOGL::refreshState()
{
    updateGlViewport();
    updateGlCompositionMode();
    updateGlBlendEquation();
    updateGlClipRect();
    updateGlTexture();
    updateGlAlphaWriting();
}

void PainterOGL::saveState()
{
    assert(m_oldStateIndex<10);
    m_olderStates[m_oldStateIndex].resolution = m_resolution;
    m_olderStates[m_oldStateIndex].transformMatrix = m_transformMatrix;
    m_olderStates[m_oldStateIndex].projectionMatrix = m_projectionMatrix;
    m_olderStates[m_oldStateIndex].textureMatrix = m_textureMatrix;
    m_olderStates[m_oldStateIndex].color = m_color;
    m_olderStates[m_oldStateIndex].opacity = m_opacity;
    m_olderStates[m_oldStateIndex].compositionMode = m_compositionMode;
    m_olderStates[m_oldStateIndex].blendEquation = m_blendEquation;
    m_olderStates[m_oldStateIndex].clipRect = m_clipRect;
    m_olderStates[m_oldStateIndex].shaderProgram = m_shaderProgram;
    m_olderStates[m_oldStateIndex].texture = m_texture;
    m_olderStates[m_oldStateIndex].alphaWriting = m_alphaWriting;
    m_oldStateIndex++;
}

void PainterOGL::saveAndResetState()
{
    saveState();
    resetState();
}

void PainterOGL::restoreSavedState()
{
    m_oldStateIndex--;
    setResolution(m_olderStates[m_oldStateIndex].resolution);
    setTransformMatrix(m_olderStates[m_oldStateIndex].transformMatrix);
    setProjectionMatrix(m_olderStates[m_oldStateIndex].projectionMatrix);
    setTextureMatrix(m_olderStates[m_oldStateIndex].textureMatrix);
    setColor(m_olderStates[m_oldStateIndex].color);
    setOpacity(m_olderStates[m_oldStateIndex].opacity);
    setCompositionMode(m_olderStates[m_oldStateIndex].compositionMode);
    setBlendEquation(m_olderStates[m_oldStateIndex].blendEquation);
    setClipRect(m_olderStates[m_oldStateIndex].clipRect);
    setShaderProgram(m_olderStates[m_oldStateIndex].shaderProgram);
    setTexture(m_olderStates[m_oldStateIndex].texture);
    setAlphaWriting(m_olderStates[m_oldStateIndex].alphaWriting);
}

void PainterOGL::clear(const Color& color)
{
    glClearColor(color.rF(), color.gF(), color.bF(), color.aF());
    glClear(GL_COLOR_BUFFER_BIT);
}

void PainterOGL::clearRect(const Color& color, const Rect& rect)
{
    Rect oldClipRect = m_clipRect;
    setClipRect(rect);
    glClearColor(color.rF(), color.gF(), color.bF(), color.aF());
    glClear(GL_COLOR_BUFFER_BIT);
    setClipRect(oldClipRect);
}

void PainterOGL::setCompositionMode(Painter::CompositionMode compositionMode)
{
    if(m_compositionMode == compositionMode)
        return;
    m_compositionMode = compositionMode;
    updateGlCompositionMode();
}

void PainterOGL::setBlendEquation(Painter::BlendEquation blendEquation)
{
    if(m_blendEquation == blendEquation)
        return;
    m_blendEquation = blendEquation;
    updateGlBlendEquation();
}

void PainterOGL::setClipRect(const Rect& clipRect)
{
    if(m_clipRect == clipRect)
        return;
    m_clipRect = clipRect;
    updateGlClipRect();
}

void PainterOGL::setTexture(Texture* texture)
{
    if(m_texture == texture)
        return;

    m_texture = texture;

    uint glTextureId;
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

void PainterOGL::setAlphaWriting(bool enable)
{
    if(m_alphaWriting == enable)
        return;

    m_alphaWriting = enable;
    updateGlAlphaWriting();
}

void PainterOGL::setResolution(const Size& resolution)
{
    // The projection matrix converts from Painter's coordinate system to GL's coordinate system
    //    * GL's viewport is 2x2, Painter's is width x height
    //    * GL has +y -> -y going from bottom -> top, Painter is the other way round
    //    * GL has [0,0] in the center, Painter has it in the top-left
    //
    // This results in the Projection matrix below.
    //
    //                                    Projection Matrix
    //   Painter Coord     ------------------------------------------------        GL Coord
    //   -------------     | 2.0 / width  |      0.0      |      0.0      |     ---------------
    //   |  x  y  1  |  *  |     0.0      | -2.0 / height |      0.0      |  =  |  x'  y'  1  |
    //   -------------     |    -1.0      |      1.0      |      1.0      |     ---------------

    Matrix3 projectionMatrix = { 2.0f/resolution.width(),  0.0f,                      0.0f,
                                 0.0f,                    -2.0f/resolution.height(),  0.0f,
                                -1.0f,                     1.0f,                      1.0f };

    m_resolution = resolution;

    setProjectionMatrix(projectionMatrix);
    if(g_painter == this)
        updateGlViewport();
}

void PainterOGL::scale(float x, float y)
{
    Matrix3 scaleMatrix = {
           x,  0.0f,  0.0f,
        0.0f,     y,  0.0f,
        0.0f,  0.0f,  1.0f
    };

    setTransformMatrix(m_transformMatrix * scaleMatrix.transposed());
}

void PainterOGL::translate(float x, float y)
{
    Matrix3 translateMatrix = {
        1.0f,  0.0f,     x,
        0.0f,  1.0f,     y,
        0.0f,  0.0f,  1.0f
    };

    setTransformMatrix(m_transformMatrix * translateMatrix.transposed());
}

void PainterOGL::rotate(float angle)
{
    Matrix3 rotationMatrix = {
        std::cos(angle), -std::sin(angle),  0.0f,
        std::sin(angle),  std::cos(angle),  0.0f,
                   0.0f,             0.0f,  1.0f
    };

    setTransformMatrix(m_transformMatrix * rotationMatrix.transposed());
}

void PainterOGL::rotate(float x, float y, float angle)
{
    translate(-x, -y);
    rotate(angle);
    translate(x, y);
}

void PainterOGL::pushTransformMatrix()
{
    m_transformMatrixStack.push_back(m_transformMatrix);
    assert(m_transformMatrixStack.size() < 100);
}

void PainterOGL::popTransformMatrix()
{
    assert(m_transformMatrixStack.size() > 0);
    setTransformMatrix(m_transformMatrixStack.back());
    m_transformMatrixStack.pop_back();
}

void PainterOGL::updateGlTexture()
{
    if(m_glTextureId != 0)
        glBindTexture(GL_TEXTURE_2D, m_glTextureId);
}

void PainterOGL::updateGlCompositionMode()
{
    switch(m_compositionMode) {
        case CompositionMode_Normal:
            if(g_graphics.canUseBlendFuncSeparate())
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
            else
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
        case CompositionMode_Light:
            glBlendFunc(GL_ZERO, GL_SRC_COLOR);
            break;
    }
}

void PainterOGL::updateGlBlendEquation()
{
    if(!g_graphics.canUseBlendEquation())
        return;
    if(m_blendEquation == BlendEquation_Add)
        glBlendEquation(0x8006); // GL_FUNC_ADD
    else if(m_blendEquation == BlendEquation_Max)
        glBlendEquation(0x8008); // GL_MAX
}

void PainterOGL::updateGlClipRect()
{
    if(m_clipRect.isValid()) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(m_clipRect.left(), m_resolution.height() - m_clipRect.bottom() - 1, m_clipRect.width(), m_clipRect.height());
    } else {
        glScissor(0, 0, m_resolution.width(), m_resolution.height());
        glDisable(GL_SCISSOR_TEST);
    }
}

void PainterOGL::updateGlAlphaWriting()
{
    if(m_alphaWriting)
        glColorMask(1,1,1,1);
    else
        glColorMask(1,1,1,0);
}

void PainterOGL::updateGlViewport()
{
    glViewport(0, 0, m_resolution.width(), m_resolution.height());
}
