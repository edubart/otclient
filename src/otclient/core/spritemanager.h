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
