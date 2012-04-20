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


#include "image.h"

#include <framework/core/resourcemanager.h>
#include <framework/thirdparty/apngloader.h>

Image::Image(const Size& size, int bpp, uint8 *pixels)
{
    m_size = size;
    m_bpp = bpp;
    m_pixels.resize(size.area() * bpp);
    memcpy(&m_pixels[0], pixels, m_pixels.size());
}

ImagePtr Image::load(const std::string& file)
{
    ImagePtr image;
    try {
        // currently only png images are supported
        if(!boost::ends_with(file, ".png"))
            Fw::throwException("image file format no supported");

        // load image file data
        image = loadPNG(file);
    } catch(Exception& e) {
        logError("unable to load image '", file, "': ", e.what());
    }
    return image;
}

ImagePtr Image::loadPNG(const std::string& file)
{
    std::stringstream fin;
    g_resources.loadFile(file, fin);
    ImagePtr image;
    apng_data apng;
    if(load_apng(fin, &apng) == 0) {
        image = ImagePtr(new Image(Size(apng.width, apng.height), apng.bpp, apng.pdata));
        free_apng(&apng);
    }
    return image;
}
