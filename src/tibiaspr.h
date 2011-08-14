#ifndef TIBIASPR_H
#define TIBIASPR_H

#include <global.h>
#include <graphics/texture.h>

class TibiaSpr
{
public:
    TibiaSpr();
    ~TibiaSpr();

    bool load(const std::string& filename);

    uint32 getSignature() { return m_signature; }
    uint16 getSpritesCount() { return m_spritesCount; }
    TexturePtr getSprite(uint32 id);

private:
    TexturePtr loadSprite(uint32 id);

    std::stringstream m_fin;
    uint32 m_signature;
    uint16 m_spritesCount;
    TexturePtr *m_sprites;
    TexturePtr m_transparentSprite;
};

extern TibiaSpr g_tibiaSpr;

#endif // TIBIASPR_H
