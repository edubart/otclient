#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "declarations.h"

class FrameBuffer
{
public:
    FrameBuffer(int width, int height);
    virtual ~FrameBuffer();

    /// Binds the framebuffer, by switching render buffer to itself, everything rendered will be draw on it
    void bind();

    /// Unbinds the framebuffer (switch render buffer to back buffer again)
    void unbind();

    /// Draws framebuffer texture
    void draw(const Rect& screenCoords, const Rect& framebufferCoords = Rect());

    TexturePtr getTexture() { return m_texture; }

private:
    TexturePtr m_texture;
    uint m_fbo;
    bool m_fallbackOldImp;
};

#endif
