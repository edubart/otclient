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

#include "demangle.h"

#ifdef _MSC_VER

#include <winsock2.h>
#include <windows.h>

#pragma warning (push)
#pragma warning (disable:4091) // warning C4091: 'typedef ': ignored on left of '' when no variable is declared
#include <dbghelp.h>
#pragma warning (pop)

#else

#include <cxxabi.h>
#include <cstring>
#include <cstdlib>

#endif

namespace stdext {

const char* demangle_name(const char* name)
{
    static const unsigned BufferSize = 1024;
    static char Buffer[BufferSize] = {};

#ifdef _MSC_VER
    int written = UnDecorateSymbolName(name, Buffer, BufferSize - 1, UNDNAME_COMPLETE);
    Buffer[written] = '\0';
#else
    size_t len;
    int status;
    char* demangled = abi::__cxa_demangle(name, nullptr, &len, &status);
    if(demangled) {
        strncpy(Buffer, demangled, BufferSize - 1);
        Buffer[BufferSize - 1] = '\0';
        free(demangled);
    } else {
        Buffer[0] = '\0';
    }
#endif

    return Buffer;
}

}
