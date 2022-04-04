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

#ifndef PAINTER_H
#define PAINTER_H

#include <framework/graphics/coordsbuffer.h>
#include <framework/graphics/declarations.h>
#include <framework/graphics/paintershaderprogram.h>

class Painter
{
public:
    enum BlendEquation
    {
        BlendEquation_Add,
        BlendEquation_Max,
        BlendEquation_Min,
        BlendEquation_Subtract,
        BlendEquation_Rever_Subtract,
    };
    enum CompositionMode
    {
        CompositionMode_Normal,
        CompositionMode_Multiply,
        CompositionMode_Add,
        CompositionMode_Replace,
        CompositionMode_DestBlending,
        CompositionMode_Light
    };

    enum class DrawMode
    {
        None = GL_NONE,
        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP
    };

    struct PainterState
    {
        Size resolution;
        Matrix3 transformMatrix;
        Matrix3 projectionMatrix;
        Matrix3 textureMatrix;
        Color color;
        float opacity;
        CompositionMode compositionMode;
        BlendEquation blendEquation;
        Rect clipRect;
        TexturePtr texture;
        PainterShaderProgram* shaderProgram;
        bool alphaWriting;

        bool operator==(const PainterState& s2) const
        {
            return resolution == s2.resolution &&
                transformMatrix == s2.transformMatrix &&
                color == s2.color &&
                opacity == s2.opacity &&
                compositionMode == s2.compositionMode &&
                blendEquation == s2.blendEquation &&
                clipRect == s2.clipRect &&
                texture == s2.texture &&
                shaderProgram == s2.shaderProgram &&
                alphaWriting == s2.alphaWriting;
        }
    };

    Painter();
    virtual ~Painter() = default;

    virtual void bind() {}
    virtual void unbind() {}

    virtual void resetState() = 0;
    virtual void saveState() = 0;
    virtual void saveAndResetState() = 0;
    virtual void restoreSavedState() = 0;
    virtual PainterState getCurrentState() = 0;
    virtual void executeState(const PainterState& state) = 0;

    virtual void clear(const Color& color) = 0;

    virtual void drawCoords(CoordsBuffer& coordsBuffer, DrawMode drawMode = DrawMode::Triangles) = 0;

    virtual void setTexture(Texture* texture) = 0;
    virtual void setClipRect(const Rect& clipRect) = 0;
    virtual void setColor(const Color& color) { m_color = color; }
    virtual void setAlphaWriting(bool enable) = 0;
    virtual void setBlendEquation(BlendEquation blendEquation) = 0;
    virtual void setShaderProgram(PainterShaderProgram* shaderProgram) { m_shaderProgram = shaderProgram; }
    void setShaderProgram(const PainterShaderProgramPtr& shaderProgram) { setShaderProgram(shaderProgram.get()); }

    virtual void scale(float x, float y) = 0;
    void scale(float factor) { scale(factor, factor); }
    virtual void translate(float x, float y) = 0;
    void translate(const Point& p) { translate(p.x, p.y); }
    virtual void rotate(float angle) = 0;
    virtual void rotate(float x, float y, float angle) = 0;
    void rotate(const Point& p, float angle) { rotate(p.x, p.y, angle); }

    virtual void setOpacity(float opacity) { m_opacity = opacity; }
    virtual void setResolution(const Size& resolution) { m_resolution = resolution; }

    Size getResolution() { return m_resolution; }
    Color getColor() { return m_color; }
    float getOpacity() { return m_opacity; }
    Rect getClipRect() { return m_clipRect; }
    CompositionMode getCompositionMode() { return m_compositionMode; }

    virtual void setCompositionMode(CompositionMode compositionMode) = 0;

    virtual void pushTransformMatrix() = 0;
    virtual void popTransformMatrix() = 0;

    void resetClipRect() { setClipRect(Rect()); }
    void resetOpacity() { setOpacity(1.0f); }
    void resetCompositionMode() { setCompositionMode(CompositionMode_Normal); }
    void resetColor() { setColor(Color::white); }
    void resetShaderProgram() { setShaderProgram(nullptr); }

    virtual bool hasShaders() = 0;

protected:
    PainterShaderProgram* m_shaderProgram;
    CompositionMode m_compositionMode;
    Color m_color;
    Size m_resolution;
    float m_opacity;
    Rect m_clipRect;
};

extern Painter* g_painter;

#endif
