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

#include "demangle.h"

#ifdef _MSC_VER
#include <winsock2.h>
#include <windows.h>
#include <dbghelp.h>
#else
#include <cxxabi.h>
#include <cstring>
#include <cstdlib>
#endif

namespace stdext {

const char* demangle_name(const char* name)
{
#ifdef _MSC_VER
    static char buffer[1024];
    UnDecorateSymbolName(name, buffer, sizeof(buffer), UNDNAME_COMPLETE);
    return buffer;
#else
    size_t len;
    int status;
    static char buffer[1024];
    char* demangled = abi::__cxa_demangle(name, 0, &len, &status);
    if(demangled) {
        strcpy(buffer, demangled);
        free(demangled);
    }
    return buffer;
#endif
}

}
