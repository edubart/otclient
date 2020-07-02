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

#include "shaderprogram.h"
#include "graphics.h"

#include <framework/core/application.h>

uint ShaderProgram::m_currentProgram = 0;

ShaderProgram::ShaderProgram()
{
    m_linked = false;
    m_programId = glCreateProgram();
    m_uniformLocations.fill(-1);
    if(!m_programId)
        g_logger.fatal("Unable to create GL shader program");
}

ShaderProgram::~ShaderProgram()
{
#ifndef NDEBUG
    assert(!g_app.isTerminated());
#endif
    if(g_graphics.ok())
        glDeleteProgram(m_programId);
}

bool ShaderProgram::addShader(const ShaderPtr& shader) {
    glAttachShader(m_programId, shader->getShaderId());
    m_linked = false;
    m_shaders.push_back(shader);
    return true;
}

bool ShaderProgram::addShaderFromSourceCode(Shader::ShaderType shaderType, const std::string& sourceCode) {
    ShaderPtr shader(new Shader(shaderType));
    if(!shader->compileSourceCode(sourceCode)) {
        g_logger.error(stdext::format("failed to compile shader: %s", shader->log()));
        return false;
    }
    return addShader(shader);
}

bool ShaderProgram::addShaderFromSourceFile(Shader::ShaderType shaderType, const std::string& sourceFile) {
    ShaderPtr shader(new Shader(shaderType));
    if(!shader->compileSourceFile(sourceFile)) {
        g_logger.error(stdext::format("failed to compile shader: %s", shader->log()));
        return false;
    }
    return addShader(shader);
}

void ShaderProgram::removeShader(const ShaderPtr& shader)
{
    auto it = std::find(m_shaders.begin(), m_shaders.end(), shader);
    if(it == m_shaders.end())
        return;

    glDetachShader(m_programId, shader->getShaderId());
    m_shaders.erase(it);
    m_linked = false;
}

void ShaderProgram::removeAllShaders()
{
    while(!m_shaders.empty())
        removeShader(m_shaders.front());
}

bool ShaderProgram::link()
{
    if(m_linked)
        return true;

    glLinkProgram(m_programId);

    int value = GL_FALSE;
    glGetProgramiv(m_programId, GL_LINK_STATUS, &value);
    m_linked = (value != GL_FALSE);

    if(!m_linked)
        g_logger.traceWarning(log());
    return m_linked;
}

bool ShaderProgram::bind()
{
    if(m_currentProgram != m_programId) {
        if(!m_linked && !link())
            return false;
        glUseProgram(m_programId);
        m_currentProgram = m_programId;
    }
    return true;
}

void ShaderProgram::release()
{
    if(m_currentProgram != 0) {
        m_currentProgram = 0;
        glUseProgram(0);
    }
}

std::string ShaderProgram::log()
{
    std::string infoLog;
    int infoLogLength = 0;
    glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 1) {
        std::vector<char> buf(infoLogLength);
        glGetShaderInfoLog(m_programId, infoLogLength-1, nullptr, &buf[0]);
        infoLog = &buf[0];
    }
    return infoLog;
}

int ShaderProgram::getAttributeLocation(const char* name)
{
    return glGetAttribLocation(m_programId, name);
}

void ShaderProgram::bindAttributeLocation(int location, const char* name)
{
    return glBindAttribLocation(m_programId, location, name);
}

void ShaderProgram::bindUniformLocation(int location, const char* name)
{
    assert(m_linked);
    assert(location >= 0 && location < MAX_UNIFORM_LOCATIONS);
    m_uniformLocations[location] = glGetUniformLocation(m_programId, name);
}
