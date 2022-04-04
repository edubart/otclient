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

#include "pool.h"

void Pool::setCompositionMode(const Painter::CompositionMode mode, const int pos)
{
    if (hasFrameBuffer()) {
        stdext::hash_combine(toFramedPool()->m_status.second, mode);
    }

    if (pos == -1) {
        m_state.compositionMode = mode;
        return;
    }

    m_objects[pos - 1].state.compositionMode = mode;
}

void Pool::setClipRect(const Rect& clipRect, const int pos)
{
    if (hasFrameBuffer()) {
        stdext::hash_combine(toFramedPool()->m_status.second, clipRect.hash());
    }

    if (pos == -1) {
        m_state.clipRect = clipRect;
        return;
    }

    m_objects[pos - 1].state.clipRect = clipRect;
}

void Pool::setOpacity(const float opacity, const int pos)
{
    if (hasFrameBuffer()) {
        stdext::hash_combine(toFramedPool()->m_status.second, opacity);
    }

    if (pos == -1) {
        m_state.opacity = opacity;
        return;
    }

    m_objects[pos - 1].state.opacity = opacity;
}

void Pool::setShaderProgram(const PainterShaderProgramPtr& shaderProgram, const int pos)
{
    const auto& shader = shaderProgram ? shaderProgram.get() : nullptr;

    if (hasFrameBuffer() && shader) {
        toFramedPool()->m_autoUpdate = true;
    }

    if (pos == -1) {
        m_state.shaderProgram = shader;
        return;
    }

    m_objects[pos - 1].state.shaderProgram = shader;
}

void Pool::resetState()
{
    resetClipRect();
    resetCompositionMode();
    resetOpacity();
    resetShaderProgram();
    m_indexToStartSearching = 0;

    if (hasFrameBuffer()) {
        toFramedPool()->m_autoUpdate = false;
    }
}

bool FramedPool::hasModification(const bool autoUpdateStatus)
{
    const bool hasModification = m_status.first != m_status.second || (m_autoUpdate && m_refreshTime.ticksElapsed() > 50);

    if (autoUpdateStatus) updateStatus();

    return hasModification;
}
