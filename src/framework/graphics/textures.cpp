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


#include <global.h>
#include <core/resources.h>
#include <graphics/textures.h>
#include <graphics/textureloader.h>
#include <core/dispatcher.h>

#include <boost/algorithm/string.hpp>

Textures g_textures;

TexturePtr Textures::get(const std::string& textureFile)
{
    TexturePtr texture;

    // check if the texture is already loaded
    auto it = m_textures.find(textureFile);
    if(it != m_textures.end()) {
        if(it->second.expired())
            m_textures.erase(it);
        else
            texture = it->second.lock();
    }

    // texture not found, load it
    if(!texture) {
        // currently only png textures are supported
        if(!boost::ends_with(textureFile, ".png")) {
            logError("ERROR: Unable to load texture '",textureFile,"', file format no supported.");
            return texture;
        }

        // load texture file data
        std::stringstream fin;
        if(!g_resources.loadFile(textureFile, fin)) {
            logError("ERROR: Unable to load texture '",textureFile,"', file could not be read.");
            return texture;
        }


        // load the texture
        texture = TexturePtr(TextureLoader::loadPNG(fin));
        if(!texture)
            logError("ERROR: Unable to load texture '",textureFile,"'");
    }
    return texture;
}
