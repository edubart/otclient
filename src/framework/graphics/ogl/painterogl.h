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

#ifndef PAINTEROGL_H
#define PAINTEROGL_H

#include <framework/graphics/painter.h>

class PainterOGL : public Painter
{
public:
    struct PainterState {
        Size resolution;
        Matrix3 transformMatrix;
        Matrix3 projectionMatrix;
        Matrix3 textureMatrix;
        Color color;
        float opacity;
        Painter::CompositionMode compositionMode;
        Painter::BlendEquation blendEquation;
        Rect clipRect;
        Texture *texture;
        PainterShaderProgram *shaderProgram;
        bool alphaWriting;
    };

    PainterOGL();
    virtual ~PainterOGL() { }

    virtual void bind() { refreshState(); }
    virtual void unbind() { }

    void resetState();
    virtual void refreshState();
    void saveState();
    void saveAndResetState();
    void restoreSavedState();

    void clear(const Color& color);
    void clearRect(const Color& color, const Rect& rect);

    virtual void setTransformMatrix(const Matrix3& transformMatrix) { m_transformMatrix = transformMatrix; }
    virtual void setProjectionMatrix(const Matrix3& projectionMatrix) { m_projectionMatrix = projectionMatrix; }
    virtual void setTextureMatrix(const Matrix3& textureMatrix) { m_textureMatrix = textureMatrix; }
    virtual void setCompositionMode(CompositionMode compositionMode);
    virtual void setBlendEquation(BlendEquation blendEquation);
    virtual void setClipRect(const Rect& clipRect);
    virtual void setShaderProgram(PainterShaderProgram *shaderProgram) { m_shaderProgram = shaderProgram; }
    virtual void setTexture(Texture *texture);
    virtual void setAlphaWriting(bool enable);

    void setTexture(const TexturePtr& texture) { setTexture(texture.get()); }
    void setResolution(const Size& resolution);

    void scale(float x, float y);
    void translate(float x, float y);
    void rotate(float angle);
    void rotate(float x, float y, float angle);

    void pushTransformMatrix();
    void popTransformMatrix();

    Matrix3 getTransformMatrix() { return m_transformMatrix; }
    Matrix3 getProjectionMatrix() { return m_projectionMatrix; }
    Matrix3 getTextureMatrix() { return m_textureMatrix; }
    BlendEquation getBlendEquation() { return m_blendEquation; }
    PainterShaderProgram *getShaderProgram() { return m_shaderProgram; }
    bool getAlphaWriting() { return m_alphaWriting; }

    void resetBlendEquation() { setBlendEquation(BlendEquation_Add); }
    void resetTexture() { setTexture(nullptr); }
    void resetAlphaWriting() { setAlphaWriting(false); }
    void resetTransformMatrix() { setTransformMatrix(Matrix3()); }

protected:
    void updateGlTexture();
    void updateGlCompositionMode();
    void updateGlBlendEquation();
    void updateGlClipRect();
    void updateGlAlphaWriting();
    void updateGlViewport();

    CoordsBuffer m_coordsBuffer;

    std::vector<Matrix3> m_transformMatrixStack;
    Matrix3 m_transformMatrix;
    Matrix3 m_projectionMatrix;
    Matrix3 m_textureMatrix;

    BlendEquation m_blendEquation;
    Texture *m_texture;
    bool m_alphaWriting;

    PainterState m_olderStates[10];
    int m_oldStateIndex;

    uint m_glTextureId;
};

#endif
