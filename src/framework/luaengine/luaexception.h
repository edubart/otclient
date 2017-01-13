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

#ifndef LUAEXCEPTION_H
#define LUAEXCEPTION_H

#include "declarations.h"

class LuaException : public stdext::exception
{
public:
    LuaException(const std::string& error, int traceLevel = -1);
    virtual ~LuaException() throw() { }

    void generateLuaErrorMessage(const std::string& error, int traceLevel);

    virtual const char* what() const throw() { return m_what.c_str(); }

protected:
    LuaException() { }

    std::string m_what;
};

class LuaBadNumberOfArgumentsException : public LuaException
{
public:
    LuaBadNumberOfArgumentsException(int expected = -1, int got = -1);
};

class LuaBadValueCastException : public LuaException
{
public:
    LuaBadValueCastException(const std::string& luaTypeName, const std::string& cppTypeName);
};

#endif
