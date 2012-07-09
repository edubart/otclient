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

#ifndef STDEXT_MATH_H
#define STDEXT_MATH_H

#include "types.h"
#include <random>

namespace stdext {

inline uint32 adler32(const uint8 *buffer, uint16 size) {
    register uint32 a = 1, b = 0, tlen;
    while(size > 0) {
        tlen = size > 5552 ? 5552 : size;
        size -= tlen;
        do {
            a += *buffer++;
            b += a;
        } while (--tlen);

        a %= 65521;
        b %= 65521;
    }
    return (b << 16) | a;
}

inline bool is_power_of_two(uint32 v) {
    return ((v != 0) && !(v & (v - 1)));
}

inline uint32 to_power_of_two(uint32 v) {
    if(v == 0)
        return 0;
    uint32 r = 1;
    while(r < v && r != 0xffffffff)
        r <<= 1;
    return r;
}

inline uint16 readLE16(const uchar *addr) { return (uint16)addr[1] << 8 | addr[0]; }
inline uint32 readLE32(const uchar *addr) { return (uint32)readLE16(addr + 2) << 16 | readLE16(addr); }
inline uint64 readLE64(const uchar *addr) { return (uint64)readLE32(addr + 4) << 32 | readLE32(addr); }

inline void writeLE16(uchar *addr, uint16 value) { addr[1] = value >> 8; addr[0] = (uint8)value; }
inline void writeLE32(uchar *addr, uint32 value) { writeLE16(addr + 2, value >> 16); writeLE16(addr, (uint16)value); }
inline void writeLE64(uchar *addr, uint64 value) { writeLE32(addr + 4, value >> 32); writeLE32(addr, (uint32)value); }

template<typename T>
T random_range(T min, T max);

template<>
inline int random_range<int>(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dis(0, 2147483647);
    return min + (dis(gen) % (max - min + 1));
}

template<>
inline float random_range<float>(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0, 1.0);
    return min + (max - min)*dis(gen);
}

}

#endif
