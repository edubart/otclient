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

#ifndef WIN32WINDOW_H
#define WIN32WINDOW_H

#include "platformwindow.h"

#include <windows.h>

struct WindowProcProxy;

class WIN32Window : public PlatformWindow
{
    void internalCreateWindow();
    void internalRegisterWindowClass();

    void internalChooseGLVisual();
    void internalCreateGLContext();

    void *getExtensionProcAddress(const char *ext);
    bool isExtensionSupported(const char *ext);

    LRESULT windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    friend class WindowProcProxy;

public:
    WIN32Window();

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
    void displayFatalError(const std::string& message);

    void setMouseCursor(const std::string& file, const Point& hotSpot);
    void setTitle(const std::string& title);
    void setMinimumSize(const Size& minimumSize);
    void setFullscreen(bool fullscreen);
    void setVerticalSync(bool enable);
    void setIcon(const std::string& iconFile);
    void setClipboardText(const std::string& text);

    Size getDisplaySize();
    std::string getClipboardText();
    std::string getPlatformType();

    bool isMaximized() { return m_maximized; }

private:
    HWND m_window;
    HINSTANCE m_instance;
    HDC m_deviceContext;
    HGLRC m_glContext;
    HCURSOR m_cursor;
    HCURSOR m_defaultCursor;
    bool m_maximized;
    Size m_minimumSize;
};

#endif
