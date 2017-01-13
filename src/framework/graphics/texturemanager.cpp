/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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
#include "animatedtexture.h"
#include "graphics.h"
#include "image.h"

#include <framework/core/resourcemanager.h>
#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>
#include <framework/graphics/apngloader.h>

TextureManager g_textures;

void TextureManager::init()
{
    m_emptyTexture = TexturePtr(new Texture);
}

void TextureManager::terminate()
{
    if(m_liveReloadEvent) {
        m_liveReloadEvent->cancel();
        m_liveReloadEvent = nullptr;
    }
    m_textures.clear();
    m_animatedTextures.clear();
    m_emptyTexture = nullptr;
}

void TextureManager::poll()
{
    // update only every 16msec, this allows upto 60 fps for animated textures
    static ticks_t lastUpdate = 0;
    ticks_t now = g_clock.millis();
    if(now - lastUpdate < 16)
        return;
    lastUpdate = now;

    for(const AnimatedTexturePtr& animatedTexture : m_animatedTextures)
        animatedTexture->updateAnimation();
}

void TextureManager::clearCache()
{
    m_animatedTextures.clear();
    m_textures.clear();
}

void TextureManager::liveReload()
{
    if(m_liveReloadEvent)
        return;
    m_liveReloadEvent = g_dispatcher.cycleEvent([this] {
        for(auto& it : m_textures) {
            const std::string& path = g_resources.guessFilePath(it.first, "png");
            const TexturePtr& tex = it.second;
            if(tex->getTime() >= g_resources.getFileTime(path))
                continue;

            ImagePtr image = Image::load(path);
            if(!image)
                continue;
            tex->uploadPixels(image, tex->hasMipmaps());
            tex->setTime(stdext::time());
        }
    }, 1000);
}

TexturePtr TextureManager::getTexture(const std::string& fileName)
{
    TexturePtr texture;

    // before must resolve filename to full path
    std::string filePath = g_resources.resolvePath(fileName);

    // check if the texture is already loaded
    auto it = m_textures.find(filePath);
    if(it != m_textures.end()) {
        texture = it->second;
    }

    // texture not found, load it
    if(!texture) {
        try {
            std::string filePathEx = g_resources.guessFilePath(filePath, "png");

            // load texture file data
            std::stringstream fin;
            g_resources.readFileStream(filePathEx, fin);
            texture = loadTexture(fin);
        } catch(stdext::exception& e) {
            g_logger.error(stdext::format("Unable to load texture '%s': %s", fileName, e.what()));
            texture = g_textures.getEmptyTexture();
        }

        if(texture) {
            texture->setTime(stdext::time());
            texture->setSmooth(true);
            m_textures[filePath] = texture;
        }
    }

    return texture;
}

TexturePtr TextureManager::loadTexture(std::stringstream& file)
{
    TexturePtr texture;

    apng_data apng;
    if(load_apng(file, &apng) == 0) {
        Size imageSize(apng.width, apng.height);
        if(apng.num_frames > 1) { // animated texture
            std::vector<ImagePtr> frames;
            std::vector<int> framesDelay;
            for(uint i=0;i<apng.num_frames;++i) {
                uchar *frameData = apng.pdata + ((apng.first_frame+i) * imageSize.area() * apng.bpp);
                int frameDelay = apng.frames_delay[i];

                framesDelay.push_back(frameDelay);
                frames.push_back(ImagePtr(new Image(imageSize, apng.bpp, frameData)));
            }
            AnimatedTexturePtr animatedTexture = new AnimatedTexture(imageSize, frames, framesDelay);
            m_animatedTextures.push_back(animatedTexture);
            texture = animatedTexture;
        } else {
            ImagePtr image = ImagePtr(new Image(imageSize, apng.bpp, apng.pdata));
            texture = TexturePtr(new Texture(image));
        }
        free_apng(&apng);
    }

    return texture;
}
