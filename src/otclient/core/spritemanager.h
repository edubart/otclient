#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include "declarations.h"
#include <framework/graphics/texture.h>

struct Sprite {
    TexturePtr texture;
    TexturePtr masks[4];
};

class SpriteManager
{
public:
    SpriteManager();

    bool load(const std::string& file);
    void unload();

    uint32 getSignature() { return m_signature; }
    int getSpritesCount() { return m_spritesCount; }

    TexturePtr getSpriteTexture(int id, SpriteMask mask = SpriteMaskNone);

private:
    TexturePtr loadSpriteTexture(int id);
    TexturePtr loadSpriteMask(TexturePtr spriteTex, SpriteMask mask);

    uint32 m_signature;
    uint16 m_spritesCount;
    std::stringstream m_fin;
    std::vector<Sprite> m_sprites;
    TexturePtr m_transparentSprite;
};

extern SpriteManager g_sprites;

#endif
