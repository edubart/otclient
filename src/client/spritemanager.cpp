/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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
#include "spriteappearances.h"
#include <framework/core/filestream.h>
#include <framework/core/resourcemanager.h>
#include <framework/graphics/image.h>
#include "game.h"

SpriteManager g_sprites;

void SpriteManager::init() { generateLightTexture(); generateShadeTexture(); }
void SpriteManager::terminate()
{
    unload();
    m_shadeTexture = nullptr;
    m_lightTexture = nullptr;
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

    #if ENABLE_ENCRYPTION == 1
        ResourceManager::decrypt(m_spritesFile->m_data.data(), m_spritesFile->m_data.size());
    #endif

        m_signature = m_spritesFile->getU32();
        m_spritesCount = g_game.getFeature(Otc::GameSpritesU32) ? m_spritesFile->getU32() : m_spritesFile->getU16();
        m_spritesOffset = m_spritesFile->tell();
        m_loaded = true;
        g_lua.callGlobalField("g_sprites", "onLoadSpr", file);
        return true;
    } catch (stdext::exception& e) {
        g_logger.error(stdext::format("Failed to load sprites from '%s': %s", file, e.what()));
        return false;
    }
}

void SpriteManager::saveSpr(const std::string& fileName)
{
    if (!m_loaded)
        stdext::throw_exception("failed to save, spr is not loaded");

    try {
        const FileStreamPtr fin = g_resources.createFile(fileName);
        if (!fin)
            stdext::throw_exception(stdext::format("failed to open file '%s' for write", fileName));

        fin->cache();

        fin->addU32(m_signature);
        if (g_game.getFeature(Otc::GameSpritesU32))
            fin->addU32(m_spritesCount);
        else
            fin->addU16(m_spritesCount);

        const uint32 offset = fin->tell();
        uint32 spriteAddress = offset + 4 * m_spritesCount;
        for (int i = 1; i <= m_spritesCount; ++i)
            fin->addU32(0);

        for (int i = 1; i <= m_spritesCount; ++i) {
            m_spritesFile->seek((i - 1) * 4 + m_spritesOffset);
            const uint32 fromAdress = m_spritesFile->getU32();
            if (fromAdress != 0) {
                fin->seek(offset + (i - 1) * 4);
                fin->addU32(spriteAddress);
                fin->seek(spriteAddress);

                m_spritesFile->seek(fromAdress);
                fin->addU8(m_spritesFile->getU8());
                fin->addU8(m_spritesFile->getU8());
                fin->addU8(m_spritesFile->getU8());

                const uint16 dataSize = m_spritesFile->getU16();
                fin->addU16(dataSize);
                char spriteData[SPRITE_DATA_SIZE];
                m_spritesFile->read(spriteData, dataSize);
                fin->write(spriteData, dataSize);

                spriteAddress = fin->tell();
            }
            //TODO: Check for overwritten sprites.
        }

        fin->flush();
        fin->close();
    } catch (std::exception& e) {
        g_logger.error(stdext::format("Failed to save '%s': %s", fileName, e.what()));
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
    if (g_game.getClientVersion() >= 1281) {
        return g_spriteAppearances.getSpriteImage(id);
    }

    try {
        if (id == 0 || !m_spritesFile)
            return nullptr;

        m_spritesFile->seek(((id - 1) * 4) + m_spritesOffset);

        const uint32 spriteAddress = m_spritesFile->getU32();

        // no sprite? return an empty texture
        if (spriteAddress == 0)
            return nullptr;

        m_spritesFile->seek(spriteAddress);

        // skip color key
        m_spritesFile->getU8();
        m_spritesFile->getU8();
        m_spritesFile->getU8();

        const uint16 pixelDataSize = m_spritesFile->getU16();

        ImagePtr image(new Image(Size(SPRITE_SIZE, SPRITE_SIZE)));

        uint8* pixels = image->getPixelData();
        int writePos = 0;
        int read = 0;
        const bool useAlpha = g_game.getFeature(Otc::GameSpritesAlphaChannel);
        const uint8 channels = useAlpha ? 4 : 3;
        // decompress pixels
        while (read < pixelDataSize && writePos < SPRITE_DATA_SIZE) {
            const uint16 transparentPixels = m_spritesFile->getU16();
            const uint16 coloredPixels = m_spritesFile->getU16();

            for (int i = 0; i < transparentPixels && writePos < SPRITE_DATA_SIZE; ++i) {
                pixels[writePos + 0] = 0x00;
                pixels[writePos + 1] = 0x00;
                pixels[writePos + 2] = 0x00;
                pixels[writePos + 3] = 0x00;
                writePos += 4;
            }

            for (int i = 0; i < coloredPixels && writePos < SPRITE_DATA_SIZE; ++i) {
                pixels[writePos + 0] = m_spritesFile->getU8();
                pixels[writePos + 1] = m_spritesFile->getU8();
                pixels[writePos + 2] = m_spritesFile->getU8();

                const uint8 alphaColor = useAlpha ? m_spritesFile->getU8() : 0xFF;
                if (alphaColor != 0xFF)
                    image->setTransparentPixel(true);

                pixels[writePos + 3] = alphaColor;

                writePos += 4;
            }

            read += 4 + (channels * coloredPixels);
        }

        // Error margin for 4 pixel transparent
        if (!image->hasTransparentPixel() && writePos + 4 < SPRITE_DATA_SIZE)
            image->setTransparentPixel(true);

        // fill remaining pixels with alpha
        while (writePos < SPRITE_DATA_SIZE) {
            pixels[writePos + 0] = 0x00;
            pixels[writePos + 1] = 0x00;
            pixels[writePos + 2] = 0x00;
            pixels[writePos + 3] = 0x00;
            writePos += 4;
        }

        if (!image->hasTransparentPixel()) {
            // The image must be more than 4 pixels transparent to be considered transparent.
            uint8 cntTrans = 0;
            for (const uint8 pixel : image->getPixels()) {
                if (pixel == 0x00 && ++cntTrans > 4) {
                    image->setTransparentPixel(true);
                    break;
                }
            }
        }

        return image;
    } catch (stdext::exception& e) {
        g_logger.error(stdext::format("Failed to get sprite id %d: %s", id, e.what()));
        return nullptr;
    }
}

void SpriteManager::generateLightTexture()
{
    const float brightnessIntensity = 1.5f;

    const int bubbleRadius = 6,
        bubbleDiameter = bubbleRadius * 2.3;

    const auto lightImage = ImagePtr(new Image(Size(bubbleDiameter, bubbleDiameter)));
    for (int_fast16_t x = -1; ++x < bubbleDiameter;) {
        for (int_fast16_t y = -1; ++y < bubbleDiameter;) {
            const float radius = std::sqrt((bubbleRadius - x) * (bubbleRadius - x) + (bubbleRadius - y) * (bubbleRadius - y));
            const float intensity = std::clamp<float>(((bubbleRadius - radius) / bubbleRadius) * brightnessIntensity, .01f, 1.0f);

            // light intensity varies inversely with the square of the distance
            const uint8_t colorByte = intensity * 0xff;

            uint8_t pixel[4] = { colorByte, colorByte, colorByte, 0xff };
            lightImage->setPixel(x, y, pixel);
        }
    }

    m_lightTexture = TexturePtr(new Texture(lightImage));
    m_lightTexture->setSmooth(true);
}

void SpriteManager::generateShadeTexture()
{
    const uint16 diameter = 4;

    const auto image = ImagePtr(new Image(Size(diameter, diameter)));
    for (int_fast16_t x = -1; ++x < diameter;) {
        for (int_fast16_t y = -1; ++y < diameter;) {
            const uint8 alpha = x == 0 || y == 0 || x == diameter - 1 || y == diameter - 1 ? 0 : 0xff;
            uint8_t pixel[4] = { 0xff, 0xff, 0xff, alpha };
            image->setPixel(x, y, pixel);
        }
    }

    m_shadeTexture = TexturePtr(new Texture(image));
    m_shadeTexture->setSmooth(true);
}
