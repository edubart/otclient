#include "spritemanager.h"
#include <framework/core/resourcemanager.h>
#include <framework/graphics/graphics.h>

SpriteManager g_sprites;

SpriteManager::SpriteManager()
{
    m_spritesCount = 0;
    m_signature = 0;
}

bool SpriteManager::load(const std::string &filename)
{
    g_resources.loadFile(filename, m_fin);
    m_signature = fw::getu32(m_fin);
    m_spritesCount = fw::getu16(m_fin);
    m_sprites.resize(m_spritesCount);
    return true;
}

void SpriteManager::unload()
{
    m_sprites.clear();
    m_spritesCount = 0;
    m_signature = 0;
}

TexturePtr SpriteManager::loadSprite(int id)
{
    m_fin.seekg(((id-1) * 4) + 6, std::ios_base::beg);

    uint32 spriteAddress = fw::getu32(m_fin);

    // no sprite? return an empty texture
    if(spriteAddress == 0)
        return g_graphics.getEmptyTexture();

    m_fin.seekg(spriteAddress, std::ios_base::beg);
    assert(m_fin.good());

    // skip color key
    fw::getu8(m_fin);
    fw::getu8(m_fin);
    fw::getu8(m_fin);

    uint16 pixelDataSize = fw::getu16(m_fin);

    uchar pixels[4096];
    int writePos = 0;
    int read = 0;

    // decompress pixels
    while(read < pixelDataSize) {
        uint16 transparentPixels, coloredPixels;
        m_fin.read((char*)&transparentPixels, 2);
        m_fin.read((char*)&coloredPixels, 2);

        for(int i = 0; i < transparentPixels; i++) {
            pixels[writePos + 0] = 0x00;
            pixels[writePos + 1] = 0x00;
            pixels[writePos + 2] = 0x00;
            pixels[writePos + 3] = 0x00;
            writePos += 4;
        }

        for(int i = 0; i < coloredPixels; i++) {
            pixels[writePos + 0] = fw::getu8(m_fin);
            pixels[writePos + 1] = fw::getu8(m_fin);
            pixels[writePos + 2] = fw::getu8(m_fin);
            pixels[writePos + 3] = 0xFF;

            writePos += 4;
        }

        read += 4 + (3 * coloredPixels);
    }

    // fill remaining pixels with alpha
    while(writePos < 4096) {
        pixels[writePos + 0] = 0x00;
        pixels[writePos + 1] = 0x00;
        pixels[writePos + 2] = 0x00;
        pixels[writePos + 3] = 0x00;
        writePos += 4;
    }

    return TexturePtr(new Texture(32, 32, 4, pixels));
}

TexturePtr SpriteManager::getSpriteTexture(int id)
{
    if(id == 0)
        return g_graphics.getEmptyTexture();

    assert(id > 0 && id <= m_spritesCount);

    // load sprites on demand
    TexturePtr texture = m_sprites[id-1];
    if(!texture) {
        texture = loadSprite(id);
        m_sprites[id-1] = texture;
    }

    //TODO: release unused sprites textures after X seconds
    // to avoid massive texture allocations

    return texture;
}
