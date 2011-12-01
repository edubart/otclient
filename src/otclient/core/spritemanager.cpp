/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "spritemanager.h"
#include <framework/core/resourcemanager.h>
#include <framework/graphics/graphics.h>

SpriteManager g_sprites;

SpriteManager::SpriteManager()
{
    m_spritesCount = 0;
    m_signature = 0;
}

bool SpriteManager::load(const std::string& file)
{
    try {
        g_resources.loadFile(file, m_fin);
        m_signature = Fw::getU32(m_fin);
        m_spritesCount = Fw::getU16(m_fin);
        m_sprites.resize(m_spritesCount);
        return true;
    } catch(Exception& e) {
        logError("Failed to load sprites from '", file, "': ", e.what());
        return false;
    }
}

void SpriteManager::unload()
{
    m_sprites.clear();
    m_spritesCount = 0;
    m_signature = 0;
}

TexturePtr SpriteManager::loadSpriteTexture(int id)
{
    m_fin.seekg(((id-1) * 4) + 6, std::ios_base::beg);

    uint32 spriteAddress = Fw::getU32(m_fin);

    // no sprite? return an empty texture
    if(spriteAddress == 0)
        return g_graphics.getEmptyTexture();

    m_fin.seekg(spriteAddress, std::ios_base::beg);
    assert(m_fin.good());

    // skip color key
    Fw::getU8(m_fin);
    Fw::getU8(m_fin);
    Fw::getU8(m_fin);

    uint16 pixelDataSize = Fw::getU16(m_fin);

    uchar pixels[4096];
    int writePos = 0;
    int read = 0;

    // decompress pixels
    while(read < pixelDataSize) {
        uint16 transparentPixels, coloredPixels;
        m_fin.read((char*)&transparentPixels, 2);
        m_fin.read((char*)&coloredPixels, 2);

        if(writePos + transparentPixels*4 + coloredPixels*3 >= 4096)
            return g_graphics.getEmptyTexture();

        for(int i = 0; i < transparentPixels; i++) {
            pixels[writePos + 0] = 0x00;
            pixels[writePos + 1] = 0x00;
            pixels[writePos + 2] = 0x00;
            pixels[writePos + 3] = 0x00;
            writePos += 4;
        }

        for(int i = 0; i < coloredPixels; i++) {
            pixels[writePos + 0] = Fw::getU8(m_fin);
            pixels[writePos + 1] = Fw::getU8(m_fin);
            pixels[writePos + 2] = Fw::getU8(m_fin);
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

TexturePtr SpriteManager::loadSpriteMask(TexturePtr spriteTex, Otc::SpriteMask mask)
{
    auto pixels = spriteTex->getPixels();

    static uint32 maskColors[4] = { Fw::red, Fw::green, Fw::blue, Fw::yellow };
    uint32 maskColor = maskColors[mask];
    uint32 whiteColor = Fw::white;
    uint32 alphaColor = Fw::alpha;

    // convert pixels
    // masked color -> white color
    // any other color -> alpha color
    for(int i=0;i<4096;i+=4) {
        uint32& currentColor = *(uint32*)&pixels[i];
        if(currentColor == maskColor)
            currentColor = whiteColor;
        else
            currentColor = alphaColor;
    }

    return TexturePtr(new Texture(32, 32, 4, &pixels[0]));
}

TexturePtr SpriteManager::getSpriteTexture(int id, Otc::SpriteMask mask)
{
    if(id == 0)
        return g_graphics.getEmptyTexture();

    assert(id > 0 && id <= m_spritesCount);

    // load sprites on demand
    Sprite& sprite = m_sprites[id-1];
    if(!sprite.texture)
        sprite.texture = loadSpriteTexture(id);

    //TODO: release unused sprites textures after X seconds
    // to avoid massive texture allocations

    if(mask != Otc::SpriteNoMask) {
        if(!sprite.masks[mask])
            sprite.masks[mask] = loadSpriteMask(sprite.texture, mask);
        return sprite.masks[mask];
    } else
        return sprite.texture;
}
