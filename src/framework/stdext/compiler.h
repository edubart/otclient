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

#ifndef STDEXT_COMPILER_H
#define STDEXT_COMPILER_H

#ifdef __clang__
    // clang is supported
#elif defined(__GNUC__)
    #if !(__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
    #error "Sorry, you need gcc 4.6 or greater to compile."
    #endif
#else
    #error "Compiler not supported."
#endif

/// Branch Prediction.  See the GCC Manual for more information.
/// NB: These are used when speed is need most; do not use in normal
/// code, they may slow down stuff.
#if defined(__clang__) || defined(__GNUC__)
#define likely(x) 	__builtin_expect(!!(x), 1)
#define unlikely(x) 	__builtin_expect(!!(x), 0)
#else
#define likely(x) 	(x)
#define unlikely(x) 	(x)
#endif

#if !defined(__GXX_EXPERIMENTAL_CXX0X__)
#error "C++0x is required to compile this application.  Try updating your compiler."
#endif

#ifdef _MSC_VER
#warning "MSVC lacks some C++11 features used in this application; compilation is most likely to fail."
#endif

#endif
