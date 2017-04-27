/*
 * Based on APNG Disassembler 2.3
 *
 * Copyright (c) 2009 Max Stepin
 * maxst at users.sourceforge.net
 *
 * GNU LGPL information
 * --------------------
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <zlib.h>
#include "apngloader.h"
#include <iostream>
#include <sstream>
#include <fstream>

#if defined(_MSC_VER) && _MSC_VER >= 1300
#define swap16(data) _byteswap_ushort(data)
#define swap32(data) _byteswap_ulong(data)
#elif __linux__
#include <byteswap.h>
#define swap16(data) bswap_16(data)
#define swap32(data) bswap_32(data)
#else
#define swap16(data) (((data >> 8) & 255) | ((data & 255) << 8))
#define swap32(data) ((swap16(data) << 16) | swap16(data >> 16))
#endif

#define PNG_ZBUF_SIZE  32768

#define PNG_DISPOSE_OP_NONE        0x00
#define PNG_DISPOSE_OP_BACKGROUND  0x01
#define PNG_DISPOSE_OP_PREVIOUS    0x02

#define PNG_BLEND_OP_SOURCE        0x00
#define PNG_BLEND_OP_OVER          0x01

#define notabc(c) ((c) < 65 || (c) > 122 || ((c) > 90 && (c) < 97))

#define ROWBYTES(pixel_bits, width) \
((pixel_bits) >= 8 ? \
((width) * (((unsigned int)(pixel_bits)) >> 3)) : \
(( ((width) * ((unsigned int)(pixel_bits))) + 7) >> 3) )

unsigned char   png_sign[8] = {137, 80, 78, 71, 13, 10, 26, 10};

int mask4[2]={240,15};
int shift4[2]={4,0};

int mask2[4]={192,48,12,3};
int shift2[4]={6,4,2,0};

int mask1[8]={128,64,32,16,8,4,2,1};
int shift1[8]={7,6,5,4,3,2,1,0};

unsigned int    keep_original = 1;
unsigned char   pal[256][3];
unsigned char   trns[256];
unsigned int    palsize, trnssize;
unsigned int    hasTRNS;
unsigned short  trns1, trns2, trns3;

unsigned int read32(std::istream& f1)
{
    unsigned char a, b, c, d;
    f1.read((char*)&a, 1);
    f1.read((char*)&b, 1);
    f1.read((char*)&c, 1);
    f1.read((char*)&d, 1);
    return ((unsigned int)a<<24)+((unsigned int)b<<16)+((unsigned int)c<<8)+(unsigned int)d;
}

unsigned short read16(std::istream& f1)
{
    unsigned char a, b;
    f1.read((char*)&a, 1);
    f1.read((char*)&b, 1);
    return ((unsigned short)a<<8)+(unsigned short)b;
}

unsigned short readshort(unsigned char * p)
{
    return ((unsigned short)(*p)<<8)+(unsigned short)(*(p+1));
}

void read_sub_row(unsigned char * row, unsigned int rowbytes, unsigned int bpp)
{
    unsigned int i;

    for (i=bpp; i<rowbytes; i++)
        row[i] += row[i-bpp];
}

void read_up_row(unsigned char * row, unsigned char * prev_row, unsigned int rowbytes, unsigned int bpp)
{
    unsigned int i;

    if (prev_row)
        for (i=0; i<rowbytes; i++)
            row[i] += prev_row[i];
}

void read_average_row(unsigned char * row, unsigned char * prev_row, unsigned int rowbytes, unsigned int bpp)
{
    unsigned int i;

    if (prev_row)
    {
        for (i=0; i<bpp; i++)
            row[i] += prev_row[i]>>1;
        for (i=bpp; i<rowbytes; i++)
            row[i] += (prev_row[i] + row[i-bpp])>>1;
    }
    else
    {
        for (i=bpp; i<rowbytes; i++)
            row[i] += row[i-bpp]>>1;
    }
}

void read_paeth_row(unsigned char * row, unsigned char * prev_row, unsigned int rowbytes, unsigned int bpp)
{
    unsigned int i;
    int a, b, c, pa, pb, pc, p;

    if (prev_row)
    {
        for (i=0; i<bpp; i++)
            row[i] += prev_row[i];
        for (i=bpp; i<rowbytes; i++)
        {
            a = row[i-bpp];
            b = prev_row[i];
            c = prev_row[i-bpp];
            p = b - c;
            pc = a - c;
            pa = abs(p);
            pb = abs(pc);
            pc = abs(p + pc);
            row[i] += ((pa <= pb && pa <= pc) ? a : (pb <= pc) ? b : c);
        }
    }
    else
    {
        for (i=bpp; i<rowbytes; i++)
            row[i] += row[i-bpp];
    }
}

void unpack(z_stream& zstream, unsigned char * dst, unsigned int dst_size, unsigned char * src, unsigned int src_size, unsigned int h, unsigned int rowbytes, unsigned char bpp)
{
    unsigned int    j;
    unsigned char * row = dst;
    unsigned char * prev_row = NULL;

    zstream.next_out  = dst;
    zstream.avail_out = dst_size;
    zstream.next_in   = src;
    zstream.avail_in  = src_size;
    inflate(&zstream, Z_FINISH);
    inflateReset(&zstream);

    for (j=0; j<h; j++)
    {
        switch (*row++)
        {
            case 0: break;
            case 1: read_sub_row(row, rowbytes, bpp); break;
            case 2: read_up_row(row, prev_row, rowbytes, bpp); break;
            case 3: read_average_row(row, prev_row, rowbytes, bpp); break;
            case 4: read_paeth_row(row, prev_row, rowbytes, bpp); break;
        }
        prev_row = row;
        row += rowbytes;
    }
}

void compose0(unsigned char * dst1, unsigned int dstbytes1, unsigned char * dst2, unsigned int dstbytes2, unsigned char * src, unsigned int srcbytes, unsigned int w, unsigned int h, unsigned int bop, unsigned char depth)
{
    unsigned int    i, j, g, a;
    unsigned char * sp;
    unsigned char * dp1;
    unsigned int  * dp2;

    for (j=0; j<h; j++)
    {
        sp = src+1;
        dp1 = dst1;
        dp2 = (unsigned int*)dst2;

        if (bop == PNG_BLEND_OP_SOURCE)
        {
            switch (depth)
            {
                case 16: for (i=0; i<w; i++) { a = 0xFF; if (hasTRNS && readshort(sp)==trns1) a = 0; *dp1++ = *sp; *dp2++ = (a << 24) + (*sp << 16) + (*sp << 8) + *sp; sp+=2; }  break;
                case 8:  for (i=0; i<w; i++) { a = 0xFF; if (hasTRNS && *sp==trns1)           a = 0; *dp1++ = *sp; *dp2++ = (a << 24) + (*sp << 16) + (*sp << 8) + *sp; sp++;  }  break;
                case 4:  for (i=0; i<w; i++) { g = (sp[i>>1] & mask4[i&1]) >> shift4[i&1]; a = 0xFF; if (hasTRNS && g==trns1) a = 0; *dp1++ = g*0x11; *dp2++ = (a<<24) + g*0x111111; } break;
                case 2:  for (i=0; i<w; i++) { g = (sp[i>>2] & mask2[i&3]) >> shift2[i&3]; a = 0xFF; if (hasTRNS && g==trns1) a = 0; *dp1++ = g*0x55; *dp2++ = (a<<24) + g*0x555555; } break;
                case 1:  for (i=0; i<w; i++) { g = (sp[i>>3] & mask1[i&7]) >> shift1[i&7]; a = 0xFF; if (hasTRNS && g==trns1) a = 0; *dp1++ = g*0xFF; *dp2++ = (a<<24) + g*0xFFFFFF; } break;
            }
        }
        else /* PNG_BLEND_OP_OVER */
        {
            switch (depth)
            {
                case 16: for (i=0; i<w; i++, dp1++, dp2++) { if (readshort(sp) != trns1) { *dp1 = *sp; *dp2 = 0xFF000000 + (*sp << 16) + (*sp << 8) + *sp; } sp+=2; } break;
                case 8:  for (i=0; i<w; i++, dp1++, dp2++) { if (*sp != trns1)           { *dp1 = *sp; *dp2 = 0xFF000000 + (*sp << 16) + (*sp << 8) + *sp; } sp++;  } break;
                case 4:  for (i=0; i<w; i++, dp1++, dp2++) { g = (sp[i>>1] & mask4[i&1]) >> shift4[i&1]; if (g != trns1) { *dp1 = g*0x11; *dp2 = 0xFF000000+g*0x111111; } } break;
                case 2:  for (i=0; i<w; i++, dp1++, dp2++) { g = (sp[i>>2] & mask2[i&3]) >> shift2[i&3]; if (g != trns1) { *dp1 = g*0x55; *dp2 = 0xFF000000+g*0x555555; } } break;
                case 1:  for (i=0; i<w; i++, dp1++, dp2++) { g = (sp[i>>3] & mask1[i&7]) >> shift1[i&7]; if (g != trns1) { *dp1 = g*0xFF; *dp2 = 0xFF000000+g*0xFFFFFF; } } break;
            }
        }

        src += srcbytes;
        dst1 += dstbytes1;
        dst2 += dstbytes2;
    }
}

