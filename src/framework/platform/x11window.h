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

#ifndef X11WINDOW_H
#define X11WINDOW_H

#include "platformwindow.h"
#include <framework/graphics/glutil.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#ifndef OPENGL_ES2
#include <GL/glx.h>
#else
#include <EGL/egl.h>
#endif

class X11Window : public PlatformWindow
{
    void internalOpenDisplay();
    void internalCreateWindow();
    bool internalSetupWindowInput();

    void internalCheckGL();
    void internalChooseGLVisual();
    void internalCreateGLContext();
    void internalDestroyGLContext();
    void internalConnectGLContext();

    void *getExtensionProcAddress(const char *ext);
    bool isExtensionSupported(const char *ext);

public:
    X11Window();

    void init();
    void terminate();

    void move(const Point& pos);
    void resize(const Size& size);
    void show();
    void hide();
    void maximize();
    void poll();
    void swapBuffers();
    void restoreMouseCursor();
    void showMouse();
    void hideMouse();

    void setMouseCursor(const std::string& file);
    void setTitle(const std::string& title);
    void setMinimumSize(const Size& minimumSize);
    void setFullscreen(bool fullscreen);
    void setVerticalSync(bool enable);
    void setIcon(const std::string& iconFile);
    void setClipboardText(const std::string& text);

    Size getDisplaySize();
    std::string getClipboardText();
    std::string getPlatformType();

    bool isMaximized();

private:
    Display *m_display;
    XVisualInfo *m_visual;
    Window m_window;
    Window m_rootWindow;
    Colormap m_colormap;
    Cursor m_cursor;
    XIM m_xim;
    XIC m_xic;
    int m_screen;
    Atom m_wmDelete;
    std::string m_clipboardText;

#ifndef OPENGL_ES2
    GLXContext m_glxContext;
#else
    EGLConfig m_eglConfig;
    EGLContext m_eglContext;
    EGLDisplay m_eglDisplay;
    EGLSurface m_eglSurface;
#endif
};

#endif

