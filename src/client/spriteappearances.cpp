/*
 * Copyright (c) 2022 Nekiro <https://github.com/nekiro>
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

#include "spriteappearances.h"
#include "game.h"
#include <framework/core/filestream.h>
#include <framework/core/resourcemanager.h>
#include <framework/graphics/image.h>

#include <framework/core/asyncdispatcher.h>
#include <nlohmann/json.hpp>

#include "lzma.h"

 // warnings related to protobuf
 // https://android.googlesource.com/platform/external/protobuf/+/brillo-m9-dev/vsprojects/readme.txt

using json = nlohmann::json;

SpriteAppearances g_spriteAppearances;

void SpriteAppearances::init()
{
    // in tibia 12.81 there is currently 3482 sheets
    m_sheets.reserve(4000);
}

void SpriteAppearances::terminate()
{
    unload();
}

bool SpriteAppearances::loadSpriteSheet(const SpriteSheetPtr& sheet)
{
    std::lock_guard lock(sheet->mutex);

    if (sheet->loaded) {
        return true;
    }

    try {
        const FileStreamPtr& fin = g_resources.openFile(stdext::format("/things/%d/%s", g_game.getClientVersion(), sheet->file));
        fin->cache();

        const auto decompressed = std::make_unique<uint8_t[]>(LZMA_UNCOMPRESSED_SIZE); // uncompressed size, bmp file + 122 bytes header

         /*
            CIP's header, always 32 (0x20) bytes.
            Header format:
            [0x00, X):          A variable number of NULL (0x00) bytes. The amount of pad-bytes can vary depending on how many
                                bytes the "7-bit integer encoded LZMA file size" take.
            [X, X + 0x05):      The constant byte sequence [0x70 0x0A 0xFA 0x80 0x24]
            [X + 0x05, 0x20]:   LZMA file size (Note: excluding the 32 bytes of this header) encoded as a 7-bit integer
        */

        while (fin->getU8() == 0x00);
        fin->skip(4);
        while ((fin->getU8() & 0x80) == 0x80);

        const uint8_t lclppb = fin->getU8();

        lzma_options_lzma options{};
        options.lc = lclppb % 9;

        const int remainder = lclppb / 9;
        options.lp = remainder % 5;
        options.pb = remainder / 5;

        uint32_t dictionarySize = 0;
        for (uint8_t i = 0; i < 4; ++i) {
            dictionarySize += fin->getU8() << (i * 8);
        }

        options.dict_size = dictionarySize;

        fin->skip(8); // cip compressed size

        lzma_stream stream = LZMA_STREAM_INIT;

        const lzma_filter filters[2] = {
            lzma_filter{LZMA_FILTER_LZMA1, &options},
            lzma_filter{LZMA_VLI_UNKNOWN, nullptr}
        };

        lzma_ret ret = lzma_raw_decoder(&stream, filters);
        if (ret != LZMA_OK) {
            throw stdext::exception(stdext::format("failed to initialize lzma raw decoder result: %d", ret));
        }

        stream.next_in = &fin->m_data.data()[fin->tell()];
        stream.next_out = decompressed.get();
        stream.avail_in = fin->size();
        stream.avail_out = LZMA_UNCOMPRESSED_SIZE;

        ret = lzma_code(&stream, LZMA_RUN);
        if (ret != LZMA_STREAM_END) {
            throw stdext::exception(stdext::format("failed to decode lzma buffer result: %d", ret));
        }

        lzma_end(&stream); // free memory

        // pixel data start (bmp header end offset)
        uint32_t data;
        std::memcpy(&data, decompressed.get() + 10, sizeof(uint32_t));

        uint8_t* bufferStart = decompressed.get() + data;

        // reverse channels
        for (uint8* itr = bufferStart; itr < bufferStart + BYTES_IN_SPRITE_SHEET; itr += 4) {
            std::swap(*(itr + 0), *(itr + 2));
        }

        // flip vertically
        for (int y = 0; y < 192; ++y) {
            uint8* itr1 = &bufferStart[y * SPRITE_SHEET_WIDTH_BYTES];
            uint8* itr2 = &bufferStart[(384 - y - 1) * SPRITE_SHEET_WIDTH_BYTES];

            for (std::size_t x = 0; x < SPRITE_SHEET_WIDTH_BYTES; ++x) {
                std::swap(*(itr1 + x), *(itr2 + x));
            }
        }

        // fix magenta
        for (int offset = 0; offset < BYTES_IN_SPRITE_SHEET; offset += 4) {
            std::memcpy(&data, bufferStart + offset, 4);
            if (data == 0xFF00FF) {
                std::memset(bufferStart + offset, 0x00, 4);
            }
        }

        sheet->data = std::make_unique<uint8_t[]>(LZMA_UNCOMPRESSED_SIZE);
        std::memcpy(sheet->data.get(), bufferStart, BYTES_IN_SPRITE_SHEET);

        sheet->loaded = true;
        sheet->loading = false;
        return true;
    } catch (std::exception& e) {
        g_logger.error(stdext::format("Failed to load single sprite sheet '%s': %s", sheet->file, e.what()));
        return false;
    }
}