void compose2(unsigned char * dst1, unsigned int dstbytes1, unsigned char * dst2, unsigned int dstbytes2, unsigned char * src, unsigned int srcbytes, unsigned int w, unsigned int h, unsigned int bop, unsigned char depth)
{
    unsigned int    i, j;
    unsigned int    r, g, b, a;
    unsigned char * sp;
    unsigned char * dp1;
    unsigned int  * dp2;

    for (j=0; j<h; j++)
    {
        sp = src+1;
        dp1 = dst1;
        dp2 = (unsigned int*)dst2;

        if (bop == PNG_BLEND_OP_SOURCE)
        {
            if (depth == 8)
            {
                for (i=0; i<w; i++)
                {
                    b = *sp++;
                    g = *sp++;
                    r = *sp++;
                    a = 0xFF;
                    if (hasTRNS && b==trns1 && g==trns2 && r==trns3)
                        a = 0;
                    *dp1++ = b; *dp1++ = g; *dp1++ = r;
                    *dp2++ = (a << 24) + (r << 16) + (g << 8) + b;
                }
            }
            else
            {
                for (i=0; i<w; i++, sp+=6)
                {
                    b = *sp;
                    g = *(sp+2);
                    r = *(sp+4);
                    a = 0xFF;
                    if (hasTRNS && readshort(sp)==trns1 && readshort(sp+2)==trns2 && readshort(sp+4)==trns3)
                        a = 0;
                    *dp1++ = b; *dp1++ = g; *dp1++ = r;
                    *dp2++ = (a << 24) + (r << 16) + (g << 8) + b;
                }
            }
        }
        else /* PNG_BLEND_OP_OVER */
        {
            if (depth == 8)
            {
                for (i=0; i<w; i++, sp+=3, dp1+=3, dp2++)
                    if ((*sp != trns1) || (*(sp+1) != trns2) || (*(sp+2) != trns3))
                    {
                        *dp1 = *sp; *(dp1+1) = *(sp+1); *(dp1+2) = *(sp+2);
                        *dp2 = 0xFF000000 + (*(sp+2) << 16) + (*(sp+1) << 8) + *sp;
                    }
            }
            else
            {
                for (i=0; i<w; i++, sp+=6, dp1+=3, dp2++)
                    if ((readshort(sp) != trns1) || (readshort(sp+2) != trns2) || (readshort(sp+4) != trns3))
                    {
                        *dp1 = *sp; *(dp1+1) = *(sp+2); *(dp1+2) = *(sp+4);
                        *dp2 = 0xFF000000 + (*(sp+4) << 16) + (*(sp+2) << 8) + *sp;
                    }
            }
        }
        src += srcbytes;
        dst1 += dstbytes1;
        dst2 += dstbytes2;
    }
}

