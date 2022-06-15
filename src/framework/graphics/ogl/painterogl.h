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

#pragma once

#include <framework/graphics/painter.h>

class PainterOGL : public Painter
{
public:
    PainterOGL();
    ~PainterOGL() override = default;

    virtual void setTransformMatrix(const Matrix3& transformMatrix) { m_transformMatrix = transformMatrix; }
    virtual void setProjectionMatrix(const Matrix3& projectionMatrix) { m_projectionMatrix = projectionMatrix; }
    virtual void setTextureMatrix(const Matrix3& textureMatrix) { m_textureMatrix = textureMatrix; }

    void resetBlendEquation() { setBlendEquation(BlendEquation::ADD); }
    void resetTexture() { setTexture(nullptr); }
    void resetAlphaWriting() { setAlphaWriting(false); }
    void resetTransformMatrix() { setTransformMatrix(DEFAULT_MATRIX_3); }

    void executeState(const PainterState& state) override;

    void bind() override { refreshState(); }
    void unbind() override {}

    void clear(const Color& color) override;
    void clearRect(const Color& color, const Rect& rect);

    void setCompositionMode(CompositionMode compositionMode) override;
    void setBlendEquation(BlendEquation blendEquation) override;
    void setClipRect(const Rect& clipRect) override;
    void setShaderProgram(PainterShaderProgram* shaderProgram) override { m_shaderProgram = shaderProgram; }
    void setTexture(Texture* texture) override;
    void setAlphaWriting(bool enable) override;

    void setTexture(const TexturePtr& texture) { setTexture(texture.get()); }
    void setResolution(const Size& resolution) override;

    void scale(float x, float y) override;
    void translate(float x, float y) override;
    void rotate(float angle) override;
    void rotate(float x, float y, float angle) override;

    void pushTransformMatrix() override;
    void popTransformMatrix() override;

    Matrix3 getTransformMatrix() override { return m_transformMatrix; }
    Matrix3 getProjectionMatrix() override { return m_projectionMatrix; }
    Matrix3 getTextureMatrix() override { return m_textureMatrix; }
    BlendEquation getBlendEquation() { return m_blendEquation; }
    PainterShaderProgram* getShaderProgram() { return m_shaderProgram; }
    bool getAlphaWriting() { return m_alphaWriting; }

protected:
    virtual void refreshState();

    Matrix3& getTransformMatrixRef() override { return m_transformMatrix; }

    void updateGlTexture();
    void updateGlCompositionMode();
    void updateGlBlendEquation();
    void updateGlClipRect();
    void updateGlAlphaWriting();
    void updateGlViewport();

    CoordsBuffer m_coordsBuffer;

    std::vector<Matrix3> m_transformMatrixStack;
    Matrix3 m_transformMatrix = DEFAULT_MATRIX_3;
    Matrix3 m_projectionMatrix = DEFAULT_MATRIX_3;
    Matrix3 m_textureMatrix = DEFAULT_MATRIX_3;

    BlendEquation m_blendEquation{ BlendEquation::ADD };
    Texture* m_texture{ nullptr };
    bool m_alphaWriting{ false };
    uint m_glTextureId{ 0 };
};
