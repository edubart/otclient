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

#ifndef PAINTERSHADERPROGRAM_H
#define PAINTERSHADERPROGRAM_H

#include "shaderprogram.h"
#include "coordsbuffer.h"
#include <framework/core/timer.h>

class PainterShaderProgram : public ShaderProgram
{
protected:
    enum {
        VERTEX_ATTR = 0,
        TEXCOORD_ATTR = 1,
        PROJECTION_MATRIX_UNIFORM = 0,
        TEXTURE_MATRIX_UNIFORM = 1,
        COLOR_UNIFORM = 2,
        OPACITY_UNIFORM = 3,
        TIME_UNIFORM = 4,
        TEX0_UNIFORM = 5,
        TEX1_UNIFORM = 6,
        TEX2_UNIFORM = 7,
        TEX3_UNIFORM = 8,
        RESOLUTION_UNIFORM = 9,
        TRANSFORM_MATRIX_UNIFORM = 10
    };

    friend class PainterOGL2;

    virtual void setupUniforms();

public:
    PainterShaderProgram();

    bool link();

    void setTransformMatrix(const Matrix3& transformMatrix);
    void setProjectionMatrix(const Matrix3& projectionMatrix);
    void setTextureMatrix(const Matrix3& textureMatrix);
    void setColor(const Color& color);
    void setOpacity(float opacity);
    void setResolution(const Size& resolution);
    void updateTime();

    void addMultiTexture(const std::string& file);
    void bindMultiTextures();

private:
    float m_startTime;

    Color m_color;
    float m_opacity;
    Matrix3 m_transformMatrix;
    Matrix3 m_projectionMatrix;
    Matrix3 m_textureMatrix;
    Size m_resolution;
    float m_time;
    std::vector<TexturePtr> m_multiTextures;
};

#endif