void compose3(unsigned char * dst1, unsigned int dstbytes1, unsigned char * dst2, unsigned int dstbytes2, unsigned char * src, unsigned int srcbytes, unsigned int w, unsigned int h, unsigned int bop, unsigned char depth)
{
    unsigned int    i, j;
    unsigned int    r, g, b, a;
    unsigned int    r2, g2, b2, a2;
    int             u, v, al;
    unsigned char   col = 0;
    unsigned char * sp;
    unsigned char * dp1;
    unsigned int  * dp2;

    for (j=0; j<h; j++)
    {
        sp = src+1;
        dp1 = dst1;
        dp2 = (unsigned int*)dst2;

        for (i=0; i<w; i++)
        {
            switch (depth)
            {
                case 8: col = sp[i]; break;
                case 4: col = (sp[i>>1] & mask4[i&1]) >> shift4[i&1]; break;
                case 2: col = (sp[i>>2] & mask2[i&3]) >> shift2[i&3]; break;
                case 1: col = (sp[i>>3] & mask1[i&7]) >> shift1[i&7]; break;
            }

            b = pal[col][0];
            g = pal[col][1];
            r = pal[col][2];
            a = trns[col];

            if (bop == PNG_BLEND_OP_SOURCE)
            {
                *dp1++ = col;
                *dp2++ = (a << 24) + (r << 16) + (g << 8) + b;
            }
            else /* PNG_BLEND_OP_OVER */
            {
                if (a == 255)
                {
                    *dp1++ = col;
                    *dp2++ = (a << 24) + (r << 16) + (g << 8) + b;
                }
                else
                    if (a != 0)
                    {
                        if ((a2 = (*dp2)>>24))
                        {
                            keep_original = 0;
                            u = a*255;
                            v = (255-a)*a2;
                            al = 255*255-(255-a)*(255-a2);
                            b2 = ((*dp2)&255);
                            g2 = (((*dp2)>>8)&255);
                            r2 = (((*dp2)>>16)&255);
                            b = (b*u + b2*v)/al;
                            g = (g*u + g2*v)/al;
                            r = (r*u + r2*v)/al;
                            a = al/255;
                        }
                        *dp1++ = col;
                        *dp2++ = (a << 24) + (r << 16) + (g << 8) + b;
                    }
                    else
                    {
                        dp1++;
                        dp2++;
                    }
            }
        }
        src += srcbytes;
        dst1 += dstbytes1;
        dst2 += dstbytes2;
    }
}

