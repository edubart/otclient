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


#include <prerequisites.h>
#include <graphics/textureloader.h>
#include <graphics/texture.h>

#include <png.h>

struct File
{
    File() {
        offset = 0;
    }

    uchar *data;
    uint offset;
};

void png_read_from_mem(png_structp png_ptr, png_bytep data, png_size_t size)
{
  File *file = (File*)png_get_io_ptr(png_ptr);

  /* Copy data from image buffer */
  memcpy(data, file->data + file->offset, size);

  /* Advance in the file */
  file->offset += size;
}

Texture *TextureLoader::loadPNG(uchar *fileData)
{
    File file;
    file.data = fileData;

    if(png_sig_cmp(file.data, 0, 8))
        return NULL;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr)
        return NULL;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }

    // Set "png_read" callback function and give source of data
    png_set_read_fn(png_ptr, (png_voidp *)&file, png_read_from_mem);

    png_read_info(png_ptr, info_ptr);

    int bitDepth = png_get_bit_depth(png_ptr, info_ptr);
    int colourType = png_get_color_type(png_ptr, info_ptr);

    if(colourType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    if(bitDepth < 8 && (colourType == PNG_COLOR_TYPE_GRAY || colourType == PNG_COLOR_TYPE_GRAY_ALPHA))
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    if(bitDepth < 8)
        png_set_packing(png_ptr);
    else if(bitDepth == 16)
        png_set_strip_16(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bitDepth, &colourType, NULL, NULL, NULL);

    int components;
    switch(colourType)
    {
        case PNG_COLOR_TYPE_GRAY:
            components = 1;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            components = 2;
            break;
        case PNG_COLOR_TYPE_RGB:
            components = 3;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            components = 4;
            break;
        default:
            if(png_ptr)
                png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return NULL;
    };

    uchar *pixels = new uchar[width * height * components];

    png_bytep *row_pointers = new png_bytep[height];
    for(uint i = 0; i < height; ++i)
        row_pointers[i] = (png_bytep)(pixels + (i * width * components));

    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, NULL);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    delete[] row_pointers;

    Texture *texture = new Texture(width, height, components, pixels);

    delete[] pixels;

    return texture;
}
