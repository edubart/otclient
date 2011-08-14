#ifndef TEXTURE_H
#define TEXTURE_H

#include "graphicsdeclarations.h"

class Texture : public std::enable_shared_from_this<Texture>
{
public:
    /// Create a texture, width and height must be a multiple of 2
    Texture(int width, int height, int channels, uchar* pixels = NULL);
    virtual ~Texture();

    /// Enable texture bilinear filter (smooth scaled textures)
    virtual void enableBilinearFilter();

    /// Get OpenGL texture id
    virtual uint getTextureId() const { return m_textureId; }

    /// Copy pixels from OpenGL texture
    uchar* getPixels();

    const Size& getSize() const { return m_size; }
    const Size& getGlSize() const { return m_glSize; }

protected:
    Texture() {  }
    uint internalLoadGLTexture(uchar* pixels, int channels, int w, int h);

    uint m_textureId;
    Size m_size;
    Size m_glSize;
};

#endif
