#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "texture.h"

class TextureManager
{
public:
    /// Load a texture from file, if is already loaded, it will be retrieved from cache
    TexturePtr getTexture(const std::string& textureFile);

    /// Load a png textures
    static TexturePtr loadPNG(std::stringstream& file);

private:
    std::map<std::string, TextureWeakPtr> m_textures;
};

extern TextureManager g_textures;

#endif
