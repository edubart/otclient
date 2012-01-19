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

#include "utf8.h"

#include <string>
#include <cstring>

char Fw::utf8CharToLatin1(uchar *utf8, int *read)
{
    char c = '?';
    uchar opt1 = utf8[0];
    if(opt1 == 0xc3) {
        *read = 2;
        uchar opt2 = utf8[1];
        c = 64 + opt2;
    } else if(opt1 == 0xc2) {
        *read = 2;
        uchar opt2 = utf8[1];
        if(opt2 > 0xa1 && opt2 < 0xbb)
            c = opt2;
    } else if(opt1 < 0xc2) {
        *read = 1;
        c = opt1;
    }
    return c;
}

std::string Fw::utf8StringToLatin1(uchar *utf8) {
    std::string out;
    int len = strlen((char*)utf8);
    for(int i=0; i<len;) {
        int read;
        uchar *utf8char = &utf8[i];
        out += Fw::utf8CharToLatin1(utf8char, &read);
        i += read;
    }
    return out;
}
