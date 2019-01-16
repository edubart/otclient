/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#ifdef WIN32
#include <winsock2.h>
#endif

#ifdef __clang__
    // clang is supported
    #define BUILD_COMPILER "clang " __VERSION__
#elif defined(__GNUC__)
    #if !(__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
    #error "Sorry, you need gcc 4.6 or greater to compile."
    #endif
    #define BUILD_COMPILER "gcc " __VERSION__
#elif defined(_MSC_VER)
    #if _MSC_VER < 1800
    #error "You need Visual Studio 2013 or greater to compile."
    #endif
    #pragma warning(disable:4244) // conversion from 'A' to 'B', possible loss of data
    #pragma warning(disable:4267) // '?' : conversion from 'A' to 'B', possible loss of data
    #pragma warning(disable:4305) // 'initializing' : truncation from 'A' to 'B'
    #pragma warning(disable:4146) // unary minus operator applied to unsigned type, result still unsigned
    #pragma warning(disable:4800) // 'A' : forcing value to bool 'true' or 'false' (performance warning)

    #if _MSC_VER == 1912 || _MSC_VER == 1911 || _MSC_VER == 1910
    #define BUILD_COMPILER "Visual Studio 2017"
    #elif _MSC_VER == 1900
    #define BUILD_COMPILER "Visual Studio 2015"
    #elif _MSC_VER == 1800
    #define BUILD_COMPILER "Visual Studio 2013"
    #else
    #define BUILD_COMPILER "Visual Studio"
    #endif

    #define __PRETTY_FUNCTION__ __FUNCDNAME__
#else
    #error "Compiler not supported."
#endif

/// Branch Prediction.  See the GCC Manual for more information.
/// NB: These are used when speed is need most; do not use in normal
/// code, they may slow down stuff.
#if defined(__clang__) || defined(__GNUC__)
#define likely(x)     __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)
#else
#define likely(x)    (x)
#define unlikely(x)    (x)
#endif

#if !defined(_MSC_VER) && !defined(__GXX_EXPERIMENTAL_CXX0X__)
#error "C++0x is required to compile this application.  Try updating your compiler."
#endif

#endif
