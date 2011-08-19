#include "texturemanager.h"
#include "animatedtexture.h"

#include <framework/core/resourcemanager.h>
#include <framework/thirdparty/apngloader.h>

TextureManager g_textures;

TexturePtr TextureManager::getTexture(const std::string& textureFile)
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
        try {
            // currently only png textures are supported
            if(!boost::ends_with(textureFile, ".png"))
                throw std::runtime_error("texture file format no supported");

            // load texture file data
            std::stringstream fin;
            g_resources.loadFile(textureFile, fin);
            texture = loadPNG(fin);
        } catch(std::exception& e) {
            logError("ERROR: unable to load texture '",textureFile,"': ", e.what());
        }
    }

    return texture;
}

TexturePtr TextureManager::loadPNG(std::stringstream& file)
{
    TexturePtr texture;

    apng_data apng;
    if(load_apng(file, &apng) == 0) {
        if(apng.num_frames > 1) { // animated texture
            uchar *framesdata = apng.pdata + (apng.first_frame * apng.width * apng.height * apng.bpp);
            texture = TexturePtr(new AnimatedTexture(apng.width, apng.height, apng.bpp, apng.num_frames, framesdata, (int*)apng.frames_delay));
        } else
            texture = TexturePtr(new Texture(apng.width, apng.height, apng.bpp, apng.pdata));
        free_apng(&apng);
    }

    return texture;
}
