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

#include "shader.h"
#include "graphics.h"

#include <framework/core/application.h>
#include <framework/core/resourcemanager.h>

Shader::Shader(Shader::ShaderType shaderType)
{
    m_shaderType = shaderType;
    switch(shaderType) {
        case Vertex:
            m_shaderId = glCreateShader(GL_VERTEX_SHADER);
            break;
        case Fragment:
            m_shaderId = glCreateShader(GL_FRAGMENT_SHADER);
            break;
    }

    if(!m_shaderId)
        g_logger.fatal("Unable to create GL shader");
}

Shader::~Shader()
{
#ifndef NDEBUG
    assert(!g_app.isTerminated());
#endif
    if(g_graphics.ok())
        glDeleteShader(m_shaderId);
}

bool Shader::compileSourceCode(const std::string& sourceCode)
{
#ifndef OPENGL_ES
    static const char *qualifierDefines =
        "#define lowp\n"
        "#define mediump\n"
        "#define highp\n";
#else
    static const char *qualifierDefines =
        "#ifndef GL_FRAGMENT_PRECISION_HIGH\n"
        "#define highp mediump\n"
        "#endif\n"
        "precision highp float;\n";
#endif

    std::string code = qualifierDefines;
    code.append(sourceCode);
    const char *c_source = code.c_str();
    glShaderSource(m_shaderId, 1, &c_source, NULL);
    glCompileShader(m_shaderId);

    int res = GL_FALSE;
    glGetShaderiv(m_shaderId, GL_COMPILE_STATUS, &res);
    return (res == GL_TRUE);
}

bool Shader::compileSourceFile(const std::string& sourceFile)
{
    try {
        std::string sourceCode = g_resources.readFileContents(sourceFile);
        return compileSourceCode(sourceCode);
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("unable to load shader source form file '%s': %s", sourceFile, e.what()));
    }
    return false;
}

std::string Shader::log()
{
    std::string infoLog;
    int infoLogLength = 0;
    glGetShaderiv(m_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 1) {
        std::vector<char> buf(infoLogLength);
        glGetShaderInfoLog(m_shaderId, infoLogLength-1, NULL, &buf[0]);
        infoLog = &buf[0];
    }
    return infoLog;
}