void SpriteAppearances::unload()
{
    m_spritesCount = 0;
    m_sheets.clear();
}

SpriteSheetPtr SpriteAppearances::getSheetBySpriteId(int id, bool load /* = true */)
{
    if (id == 0) {
        return nullptr;
    }

    // find sheet
    const auto sheetIt = std::find_if(m_sheets.begin(), m_sheets.end(), [=](const SpriteSheetPtr& sheet) {
        return id >= sheet->firstId && id <= sheet->lastId;
    });

    if (sheetIt == m_sheets.end()) {
        return nullptr;
    }

    const SpriteSheetPtr& sheet = *sheetIt;

    if (!sheet->loaded && load) {
        if (!sheet->loading) {
            sheet->loading = true;
            g_asyncDispatcher.dispatch([this, &sheet] {
                loadSpriteSheet(sheet);
            });
        }

        return nullptr;
    }

    return sheet;
}

ImagePtr SpriteAppearances::getSpriteImage(int id)
{
    try {
        const SpriteSheetPtr sheet = getSheetBySpriteId(id);
        if (!sheet) {
            return nullptr;
        }

        const Size& size = sheet->getSpriteSize();

        ImagePtr image(new Image(size));
        uint8* pixelData = image->getPixelData();

        const int spriteOffset = id - sheet->firstId;
        const int allColumns = size.width() == 32 ? 12 : 6; // 64 pixel width == 6 columns each 64x or 32 pixels, 12 columns
        const int spriteRow = std::floor(static_cast<float>(spriteOffset) / static_cast<float>(allColumns));
        const int spriteColumn = spriteOffset % allColumns;

        const int spriteWidthBytes = size.width() * 4;

        for (int height = size.height() * spriteRow, offset = 0; height < size.height() + (spriteRow * size.height()); height++, offset++) {
            std::memcpy(&pixelData[offset * spriteWidthBytes], &sheet->data[(height * SPRITE_SHEET_WIDTH_BYTES) + (spriteColumn * spriteWidthBytes)], spriteWidthBytes);
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

void SpriteAppearances::saveSpriteToFile(int id, const std::string& file)
{
    const ImagePtr sprite = getSpriteImage(id);
    if (sprite) {
        sprite->savePNG(file);
    }
}

void SpriteAppearances::saveSheetToFileBySprite(int id, const std::string& file)
{
    const SpriteSheetPtr sheet = getSheetBySpriteId(id);
    if (sheet) {
        Image image(Size(384, 384), 4, sheet->data.get());
        image.savePNG(file);
    }
}

void SpriteAppearances::saveSheetToFile(const SpriteSheetPtr& sheet, const std::string& file)
{
    Image image(Size(384, 384), 4, sheet->data.get());
    image.savePNG(file);
}
