/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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
#include "game.h"
#include <framework/core/resourcemanager.h>
#include <framework/core/filestream.h>
#include <framework/graphics/image.h>

SpriteManager g_sprites;

SpriteManager::SpriteManager()
{
    m_spritesCount = 0;
    m_signature = 0;
}

void SpriteManager::terminate()
{
    unload();
}

bool SpriteManager::loadSpr(std::string file)
{
    m_spritesCount = 0;
    m_signature = 0;
    m_loaded = false;
    try {
        file = g_resources.guessFilePath(file, "spr");

        m_spritesFile = g_resources.openFile(file);
        // cache file buffer to avoid lags from hard drive
        m_spritesFile->cache();

        m_signature = m_spritesFile->getU32();
        m_spritesCount = g_game.getFeature(Otc::GameSpritesU32) ? m_spritesFile->getU32() : m_spritesFile->getU16();
        m_spritesOffset = m_spritesFile->tell();
        m_loaded = true;
        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("Failed to load sprites from '%s': %s", file, e.what()));
        return false;
    }
}

void SpriteManager::unload()
{
    m_spritesCount = 0;
    m_signature = 0;
    m_spritesFile = nullptr;
}

ImagePtr SpriteManager::getSpriteImage(int id)
{
    try {
        enum {
            SPRITE_SIZE = 32,
            SPRITE_DATA_SIZE = SPRITE_SIZE*SPRITE_SIZE*4
        };

        if(id == 0 || !m_spritesFile)
            return nullptr;

        m_spritesFile->seek(((id-1) * 4) + m_spritesOffset);

        uint32 spriteAddress = m_spritesFile->getU32();

        // no sprite? return an empty texture
        if(spriteAddress == 0)
            return nullptr;

        m_spritesFile->seek(spriteAddress);

        // skip color key
        m_spritesFile->getU8();
        m_spritesFile->getU8();
        m_spritesFile->getU8();

        uint16 pixelDataSize = m_spritesFile->getU16();

        ImagePtr image(new Image(Size(SPRITE_SIZE, SPRITE_SIZE)));

        uint8 *pixels = image->getPixelData();
        int writePos = 0;
        int read = 0;

        // decompress pixels
        while(read < pixelDataSize && writePos < SPRITE_DATA_SIZE) {
            uint16 transparentPixels = m_spritesFile->getU16();
            uint16 coloredPixels = m_spritesFile->getU16();

            for(int i = 0; i < transparentPixels && writePos < SPRITE_DATA_SIZE; i++) {
                pixels[writePos + 0] = 0x00;
                pixels[writePos + 1] = 0x00;
                pixels[writePos + 2] = 0x00;
                pixels[writePos + 3] = 0x00;
                writePos += 4;
            }

            for(int i = 0; i < coloredPixels && writePos < SPRITE_DATA_SIZE; i++) {
                pixels[writePos + 0] = m_spritesFile->getU8();
                pixels[writePos + 1] = m_spritesFile->getU8();
                pixels[writePos + 2] = m_spritesFile->getU8();
                pixels[writePos + 3] = 0xFF;
                writePos += 4;
            }

            read += 4 + (3 * coloredPixels);
        }

        // fill remaining pixels with alpha
        while(writePos < SPRITE_DATA_SIZE) {
            pixels[writePos + 0] = 0x00;
            pixels[writePos + 1] = 0x00;
            pixels[writePos + 2] = 0x00;
            pixels[writePos + 3] = 0x00;
            writePos += 4;
        }

        return image;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("Failed to get sprite id %d: %s", id, e.what()));
        return nullptr;
    }
}

