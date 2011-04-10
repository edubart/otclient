/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#include "textures.h"
#include "core/resources.h"
#include "textureloader.h"

Textures g_textures;

TexturePtr Textures::get(const std::string& textureFile)
{
    TexturePtr texture;

    // check if the texture is already loaded
    auto it = m_texturesMap.find(textureFile);
    if(it != m_texturesMap.end()) {
        if(it->second.expired())
            m_texturesMap.erase(it);
        else
            texture = it->second.lock();
    }

    // texture not found, load it
    if(!texture) {
        // currently only png textures are supported
        if(!boost::ends_with(textureFile, ".png"))
            logFatal("Unable to load texture %s, file format no supported.", textureFile.c_str());

        // load texture file data
        uint fileSize;
        uchar *textureFileData = g_resources.loadFile(textureFile, &fileSize);
        if(!textureFileData)
            logFatal("Unable to load texture %s, file could not be read.", textureFile.c_str());

        // load the texture
        texture = TexturePtr(TextureLoader::loadPNG(textureFileData));
        if(!texture)
            logFatal("Unable to load texture %s", textureFile.c_str());
        delete[] textureFileData;
    }

    return texture;
}
