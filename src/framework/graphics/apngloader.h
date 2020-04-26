/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#ifndef APNGLOADER_H
#define APNGLOADER_H

#include <sstream>

struct apng_data {
    unsigned char *pdata;
    unsigned int width;
    unsigned int height;
    unsigned int first_frame;
    unsigned int last_frame;
    unsigned char bpp;
    unsigned char coltype;
    unsigned int num_frames;
    unsigned int num_plays;
    unsigned short *frames_delay; // each frame delay in ms
};

// returns -1 on error, 0 on success
int load_apng(std::stringstream& file, struct apng_data *apng);
void save_png(std::stringstream& file, unsigned int width, unsigned int height, int channels, unsigned char *pixels);
void free_apng(struct apng_data *apng);

#endif
