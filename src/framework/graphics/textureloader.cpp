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
#include <graphics/textureloader.h>
#include <graphics/texture.h>
#include <util/apngloader.h>
#include "animatedtexture.h"

TexturePtr TextureLoader::loadPNG(std::stringstream& file)
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
