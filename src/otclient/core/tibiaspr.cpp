#include "tibiaspr.h"
#include <framework/core/resourcemanager.h>

TibiaSpr g_tibiaSpr;

TibiaSpr::TibiaSpr()
{
    m_sprites = NULL;
    m_spritesCount = 0;
}

TibiaSpr::~TibiaSpr()
{
    //for(uint16 i = 0; i < m_spritesCount; i++)
        //if(m_sprites[i])
            //delete m_sprites[i];

    if(m_sprites)
        delete []m_sprites;

    //delete m_transparentSprite;
}

bool TibiaSpr::load(const std::string &filename)
{
    g_resources.loadFile(filename, m_fin);

    m_fin.read((char*)&m_signature, 4);
    m_fin.read((char*)&m_spritesCount, 2);

    m_sprites = new TexturePtr[m_spritesCount];

    //for(uint16 i = 0; i < m_spritesCount; i++)
        //m_sprites[i] = NULL;


    uchar pixels[4096];
    for(int i = 0; i < 32*32*4; i += 4) {
        pixels[i + 0] = 0xFF;
        pixels[i + 1] = 0x00;
        pixels[i + 2] = 0xFF;
        pixels[i + 3] = 0xFF;
    }
    m_transparentSprite = TexturePtr(new Texture(32, 32, 4, pixels));

    return true;
}

TexturePtr TibiaSpr::loadSprite(uint32 id)
{
    if(m_fin.eof())
        return TexturePtr();

    m_fin.seekg((id * 4) + 6, std::ios_base::beg);

    uint32 spriteAddress;
    m_fin.read((char*)&spriteAddress, 4);

    if(spriteAddress == 0) // Some error on tibia.spr, save a blank image.
        return TexturePtr();

    m_fin.seekg(spriteAddress, std::ios_base::beg);

    uint32 colorKey = 0;
    m_fin.read((char*)&colorKey, 3);

    uint16 spriteSize;
    m_fin.read((char*)&spriteSize, 2);

    uchar image[4096];
    uint16 imgPos = 0, read = 0, transparentPixels, coloredPixels, x;
    while(read < spriteSize) {
        m_fin.read((char*)&transparentPixels, 2);
        m_fin.read((char*)&coloredPixels, 2);

        for(x = 0; x < transparentPixels; x++) {
            image[imgPos + 0] = 0x00;
            image[imgPos + 1] = 0x00;
            image[imgPos + 2] = 0x00;
            image[imgPos + 3] = 0x00;
            imgPos += 4;
        }

        for(x = 0; x < coloredPixels; x++) {
            m_fin.read((char*)&image[imgPos + 0], 1);
            m_fin.read((char*)&image[imgPos + 1], 1);
            m_fin.read((char*)&image[imgPos + 2], 1);
            image[imgPos + 3] = 0xFF;

            imgPos += 4;
        }

        read += 4 + (3 * coloredPixels);
    }

    // Fill remaining bytes with pink.
    if(imgPos < 32 * 32 * 4) {
        for(x = imgPos; x < 32 * 32 * 4; x += 4) {
            image[x + 0] = 0xFF;
            image[x + 1] = 0x00;
            image[x + 2] = 0xFF;
            image[x + 3] = 0x00;
        }
    }

    return TexturePtr(new Texture(32, 32, 4, image));
}

TexturePtr TibiaSpr::getSprite(uint32 id)
{
    if(!m_sprites)
        return m_transparentSprite;

    if(!m_sprites[id]) {
        m_sprites[id] = loadSprite(id);
        if(!m_sprites[id])
            return m_transparentSprite;
    }
    return m_sprites[id];
}
