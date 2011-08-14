#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "graphicsdeclarations.h"

class Graphics
{
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

    void bindColor(const Color& color);
    void bindTexture(const TexturePtr& texture);

    // drawing API
    void drawTexturedRect(const Rect& screenCoords,
                          const TexturePtr& texture,
                          const Rect& textureCoords = Rect());

    void drawRepeatedTexturedRect(const Rect& screenCoords,
                                  const TexturePtr& texture,
                                  const Rect& textureCoords);

    void drawFilledRect(const Rect& screenCoords,
                        const Color& color);

    void drawBoundingRect(const Rect& screenCoords,
                          const Color& color = Color::green,
                          int innerLineWidth = 1);

    const Size& getScreenSize() const { return m_screenSize; }

    void startDrawing();
    void stopDrawing();

    int getOpacity() const { return m_opacity; }
    void setOpacity(int opacity) { m_opacity = opacity; }

private:
    bool m_drawing;
    int m_opacity;
    Size m_screenSize;
};

extern Graphics g_graphics;

#endif
