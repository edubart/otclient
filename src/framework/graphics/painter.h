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

#ifndef PAINTER_H
#define PAINTER_H

#include "declarations.h"
#include "coordsbuffer.h"
#include "paintershaderprogram.h"
#include "texture.h"

class Painter
{
public:
    enum CompositionMode {
        CompositionMode_Normal,
        CompositionMode_Multiply,
        CompositionMode_Add,
        CompositionMode_Replace,
        CompositionMode_DestBlending
    };
    enum DrawMode {
        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP
    };

    struct PainterState {
        Size resolution;
        Matrix3 projectionMatrix;
        Matrix3 textureMatrix;
        Color color;
        float opacity;
        Painter::CompositionMode compositionMode;
        Rect clipRect;
        Texture *texture;
        PainterShaderProgram *shaderProgram;
        bool alphaWriting;
    };

    Painter();
    virtual ~Painter() { }

    virtual void bind() { refreshState(); }
    virtual void unbind() { }

    void resetState();
    virtual void refreshState();
    void saveState();
    void saveAndResetState();
    void restoreSavedState();

    void clear(const Color& color);
    void clearRect(const Color& color, const Rect& rect);

    virtual void drawCoords(CoordsBuffer& coordsBuffer, DrawMode drawMode = Triangles) = 0;
    virtual void drawTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture) = 0;
    virtual void drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src) = 0;
    void drawTexturedRect(const Rect& dest, const TexturePtr& texture) { drawTexturedRect(dest, texture, Rect(Point(0,0), texture->getSize())); }
    virtual void drawRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src) = 0;
    virtual void drawFilledRect(const Rect& dest) = 0;
    virtual void drawFilledTriangle(const Point& a, const Point& b, const Point& c) = 0;
    virtual void drawBoundingRect(const Rect& dest, int innerLineWidth = 1) = 0;

    virtual void setProjectionMatrix(const Matrix3& projectionMatrix) { m_projectionMatrix = projectionMatrix; }
    virtual void setTextureMatrix(const Matrix3& textureMatrix) { m_textureMatrix = textureMatrix; }
    virtual void setColor(const Color& color) { m_color = color; }
    virtual void setOpacity(float opacity) { m_opacity = opacity; }
    virtual void setCompositionMode(CompositionMode compositionMode);
    virtual void setClipRect(const Rect& clipRect);
    virtual void setShaderProgram(PainterShaderProgram *shaderProgram) { m_shaderProgram = shaderProgram; }
    virtual void setTexture(Texture *texture);
    void setAlphaWriting(bool enable);

    void setShaderProgram(const PainterShaderProgramPtr& shaderProgram) { setShaderProgram(shaderProgram.get()); }
    void setTexture(const TexturePtr& texture) { setTexture(texture.get()); }
    void setResolution(const Size& resolution);

    Matrix3 getProjectionMatrix() { return m_projectionMatrix; }
    Matrix3 getTextureMatrix() { return m_textureMatrix; }
    Color getColor() { return m_color; }
    float getOpacity() { return m_opacity; }
    CompositionMode getCompositionMode() { return m_compositionMode; }
    Rect getClipRect() { return m_clipRect; }
    PainterShaderProgram *getShaderProgram() { return m_shaderProgram; }
    bool getAlphaWriting() { return m_alphaWriting; }
    Size getResolution() { return m_resolution; };

    void resetColor() { setColor(Color::white); }
    void resetOpacity() { setOpacity(1.0f); }
    void resetClipRect() { setClipRect(Rect()); }
    void resetCompositionMode() { setCompositionMode(CompositionMode_Normal); }
    void resetShaderProgram() { setShaderProgram(nullptr); }
    void resetTexture() { setTexture(nullptr); }
    void resetAlphaWriting() { setAlphaWriting(false); }

    virtual bool hasShaders() = 0;

protected:
    void updateGlTexture();
    void updateGlCompositionMode();
    void updateGlClipRect();
    void updateGlAlphaWriting();
    void updateGlViewport();

    CoordsBuffer m_coordsBuffer;

    Matrix3 m_projectionMatrix;
    Matrix3 m_textureMatrix;
    Color m_color;
    float m_opacity;
    CompositionMode m_compositionMode;
    Rect m_clipRect;
    Texture *m_texture;
    PainterShaderProgram *m_shaderProgram;
    bool m_alphaWriting;
    Size m_resolution;

    PainterState m_olderStates[10];
    int m_oldStateIndex;

    uint m_glTextureId;
};

extern Painter *g_painter;

#endif