void compose4(unsigned char * dst, unsigned int dstbytes, unsigned char * src, unsigned int srcbytes, unsigned int w, unsigned int h, unsigned int bop, unsigned char depth)
{
    unsigned int    i, j, step;
    unsigned int    g, a, g2, a2;
    int             u, v, al;
    unsigned char * sp;
    unsigned char * dp;

    step = (depth+7)/8;

    for (j=0; j<h; j++)
    {
        sp = src+1;
        dp = dst;

        if (bop == PNG_BLEND_OP_SOURCE)
        {
            for (i=0; i<w; i++)
            {
                g = *sp; sp += step;
                a = *sp; sp += step;
                *dp++ = g;
                *dp++ = a;
            }
        }
        else /* PNG_BLEND_OP_OVER */
        {
            for (i=0; i<w; i++)
            {
                g = *sp; sp += step;
                a = *sp; sp += step;
                if (a == 255)
                {
                    *dp++ = g;
                    *dp++ = a;
                }
                else
                    if (a != 0)
                    {
                        if ((a2 = *(dp+1)))
                        {
                            u = a*255;
                            v = (255-a)*a2;
                            al = 255*255-(255-a)*(255-a2);
                            g2 = ((*dp)&255);
                            g = (g*u + g2*v)/al;
                            a = al/255;
                        }
                        *dp++ = g;
                        *dp++ = a;
                    }
                    else
                        dp+=2;
            }
        }
        src += srcbytes;
        dst += dstbytes;
    }
}

void compose6(unsigned char * dst, unsigned int dstbytes, unsigned char * src, unsigned int srcbytes, unsigned int w, unsigned int h, unsigned int bop, unsigned char depth)
{
    unsigned int    i, j, step;
    unsigned int    r, g, b, a;
    unsigned int    r2, g2, b2, a2;
    int             u, v, al;
    unsigned char * sp;
    unsigned int  * dp;

    step = (depth+7)/8;

    for (j=0; j<h; j++)
    {
        sp = src+1;
        dp = (unsigned int*)dst;

        if (bop == PNG_BLEND_OP_SOURCE)
        {
            for (i=0; i<w; i++)
            {
                b = *sp; sp += step;
                g = *sp; sp += step;
                r = *sp; sp += step;
                a = *sp; sp += step;
                *dp++ = (a << 24) + (r << 16) + (g << 8) + b;
            }
        }
        else /* PNG_BLEND_OP_OVER */
        {
            for (i=0; i<w; i++)
            {
                b = *sp; sp += step;
                g = *sp; sp += step;
                r = *sp; sp += step;
                a = *sp; sp += step;
                if (a == 255)
                    *dp++ = (a << 24) + (r << 16) + (g << 8) + b;
                else
                    if (a != 0)
                    {
                        if ((a2 = (*dp)>>24))
                        {
                            u = a*255;
                            v = (255-a)*a2;
                            al = 255*255-(255-a)*(255-a2);
                            b2 = ((*dp)&255);
                            g2 = (((*dp)>>8)&255);
                            r2 = (((*dp)>>16)&255);
                            b = (b*u + b2*v)/al;
                            g = (g*u + g2*v)/al;
                            r = (r*u + r2*v)/al;
                            a = al/255;
                        }
                        *dp++ = (a << 24) + (r << 16) + (g << 8) + b;
                    }
                    else
                        dp++;
            }
        }
        src += srcbytes;
        dst += dstbytes;
    }
}

