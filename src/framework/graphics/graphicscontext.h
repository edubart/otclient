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

#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

#include "declarations.h"

#ifdef WIN32
#include <windows.h>
typedef HWND WindowType;
typedef HDC DisplayType;
#else
#include <X11/Xlib.h>
typedef Window WindowType;
typedef Display *DisplayType;
#endif

class GraphicsContext
{
public:
    GraphicsContext(const std::string& name);
    virtual ~GraphicsContext() {}

    std::string getName() { return m_name; }

    virtual void create(WindowType window, DisplayType display) = 0;
    virtual void destroy() = 0;
    virtual void restore() = 0;

    virtual void swapBuffers() = 0;

    virtual void setVerticalSync(bool enable) = 0;

protected:
    std::string m_name;
    WindowType m_window;
    DisplayType m_display;

};

#endif
