#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "graphicsdeclarations.h"

class FrameBuffer
{
public:
    FrameBuffer(int width, int height);
    virtual ~FrameBuffer();

    /// Bind the framebuffer, everything rendered will be draw on it
    void bind();

    /// Unbind the framebuffer, render on back buffer again
    void unbind();

    /// Draw framebuffer
    void draw(int x, int y, int width, int height);

private:
    uint m_fboTexture;
    uint m_fbo;
    bool m_fallbackOldImp;
    int m_width;
    int m_height;
};

#endif
