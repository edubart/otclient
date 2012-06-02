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

#include "paintershadermanager.h"
#include "painterogl2.h"
#include "texture.h"
#include "painterogl2_shadersources.h"
#include "paintershaderprogram.h"
#include "shaderprogram.h"
#include "graphics.h"

PainterShaderManager g_shaders;

void PainterShaderManager::init()
{
    m_drawTexturedProgram = createShader();
    assert(m_drawTexturedProgram);
    m_drawTexturedProgram->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
    m_drawTexturedProgram->addShaderFromSourceCode(Shader::Fragment, glslMainFragmentShader + glslTextureSrcFragmentShader);
    m_drawTexturedProgram->link();

    m_drawSolidColorProgram = createShader();
    assert(m_drawSolidColorProgram);
    m_drawSolidColorProgram->addShaderFromSourceCode(Shader::Vertex, glslMainVertexShader + glslPositionOnlyVertexShader);
    m_drawSolidColorProgram->addShaderFromSourceCode(Shader::Fragment, glslMainFragmentShader + glslSolidColorFragmentShader);
    m_drawSolidColorProgram->link();

    PainterShaderProgram::release();
}

void PainterShaderManager::terminate()
{
    m_drawTexturedProgram = nullptr;
    m_drawSolidColorProgram = nullptr;
}

PainterShaderProgramPtr PainterShaderManager::createShader()
{
    if(!g_graphics.canUseShaders())
        return nullptr;
    return PainterShaderProgramPtr(new PainterShaderProgram);
}

PainterShaderProgramPtr PainterShaderManager::createTexturedFragmentShader(const std::string& shaderFile)
{
    PainterShaderProgramPtr shader = createShader();
    if(!shader)
        return nullptr;

    shader->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
    shader->addShaderFromSourceFile(Shader::Fragment, shaderFile);

    if(!shader->link())
        return nullptr;

    return shader;
}
