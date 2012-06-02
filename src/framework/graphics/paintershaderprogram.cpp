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

#include "paintershaderprogram.h"
#include "painter.h"
#include "texture.h"
#include "texturemanager.h"
#include <framework/core/clock.h>

PainterShaderProgram::PainterShaderProgram()
{
    m_startTime = g_clock.seconds();
    m_opacity = 1;
    m_color = Color::white;
    m_time = 0;
}

bool PainterShaderProgram::link()
{
    m_startTime = g_clock.seconds();
    bindAttributeLocation(VERTEX_ATTR, "a_vertex");
    bindAttributeLocation(TEXCOORD_ATTR, "a_texCoord");
    if(ShaderProgram::link()) {
        bindUniformLocation(PROJECTION_MATRIX_UNIFORM, "projectionMatrix");
        bindUniformLocation(TEXTURE_MATRIX_UNIFORM, "textureMatrix");
        bindUniformLocation(COLOR_UNIFORM, "color");
        bindUniformLocation(OPACITY_UNIFORM, "opacity");
        bindUniformLocation(TIME_UNIFORM, "time");
        bindUniformLocation(TEX0_UNIFORM, "tex0");
        bindUniformLocation(TEX1_UNIFORM, "tex1");

        bind();
        setUniformValue(PROJECTION_MATRIX_UNIFORM, m_projectionMatrix);
        setUniformValue(TEXTURE_MATRIX_UNIFORM, m_textureMatrix);
        setUniformValue(COLOR_UNIFORM, m_color);
        setUniformValue(OPACITY_UNIFORM, m_opacity);
        setUniformValue(TIME_UNIFORM, m_time);
        setUniformValue(TEX0_UNIFORM, 0);
        setUniformValue(TEX1_UNIFORM, 1);
        release();
        return true;
    }
    return false;
}

void PainterShaderProgram::setProjectionMatrix(const Matrix3& projectionMatrix)
{
    if(projectionMatrix == m_projectionMatrix)
        return;

    bind();
    setUniformValue(PROJECTION_MATRIX_UNIFORM, projectionMatrix);
    m_projectionMatrix = projectionMatrix;
}

void PainterShaderProgram::setTextureMatrix(const Matrix3& textureMatrix)
{
    if(textureMatrix == m_textureMatrix)
        return;

    bind();
    setUniformValue(TEXTURE_MATRIX_UNIFORM, textureMatrix);
    m_textureMatrix = textureMatrix;
}

void PainterShaderProgram::setColor(const Color& color)
{
    if(color == m_color)
        return;

    bind();
    setUniformValue(COLOR_UNIFORM, color);
    m_color = color;
}

void PainterShaderProgram::setOpacity(float opacity)
{
    if(m_opacity == opacity)
        return;

    bind();
    setUniformValue(OPACITY_UNIFORM, opacity);
    m_opacity = opacity;
}

void PainterShaderProgram::updateTime()
{
    float time = g_clock.seconds() - m_startTime;
    if(m_time == time)
        return;

    bind();
    setUniformValue(TIME_UNIFORM, time);
    m_time = time;
}
