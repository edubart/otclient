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


#include "texturemanager.h"
#include "resourcemanager.h"
#include "textureloader.h"

TextureManager g_textures;

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
    m_texturesMap.clear();
}

TexturePtr TextureManager::get(const std::string& textureFile)
{
    TexturePtr texture;

    // check if the texture is already loaded
    TexturesMap::iterator it = m_texturesMap.find(textureFile);
    if(it != m_texturesMap.end())
        texture = it->second;
    else { // load texture
        // currently only png textures are supported
        if(!boost::ends_with(textureFile, ".png")) {
            error("Unable to load texture %s, file format no supported.", textureFile.c_str());
            return texture;
        }

        unsigned int fileSize;
        unsigned char *textureFileData = g_resources.loadFile(textureFile, &fileSize);
        if(!textureFileData)
            return texture;

        texture = TexturePtr(TextureLoader::loadPNG(textureFileData, fileSize));
        if(!texture)
            error("Unable to load texture %s, loading error.", textureFile.c_str());
        delete[] textureFileData;
    }

    return texture;
}
