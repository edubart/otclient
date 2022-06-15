/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#include "pool.h"

void Pool::setCompositionMode(const CompositionMode mode, const int pos)
{
    if (pos == -1) {
        m_state.compositionMode = mode;
        return;
    }

    m_objects[pos - 1].state.compositionMode = mode;
    stdext::hash_combine(m_status.second, mode);
}

void Pool::setBlendEquation(BlendEquation equation, const int pos)
{
    if (pos == -1) {
        m_state.blendEquation = equation;
        return;
    }

    m_objects[pos - 1].state.blendEquation = equation;
    stdext::hash_combine(m_status.second, equation);
}

void Pool::setClipRect(const Rect& clipRect, const int pos)
{
    if (pos == -1) {
        m_state.clipRect = clipRect;
        return;
    }

    m_objects[pos - 1].state.clipRect = clipRect;
    stdext::hash_combine(m_status.second, clipRect.hash());
}

void Pool::setOpacity(const float opacity, const int pos)
{
    if (pos == -1) {
        m_state.opacity = opacity;
        return;
    }

    m_objects[pos - 1].state.opacity = opacity;
    stdext::hash_combine(m_status.second, opacity);
}

void Pool::setShaderProgram(const PainterShaderProgramPtr& shaderProgram, const int pos, const std::function<void()>& action)
{
    const auto& shader = shaderProgram ? shaderProgram.get() : nullptr;

    if (pos == -1) {
        m_state.shaderProgram = shader;
        m_state.action = action;
        return;
    }

    if (shader) {
        m_autoUpdate = true;
    }

    auto& o = m_objects[pos - 1];
    o.state.shaderProgram = shader;
    o.state.action = action;
}

void Pool::resetState()
{
    resetOpacity();
    resetClipRect();
    resetShaderProgram();
    resetBlendEquation();
    resetCompositionMode();

    m_autoUpdate = false;
    m_status.second = 0;
    m_drawingPointer.clear();
}

bool Pool::hasModification(const bool autoUpdateStatus)
{
    const bool hasModification = m_status.first != m_status.second || (m_autoUpdate && m_refreshTime.ticksElapsed() > 50);

    if (hasModification && autoUpdateStatus)
        updateStatus();

    return hasModification;
}
