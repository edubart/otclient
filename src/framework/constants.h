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


#ifndef CONSTANTS_H
#define CONSTANTS_H

enum AlignmentFlag {
    AlignLeft = 1,
    AlignRight = 2,
    AlignTop = 4,
    AlignBottom = 8,
    AlignHorizontalCenter = 16,
    AlignVerticalCenter = 32,
    AlignTopLeft = AlignTop | AlignLeft,
    AlignTopRight = AlignTop | AlignRight,
    AlignBottomLeft = AlignBottom | AlignLeft,
    AlignBottomRight = AlignBottom | AlignRight,
    AlignLeftCenter = AlignLeft | AlignVerticalCenter,
    AlignRightCenter = AlignRight | AlignVerticalCenter,
    AlignTopCenter = AlignTop | AlignHorizontalCenter,
    AlignBottomCenter = AlignBottom | AlignHorizontalCenter,
    AlignCenter = AlignVerticalCenter | AlignHorizontalCenter
};

#endif // CONSTANTS_H