int load_apng(std::stringstream& file, struct apng_data *apng)
{
    unsigned int    i, j;
    unsigned int    rowbytes;
    int             imagesize, zbuf_size, zsize, trns_idx;
    unsigned int    len, chunk/*, crc, seq*/;
    unsigned int    w, h, w0, h0, x0, y0;
    unsigned int    frames, loops, first_frame, cur_frame;
    unsigned int    outrow1, outrow2, outimg1, outimg2;
    unsigned short  d1, d2;
    unsigned char   c, dop, bop;
    unsigned char   channels, depth, pixeldepth, bpp;
    unsigned char   coltype, compr, filter, interl;
    z_stream        zstream;
    memset(apng, 0, sizeof(struct apng_data));

    for (i=0; i<256; i++)
    {
        pal[i][0] = i;
        pal[i][1] = i;
        pal[i][2] = i;
        trns[i] = 255;
    }

    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
    inflateInit(&zstream);

    frames = 1;
    first_frame = 0;
    cur_frame = 0;
    zsize = 0;
    hasTRNS = 0;
    trns_idx = -1;
    x0 = 0;
    y0 = 0;
    loops = 0;
    bop = PNG_BLEND_OP_SOURCE;

    unsigned char sig[8];
    unsigned char * pOut1;
    unsigned char * pOut2;
    unsigned char * pTemp;
    unsigned char * pData;
    unsigned char * pImg1;
    unsigned char * pImg2;
    unsigned char * pDst1;
    unsigned char * pDst2;
    unsigned short* frames_delay;

    file.read((char*)sig, 8);
    if(!file.eof() && memcmp(sig, png_sign, 8) == 0) {
        len  = read32(file);
        chunk = read32(file);

        if ((len == 13) && (chunk == 0x49484452)) /* IHDR */
        {
            w = w0 = read32(file);
            h = h0 = read32(file);
            file.read((char*)&depth, 1);
            file.read((char*)&coltype, 1);
            file.read((char*)&compr, 1);
            file.read((char*)&filter, 1);
            file.read((char*)&interl, 1);
            /*crc = */read32(file);

            channels = 1;
            if (coltype == 2)
                channels = 3;
            else if (coltype == 4)
                channels = 2;
            else if (coltype == 6)
                channels = 4;

            pixeldepth = depth*channels;
            bpp = (pixeldepth + 7) >> 3;
            rowbytes = ROWBYTES(pixeldepth, w);

            imagesize = (rowbytes + 1) * h;
            zbuf_size = imagesize + ((imagesize + 7) >> 3) + ((imagesize + 63) >> 6) + 11;

            /*
                * We'll render into 2 output buffers, first in original coltype,
                * second in RGBA.
                *
                * It's better to try to keep the original coltype, but if dispose/blend
                * operations will make it impossible, then we'll save RGBA version instead.
                */

            outrow1 = w*channels; /* output coltype = input coltype */
            outrow2 = w*4;        /* output coltype = RGBA          */
            outimg1 = h*outrow1;
            outimg2 = h*outrow2;

            pOut1=(unsigned char *)malloc(outimg1);
            pOut2=(unsigned char *)malloc(outimg2);
            pTemp=(unsigned char *)malloc(imagesize);
            pData=(unsigned char *)malloc(zbuf_size);
            pImg1=pOut1;
            pImg2=pOut2;
            frames_delay = NULL;

            /* apng decoding - begin */
            memset(pOut1, 0, outimg1);
            memset(pOut2, 0, outimg2);

            while (!file.eof())
            {
                len  = read32(file);
                chunk = read32(file);

                if (chunk == 0x504C5445) /* PLTE */
                {
                    unsigned int col;
                    for (i=0; i<len; i++)
                    {
                        file.read((char*)&c, 1);
                        col = i/3;
                        if (col<256)
                        {
                            pal[col][i%3] = c;
                            palsize = col+1;
                        }
                    }
                    /*crc = */read32(file);
                }
                else if (chunk == 0x74524E53) /* tRNS */
                {
                    hasTRNS = 1;
                    for (i=0; i<len; i++)
                    {
                        file.read((char*)&c, 1);
                        if (i<256)
                        {
                            trns[i] = c;
                            trnssize = i+1;
                            if (c == 0 && coltype == 3 && trns_idx == -1)
                                trns_idx = i;
                        }
                    }
                    if (coltype == 0)
                    {
                        trns1 = readshort(&trns[0]);
                        if (depth == 16)
                        {
                            trns[1] = trns[0]; trns[0] = 0;
                        }
                    }
                    else
                        if (coltype == 2)
                        {
                            trns1 = readshort(&trns[0]);
                            trns2 = readshort(&trns[2]);
                            trns3 = readshort(&trns[4]);
                            if (depth == 16)
                            {
                                trns[1] = trns[0]; trns[0] = 0;
                                trns[3] = trns[2]; trns[2] = 0;
                                trns[5] = trns[4]; trns[4] = 0;
                            }
                        }
                        /*crc = */read32(file);
                }
                else if (chunk == 0x6163544C) /* acTL */
                {
                    frames = read32(file);
                    if(frames_delay)
                        free(frames_delay);
                    frames_delay = (unsigned short*)malloc(frames*sizeof(int));
                    loops  = read32(file);
                    /*crc = */read32(file);
                    if (pOut1)
                        free(pOut1);
                    if (pOut2)
                        free(pOut2);
                    pOut1 = (unsigned char *)malloc((frames+1)*outimg1);
                    pOut2 = (unsigned char *)malloc((frames+1)*outimg2);
                    pImg1 = pOut1;
                    pImg2 = pOut2;
                    memset(pOut1, 0, outimg1);
                    memset(pOut2, 0, outimg2);
                }
                else if (chunk == 0x6663544C) /* fcTL */
                {
                    if (zsize == 0)
                        first_frame = 1;
                    else
                    {
                        if (dop == PNG_DISPOSE_OP_PREVIOUS)
                        {
                            if (coltype != 6)
                                memcpy(pImg1 + outimg1, pImg1, outimg1);
                            if (coltype != 4)
                                memcpy(pImg2 + outimg2, pImg2, outimg2);
                        }

                        pDst1 = pImg1 + y0*outrow1 + x0*channels;
                        pDst2 = pImg2 + y0*outrow2 + x0*4;
                        unpack(zstream, pTemp, imagesize, pData, zsize, h0, rowbytes, bpp);
                        switch (coltype)
                        {
                            case 0: compose0(pDst1, outrow1, pDst2, outrow2, pTemp, rowbytes+1, w0, h0, bop, depth); break;
                            case 2: compose2(pDst1, outrow1, pDst2, outrow2, pTemp, rowbytes+1, w0, h0, bop, depth); break;
                            case 3: compose3(pDst1, outrow1, pDst2, outrow2, pTemp, rowbytes+1, w0, h0, bop, depth); break;
                            case 4: compose4(pDst1, outrow1,                 pTemp, rowbytes+1, w0, h0, bop, depth); break;
                            case 6: compose6(                pDst2, outrow2, pTemp, rowbytes+1, w0, h0, bop, depth); break;
                        }
                        zsize = 0;

                        if (dop != PNG_DISPOSE_OP_PREVIOUS)
                        {
                            if (coltype != 6)
                                memcpy(pImg1 + outimg1, pImg1, outimg1);
                            if (coltype != 4)
                                memcpy(pImg2 + outimg2, pImg2, outimg2);

                            if (dop == PNG_DISPOSE_OP_BACKGROUND)
                            {
                                pDst1 += outimg1;
                                pDst2 += outimg2;

                                for (j=0; j<h0; j++)
                                {
                                    switch (coltype)
                                    {
                                        case 0:  memset(pDst2, 0, w0*4); if (hasTRNS) memset(pDst1, trns[1], w0); else keep_original = 0; break;
                                        case 2:  memset(pDst2, 0, w0*4); if (hasTRNS) for (i=0; i<w0; i++) { pDst1[i*3] = trns[1]; pDst1[i*3+1] = trns[3]; pDst1[i*3+2] = trns[5]; } else keep_original = 0; break;
                                        case 3:  memset(pDst2, 0, w0*4); if (trns_idx >= 0) memset(pDst1, trns_idx, w0); else keep_original = 0; break;
                                        case 4:  memset(pDst1, 0, w0*2); break;
                                        case 6:  memset(pDst2, 0, w0*4); break;
                                    }
                                    pDst1 += outrow1;
                                    pDst2 += outrow2;
                                }
                            }
                        }
                    }

                    /*seq = */read32(file);
                    w0  = read32(file);
                    h0  = read32(file);
                    x0  = read32(file);
                    y0  = read32(file);
                    d1  = read16(file);
                    d2  = read16(file);
                    file.read((char*)&dop, 1);
                    file.read((char*)&bop, 1);
                    /*crc = */read32(file);

                    if(d2 == 0)
                        d2 = 100;
                    frames_delay[cur_frame] = (d1 * 1000)/d2;

                    if (cur_frame == 0)
                    {
                        bop = PNG_BLEND_OP_SOURCE;
                        if (dop == PNG_DISPOSE_OP_PREVIOUS)
                            dop = PNG_DISPOSE_OP_BACKGROUND;
                    }

                    if (!(coltype & 4) && !(hasTRNS))
                        bop = PNG_BLEND_OP_SOURCE;

                    rowbytes = ROWBYTES(pixeldepth, w0);
                    cur_frame++;
                    pImg1 += outimg1;
                    pImg2 += outimg2;
                }
                else if (chunk == 0x49444154) /* IDAT */
                {
                    file.read((char*)(pData + zsize), len);
                    zsize += len;
                    /*crc = */read32(file);
                }
                else if (chunk == 0x66644154) /* fdAT */
                {
                    /*seq = */read32(file);
                    len -= 4;
                    file.read((char*)(pData + zsize), len);
                    zsize += len;
                    /*crc = */read32(file);
                }
                else if (chunk == 0x49454E44) /* IEND */
                {
                    pDst1 = pImg1 + y0*outrow1 + x0*channels;
                    pDst2 = pImg2 + y0*outrow2 + x0*4;
                    unpack(zstream, pTemp, imagesize, pData, zsize, h0, rowbytes, bpp);
                    switch (coltype)
                    {
                        case 0: compose0(pDst1, outrow1, pDst2, outrow2, pTemp, rowbytes+1, w0, h0, bop, depth); break;
                        case 2: compose2(pDst1, outrow1, pDst2, outrow2, pTemp, rowbytes+1, w0, h0, bop, depth); break;
                        case 3: compose3(pDst1, outrow1, pDst2, outrow2, pTemp, rowbytes+1, w0, h0, bop, depth); break;
                        case 4: compose4(pDst1, outrow1,                 pTemp, rowbytes+1, w0, h0, bop, depth); break;
                        case 6: compose6(                pDst2, outrow2, pTemp, rowbytes+1, w0, h0, bop, depth); break;
                    }
                    break;
                }
                else
                {
                    c = (unsigned char)(chunk>>24);
                    if (notabc(c)) break;
                    c = (unsigned char)((chunk>>16) & 0xFF);
                    if (notabc(c)) break;
                    c = (unsigned char)((chunk>>8) & 0xFF);
                    if (notabc(c)) break;
                    c = (unsigned char)(chunk & 0xFF);
                    if (notabc(c)) break;

                    file.seekg(len, std::ios_base::cur);
                    /*crc = */read32(file);
                }
            }
            /* apng decoding - end */

            if (coltype == 0)
            {
                switch (depth)
                {
                    case 4: trns[1] *= 0x11; break;
                    case 2: trns[1] *= 0x55; break;
                    case 1: trns[1] *= 0xFF; break;
                }
            }

            inflateEnd(&zstream);

            apng->bpp = channels;
            apng->coltype = coltype;
            apng->last_frame = cur_frame;
            apng->first_frame = first_frame;
            apng->height = h;
            apng->width = w;
            apng->num_frames = frames;
            apng->num_plays = loops;
            apng->frames_delay = frames_delay;
            apng->pdata = pOut2;
            apng->bpp = 4;
            apng->coltype = 6;

            if(pData)
                free(pData);
            if(pTemp)
                free(pTemp);
            if(pOut1)
                free(pOut1);
        } else
            return -1;
    } else
        return -1;

    return 0;
}

