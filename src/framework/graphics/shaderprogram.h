/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "shader.h"

class ShaderProgram
{
    enum {
        MAX_UNIFORM_LOCATIONS = 10
    };
public:
    ShaderProgram();
    ~ShaderProgram();

    bool addShader(const ShaderPtr& shader);
    bool addShaderFromSourceCode(Shader::ShaderType shaderType, const std::string& sourceCode);
    bool addShaderFromSourceFile(Shader::ShaderType shaderType, const std::string& sourceFile);
    void removeShader(const ShaderPtr& shader);
    void removeAllShaders();
    virtual bool link();
    bool bind();
    void release();
    std::string log();

    void disableAttributeArray(int location) { glDisableVertexAttribArray(location); }
    void enableAttributeArray(int location) { glEnableVertexAttribArray(location); }
    void disableAttributeArray(const char *name) { glDisableVertexAttribArray(getAttributeLocation(name)); }
    void enableAttributeArray(const char *name) { glEnableVertexAttribArray(getAttributeLocation(name)); }

    int getAttributeLocation(const char *name);
    void bindAttributeLocation(int location, const char *name);
    void bindUniformLocation(int location, const char *name);

    void setAttributeArray(int location, const GLfloat *values, int size, int stride = 0) { glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride, values); }
    void setAttributeValue(int location, GLfloat value) { glVertexAttrib1f(location, value); }
    void setAttributeValue(int location, GLfloat x, GLfloat y) { glVertexAttrib2f(location, x, y); }
    void setAttributeValue(int location, GLfloat x, GLfloat y, GLfloat z) { glVertexAttrib3f(location, x, y, z); }
    void setAttributeArray(const char *name, const GLfloat *values, int size, int stride = 0) { glVertexAttribPointer(getAttributeLocation(name), size, GL_FLOAT, GL_FALSE, stride, values); }
    void setAttributeValue(const char *name, GLfloat value) { glVertexAttrib1f(getAttributeLocation(name), value); }
    void setAttributeValue(const char *name, GLfloat x, GLfloat y) { glVertexAttrib2f(getAttributeLocation(name), x, y); }
    void setAttributeValue(const char *name, GLfloat x, GLfloat y, GLfloat z) { glVertexAttrib3f(getAttributeLocation(name), x, y, z); }

    void setUniformValue(int location, GLint value) { glUniform1i(m_uniformLocations[location], value); }
    void setUniformValue(int location, GLfloat value) { glUniform1f(m_uniformLocations[location], value); }
    void setUniformValue(int location, GLfloat x, GLfloat y) { glUniform2f(m_uniformLocations[location], x, y); }
    void setUniformValue(int location, GLfloat x, GLfloat y, GLfloat z) { glUniform3f(m_uniformLocations[location], x, y, z); }
    void setUniformValue(int location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) { glUniform4f(m_uniformLocations[location], x, y, z, w); }
    void setUniformValue(int location, GLfloat mat2[2][2], bool transpose) { glUniformMatrix2fv(m_uniformLocations[location], 1, transpose ? GL_TRUE : GL_FALSE, (GLfloat *)mat2); }
    void setUniformValue(int location, GLfloat mat3[3][3], bool transpose) { glUniformMatrix3fv(m_uniformLocations[location], 1, transpose ? GL_TRUE : GL_FALSE, (GLfloat *)mat3); }
    void setUniformValue(const char *name, GLint value) { glUniform1i(glGetUniformLocation(m_programId, name), value); }
    void setUniformValue(const char *name, GLfloat value) { glUniform1f(glGetUniformLocation(m_programId, name), value); }
    void setUniformValue(const char *name, GLfloat x, GLfloat y) { glUniform2f(glGetUniformLocation(m_programId, name), x, y); }
    void setUniformValue(const char *name, GLfloat x, GLfloat y, GLfloat z) { glUniform3f(glGetUniformLocation(m_programId, name), x, y, z); }
    void setUniformValue(const char *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) { glUniform4f(glGetUniformLocation(m_programId, name), x, y, z, w); }
    void setUniformValue(const char *name, GLfloat mat2[2][2], bool transpose = false) { glUniformMatrix2fv(glGetUniformLocation(m_programId, name), 1, transpose ? GL_TRUE : GL_FALSE, (GLfloat *)mat2); }
    void setUniformValue(const char *name, GLfloat mat3[3][3], bool transpose = false) { glUniformMatrix3fv(glGetUniformLocation(m_programId, name), 1, transpose ? GL_TRUE : GL_FALSE, (GLfloat *)mat3); }

    // Point, PointF, Color, Size, SizeF,
    bool isLinked() { return m_linked; }
    GLuint getProgramId() { return m_programId; }
    ShaderList getShaders() { return m_shaders; }

private:
    bool m_linked;
    GLuint m_programId;
    static GLuint m_currentProgram;
    ShaderList m_shaders;
    std::array<GLint, MAX_UNIFORM_LOCATIONS> m_uniformLocations;
};

#endif
