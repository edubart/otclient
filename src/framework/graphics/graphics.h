#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "graphicsdeclarations.h"

class Graphics
{
    enum DrawMode {
        DRAW_NONE = 0,
        DRAW_QUADS = 1,
        DRAW_TEXTURE = 2,
        DRAW_COLORED = 4,
        DRAW_COLOR_QUADS = DRAW_QUADS | DRAW_COLORED,
        DRAW_TEXTURE_QUADS = DRAW_QUADS | DRAW_TEXTURE | DRAW_COLORED
    };

public:
    /// Initialize default OpenGL states
    void init();

    /// Termiante graphics
    void terminate();

    /// Check if a GL extension is supported
    bool isExtensionSupported(const char *extension);

    /// Resizes OpenGL viewport
    void resize(const Size& size);

    /// Restore original viewport
    void restoreViewport();

    /// Called before every render
    void beginRender();

    /// Called after every render
    void endRender();
    void disableDrawing();

    // drawing API
    void drawTexturedRect(const Rect& screenCoords,
                          const TexturePtr& texture,
                          const Rect& textureCoords = Rect(),
                          const Color& color = Color::white);

    void drawRepeatedTexturedRect(const Rect& screenCoords,
                                  const TexturePtr& texture,
                                  const Rect& textureCoords,
                                  const Color& color = Color::white);

    void drawFilledRect(const Rect& screenCoords,
                        const Color& color);

    void drawBoundingRect(const Rect& screenCoords,
                          const Color& color = Color::green,
                          int innerLineWidth = 1);

    const Size& getScreenSize() const { return m_screenSize; }

private:
    void bindTexture(const TexturePtr& texture, const Color& color = Color::white);
    void bindColor(const Color& color);

    TexturePtr m_boundTexture;
    Color m_boundColor;
    Size m_screenSize;
    DrawMode m_drawMode;
};

extern Graphics g_graphics;

#endif