void write_chunk(std::ostream& f, const char* name, unsigned char* data, unsigned int length)
{
    unsigned int crc = crc32(0, Z_NULL, 0);
    unsigned int len = swap32(length);

    f.write((char*)&len, 4);
    f.write(name, 4);
    crc = crc32(crc, (const Bytef*)name, 4);

    if(data != NULL && length > 0) {
        f.write((char*)data, length);
        crc = crc32(crc, data, length);
    }

    crc = swap32(crc);
    f.write((char*)&crc, 4);
}

void write_IDATs(std::ostream& f, unsigned char* data, unsigned int length, unsigned int idat_size)
{
    unsigned int z_cmf = data[0];

    if((z_cmf & 0x0f) == 8 && (z_cmf & 0xf0) <= 0x70) {
        if(length >= 2) {
            unsigned int z_cinfo = z_cmf >> 4;
            unsigned int half_z_window_size = 1 << (z_cinfo + 7);

            while(idat_size <= half_z_window_size && half_z_window_size >= 256) {
                z_cinfo--;
                half_z_window_size >>= 1;
            }

            z_cmf = (z_cmf & 0x0f) | (z_cinfo << 4);

            if(data[0] != (unsigned char)z_cmf) {
                data[0] = (unsigned char)z_cmf;
                data[1] &= 0xe0;
                data[1] += (unsigned char)(0x1f - ((z_cmf << 8) + data[1]) % 0x1f);
            }
        }
    }

    while(length > 0) {
        unsigned int ds = length;

        if(ds > PNG_ZBUF_SIZE)
            ds = PNG_ZBUF_SIZE;

        write_chunk(f, "IDAT", data, ds);

        data += ds;
        length -= ds;
    }
}

