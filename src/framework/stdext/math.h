/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

#ifndef STDEXT_MATH_H
#define STDEXT_MATH_H

#include "types.h"

namespace stdext {

inline bool is_power_of_two(size_t v) { return ((v != 0) && !(v & (v - 1))); }
inline size_t to_power_of_two(size_t v) { if(v == 0) return 0; size_t r = 1; while(r < v && r != 0xffffffff) r <<= 1; return r; }

inline uint16_t readLE16(const uchar *addr) { return (uint16_t)addr[1] << 8 | addr[0]; }
inline uint32_t readLE32(const uchar *addr) { return (uint32_t)readLE16(addr + 2) << 16 | readLE16(addr); }
inline uint64_t readLE64(const uchar *addr) { return (uint64_t)readLE32(addr + 4) << 32 | readLE32(addr); }

inline void writeLE16(uchar *addr, uint16_t value) { addr[1] = value >> 8; addr[0] = (uint8_t)value; }
inline void writeLE32(uchar *addr, uint32_t value) { writeLE16(addr + 2, value >> 16); writeLE16(addr, (uint16_t)value); }
inline void writeLE64(uchar *addr, uint64_t value) { writeLE32(addr + 4, value >> 32); writeLE32(addr, (uint32_t)value); }

uint32_t adler32(const uint8_t *buffer, size_t size);

long random_range(long min, long max);
float random_range(float min, float max);

double round(double r);

}

#endif
