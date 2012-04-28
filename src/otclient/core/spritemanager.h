/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include <framework/core/declarations.h>
#include <framework/graphics/texture.h>
#include <png.h>

#define SPRITE_WIDTH 32
#define SPRITE_HEIGHT 32
#define SPRITE_CHANNELS 4
#define SPRITE_SIZE SPRITE_WIDTH*SPRITE_HEIGHT*SPRITE_CHANNELS

/*
    Compression levels:
        None = 0
        Fastest = 1
        Best = 9
*/

#define PNG_COMPRESSION 6

class SpriteManager
{
public:
    SpriteManager();

    bool load(const std::string& file);
    void unload();
    void preloadSprites();

    uint32 getSignature() { return m_signature; }
    int getSpritesCount() { return m_spritesCount; }

    bool exportSprites();

    TexturePtr& getSpriteTexture(int id);
    bool isLoaded() { return m_loaded; }

private:
    TexturePtr loadSpriteTexture(int id);

    Boolean<false> m_loaded;
    uint32 m_signature;
    int m_spritesCount;
    std::vector<TexturePtr> m_sprites;
    TexturePtr m_transparentSprite;
    FileStreamPtr m_spritesFile;
};

extern SpriteManager g_sprites;

#endif