void save_png(std::stringstream& f, unsigned int width, unsigned int height, int channels, unsigned char *pixels)
{
    unsigned int bpp = 4;
    unsigned char coltype = 0;

    if(channels == 3)
        coltype = 2;
    else if (channels == 2)
        coltype = 4;
    else if (channels == 4)
        coltype = 6;

    struct IHDR {
        unsigned int    mWidth;
        unsigned int    mHeight;
        unsigned char   mDepth;
        unsigned char   mColorType;
        unsigned char   mCompression;
        unsigned char   mFilterMethod;
        unsigned char   mInterlaceMethod;
    } ihdr = { swap32(width), swap32(height), 8, coltype, 0, 0, 0 };

    z_stream        zstream1;
    z_stream        zstream2;
    unsigned int    i, j;

    unsigned int rowbytes  = width * bpp;
    unsigned int idat_size = (rowbytes + 1) * height;
    unsigned int zbuf_size = idat_size + ((idat_size + 7) >> 3) + ((idat_size + 63) >> 6) + 11;

    unsigned char* row_buf   = (unsigned char*)malloc(rowbytes + 1);
    unsigned char* sub_row   = (unsigned char*)malloc(rowbytes + 1);
    unsigned char* up_row    = (unsigned char*)malloc(rowbytes + 1);
    unsigned char* avg_row   = (unsigned char*)malloc(rowbytes + 1);
    unsigned char* paeth_row = (unsigned char*)malloc(rowbytes + 1);
    unsigned char* zbuf1     = (unsigned char*)malloc(zbuf_size);
    unsigned char* zbuf2     = (unsigned char*)malloc(zbuf_size);

    if(!row_buf || !sub_row || !up_row || !avg_row || !paeth_row || !zbuf1 || !zbuf2)
        return;

    row_buf[0]   = 0;
    sub_row[0]   = 1;
    up_row[0]    = 2;
    avg_row[0]   = 3;
    paeth_row[0] = 4;

    zstream1.data_type = Z_BINARY;
    zstream1.zalloc    = Z_NULL;
    zstream1.zfree     = Z_NULL;
    zstream1.opaque    = Z_NULL;
    deflateInit2(&zstream1, Z_BEST_COMPRESSION, 8, 15, 8, Z_DEFAULT_STRATEGY);

    zstream2.data_type = Z_BINARY;
    zstream2.zalloc    = Z_NULL;
    zstream2.zfree     = Z_NULL;
    zstream2.opaque    = Z_NULL;
    deflateInit2(&zstream2, Z_BEST_COMPRESSION, 8, 15, 8, Z_FILTERED);

    int a, b, c, pa, pb, pc, p, v;
    unsigned char* prev;
    unsigned char* row;

    f.write((char*)png_sign, 8);
    write_chunk(f, "IHDR", (unsigned char*)(&ihdr), 13);

    if(palsize > 0)
        write_chunk(f, "PLTE", (unsigned char*)(&pal), palsize * 3);

    if(trnssize > 0)
        write_chunk(f, "tRNS", trns, trnssize);

    zstream1.next_out  = zbuf1;
    zstream1.avail_out = zbuf_size;
    zstream2.next_out  = zbuf2;
    zstream2.avail_out = zbuf_size;

    prev = NULL;
    row  = pixels;

    for(j = 0; j < (unsigned int)height; j++) {
        unsigned char* out;
        unsigned int    sum = 0;
        unsigned char* best_row = row_buf;
        unsigned int    mins = ((unsigned int)(-1)) >> 1;

        out = row_buf + 1;

        for(i = 0; i < rowbytes; i++) {
            v = out[i] = row[i];
            sum += (v < 128) ? v : 256 - v;
        }

        mins = sum;

        sum = 0;
        out = sub_row + 1;

        for(i = 0; i < bpp; i++) {
            v = out[i] = row[i];
            sum += (v < 128) ? v : 256 - v;
        }

        for(i = bpp; i < rowbytes; i++) {
            v = out[i] = row[i] - row[i - bpp];
            sum += (v < 128) ? v : 256 - v;

            if(sum > mins) break;
        }

        if(sum < mins) {
            mins = sum;
            best_row = sub_row;
        }

        if(prev) {
            sum = 0;
            out = up_row + 1;

            for(i = 0; i < rowbytes; i++) {
                v = out[i] = row[i] - prev[i];
                sum += (v < 128) ? v : 256 - v;

                if(sum > mins) break;
            }

            if(sum < mins) {
                mins = sum;
                best_row = up_row;
            }

            sum = 0;
            out = avg_row + 1;

            for(i = 0; i < bpp; i++) {
                v = out[i] = row[i] - prev[i] / 2;
                sum += (v < 128) ? v : 256 - v;
            }

            for(i = bpp; i < rowbytes; i++) {
                v = out[i] = row[i] - (prev[i] + row[i - bpp]) / 2;
                sum += (v < 128) ? v : 256 - v;

                if(sum > mins) break;
            }

            if(sum < mins) {
                mins = sum;
                best_row = avg_row;
            }

            sum = 0;
            out = paeth_row + 1;

            for(i = 0; i < bpp; i++) {
                v = out[i] = row[i] - prev[i];
                sum += (v < 128) ? v : 256 - v;
            }

            for(i = bpp; i < rowbytes; i++) {
                a = row[i - bpp];
                b = prev[i];
                c = prev[i - bpp];
                p = b - c;
                pc = a - c;
                pa = abs(p);
                pb = abs(pc);
                pc = abs(p + pc);
                p = (pa <= pb && pa <= pc) ? a : (pb <= pc) ? b : c;
                v = out[i] = row[i] - p;
                sum += (v < 128) ? v : 256 - v;

                if(sum > mins) break;
            }

            if(sum < mins) {
                best_row = paeth_row;
            }
        }

        zstream1.next_in = row_buf;
        zstream1.avail_in = rowbytes + 1;
        deflate(&zstream1, Z_NO_FLUSH);

        zstream2.next_in = best_row;
        zstream2.avail_in = rowbytes + 1;
        deflate(&zstream2, Z_NO_FLUSH);

        prev = row;
        row += rowbytes;
    }

    deflate(&zstream1, Z_FINISH);
    deflate(&zstream2, Z_FINISH);

    if(zstream1.total_out <= zstream2.total_out)
        write_IDATs(f, zbuf1, zstream1.total_out, idat_size);
    else
        write_IDATs(f, zbuf2, zstream2.total_out, idat_size);

    deflateReset(&zstream1);
    zstream1.data_type = Z_BINARY;
    deflateReset(&zstream2);
    zstream2.data_type = Z_BINARY;

    write_chunk(f, "IEND", 0, 0);

    deflateEnd(&zstream1);
    deflateEnd(&zstream2);
    free(zbuf1);
    free(zbuf2);
    free(row_buf);
    free(sub_row);
    free(up_row);
    free(avg_row);
    free(paeth_row);
}

void free_apng(struct apng_data *apng)
{
    if(apng->pdata)
        free(apng->pdata);
    if(apng->frames_delay)
        free(apng->frames_delay);
}

