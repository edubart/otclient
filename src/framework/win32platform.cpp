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

#include "platform.h"
#include "engine.h"

#include <dir.h>
#include <physfs.h>
#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct Win32PlatformPrivate {
    HWND window;
    HINSTANCE instance;
    HDC hdc;
    HGLRC hrc;

    std::string appName;
    int x, y;
    int width, height;
    int minWidth, minHeight;
    bool maximized;
} win32;

void Platform::init(const char *appName)
{
    win32.appName = appName;
    win32.instance = GetModuleHandle(NULL);

    WNDCLASSA wc;
    wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
    wc.lpfnWndProc		= (WNDPROC)WndProc;			// WndProc Handles Messages
    wc.cbClsExtra		= 0;					// No Extra Window Data
    wc.cbWndExtra		= 0;					// No Extra Window Data
    wc.hInstance		= win32.instance;			// Set The Instance
    wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);		// Load The Default Icon
    wc.hCursor			= LoadCursor(NULL, IDC_ARROW);		// Load The Arrow Pointer
    wc.hbrBackground            = NULL;					// No Background Required For GL
    wc.lpszMenuName		= NULL;					// We Don't Want A Menu
    wc.lpszClassName            = win32.appName.c_str();                        // Set The Class Name

    if(!RegisterClassA(&wc))
        fatal("Failed to register the window class.");

    // force first tick
    Platform::getTicks();
}

void Platform::terminate()
{
    if(win32.window) {
        destroyWindow();
        win32.window = NULL;
    }

    if(win32.instance) {
        if(!UnregisterClassA(win32.appName.c_str(), win32.instance))
            error("Unregister class failed.");

        win32.instance = NULL;
    }
}

void Platform::poll()
{
    MSG msg;
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int Platform::getTicks()
{
    static unsigned long firstTick = 0;
    if(!firstTick)
        firstTick = GetTickCount();
    
    return (uint32_t)(GetTickCount() - firstTick);
}

void Platform::sleep(unsigned long miliseconds)
{
    Sleep(miliseconds);
}

bool Platform::createWindow(int x, int y, int width, int height, int minWidth, int minHeight, bool maximized)
{
    DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    win32.x = x;
    win32.y = y;
    win32.width = width;
    win32.height = height;
    win32.minWidth = minWidth;
    win32.minHeight = minHeight;
    win32.maximized = maximized;

    RECT windowRect = {x, y, x + width, y + height};
    AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

    win32.window = CreateWindowExA(dwExStyle,                           // Extended Style For The Window
                                   win32.appName.c_str(),               // Class Name
                                   win32.appName.c_str(),               // Window Title
                                   dwStyle,                             // Required Window Style
                                   windowRect.left,                     // Window X Position
                                   windowRect.top,                      // Window Y Position
                                   windowRect.right - windowRect.left,  // Calculate Window Width
                                   windowRect.bottom - windowRect.top,  // Calculate Window Height
                                   NULL,		                // No Parent Window
                                   NULL,		                // No Menu
                                   win32.instance,                      // Instance
                                   NULL);

    if(!win32.window) {
        terminate();
        fatal("Window creation error.");
        return false;
    }

    GLuint pixelFormat;
    static PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
        1,							// Version Number
        PFD_DRAW_TO_WINDOW |					// Format Must Support Window
        PFD_SUPPORT_OPENGL |					// Format Must Support OpenGL
        PFD_DOUBLEBUFFER,					// Must Support Double Buffering
        PFD_TYPE_RGBA,						// Request An RGBA Format
        32,							// Select Our Color Depth
        0, 0, 0, 0, 0, 0,					// Color Bits Ignored
        0,							// No Alpha Buffer
        0,							// Shift Bit Ignored
        0,							// No Accumulation Buffer
        0, 0, 0, 0,						// Accumulation Bits Ignored
        16,							// 16Bit Z-Buffer (Depth Buffer)
        0,							// No Stencil Buffer
        0,							// No Auxiliary Buffer
        PFD_MAIN_PLANE,						// Main Drawing Layer
        0,							// Reserved
        0, 0, 0							// Layer Masks Ignored
    };

    if(!(win32.hdc = GetDC(win32.window))) {
        terminate();
        fatal("Can't Create A GL Device Context.");
        return false;
    }

    if(!(pixelFormat = ChoosePixelFormat(win32.hdc, &pfd))) {
        terminate();
        fatal("Can't Find A Suitable PixelFormat.");
        return false;
    }

    if(!SetPixelFormat(win32.hdc, pixelFormat, &pfd)) {
        terminate();
        fatal("Can't Set The PixelFormat.");
        return false;
    }

    if(!(win32.hrc = wglCreateContext(win32.hdc))) {
        terminate();
        fatal("Can't Create A GL Rendering Context.");
        return false;
    }

    if(!wglMakeCurrent(win32.hdc, win32.hrc)) {
        terminate();
        fatal("Can't Activate The GL Rendering Context.");
        return false;
    }

    return true;
}

void Platform::destroyWindow()
{
    if(win32.hrc) {
        if(!wglMakeCurrent(NULL, NULL))
            error("Release Of DC And RC Failed.");

        if(!wglDeleteContext(win32.hrc))
            error("Release Rendering Context Failed.");

        win32.hrc = NULL;
    }

    if(win32.hdc) {
        if(!ReleaseDC(win32.window, win32.hdc))
            error("Release Device Context Failed.");

        win32.hdc = NULL;
    }

    if(win32.window) {
        if(!DestroyWindow(win32.window))
            error("Destroy window failed.");

        win32.window = NULL;
    }
}

void Platform::showWindow()
{
    if(win32.maximized)
        ShowWindow(win32.window, SW_MAXIMIZE);
    else
        ShowWindow(win32.window, SW_SHOW);
}

void Platform::setWindowTitle(const char *title)
{
    SetWindowTextA(win32.window, title);
}

void *Platform::getExtensionProcAddress(const char *ext)
{
    return (void*)wglGetProcAddress(ext);
}

bool Platform::isExtensionSupported(const char *ext)
{
    typedef const char* _wglGetExtensionsStringARB(HDC hdc);
    _wglGetExtensionsStringARB *wglGetExtensionsStringARB = (_wglGetExtensionsStringARB*)getExtensionProcAddress("wglGetExtensionsStringARB");

    const char *exts = wglGetExtensionsStringARB(win32.hdc);
    if(strstr(exts, ext))
        return true;
    return false;
}

const char *Platform::getClipboardText()
{
    const char *text = "";
    if(OpenClipboard(NULL)) {
        text = (const char*)GetClipboardData(CF_TEXT);
        CloseClipboard();
    }
    return text;
}

void Platform::setClipboardText(const char *text)
{
    int textLenght = strlen(text);
    HANDLE hData = new HANDLE[textLenght + 1];
    memcpy(hData, text, textLenght + 1);

    if(OpenClipboard(NULL)) {
        EmptyClipboard();
        SetClipboardData(CF_TEXT, hData);
        CloseClipboard();
    }
}

void Platform::hideMouseCursor()
{
    ShowCursor(false);
}

void Platform::showMouseCursor()
{
    ShowCursor(true);
}

void Platform::setVsync(bool enable)
{
    typedef GLint (*glSwapIntervalProc)(GLint);
    glSwapIntervalProc glSwapInterval = NULL;

    if(isExtensionSupported("WGL_EXT_swap_control"))
        glSwapInterval = (glSwapIntervalProc)getExtensionProcAddress("wglSwapIntervalEXT");

    if(glSwapInterval)
        glSwapInterval(enable ? 1 : 0);
}

void Platform::swapBuffers()
{
    SwapBuffers(win32.hdc);
}

int Platform::getWindowX()
{
    return win32.x;
}

int Platform::getWindowY()
{
    return win32.y;
}

int Platform::getWindowWidth()
{
    return win32.width;
}

int Platform::getWindowHeight()
{
    return win32.height;
}

int Platform::getDisplayWidth()
{
    return GetSystemMetrics(SM_CXSCREEN);
}

int Platform::getDisplayHeight()
{
    return GetSystemMetrics(SM_CYSCREEN);
}

bool Platform::isWindowMaximized()
{
    return win32.maximized;
}

const char *Platform::getAppUserDir()
{
    std::stringstream sdir;
    sdir << PHYSFS_getUserDir() << "/." << win32.appName << "/";
    if((mkdir(sdir.str().c_str()) != 0) && (errno != EEXIST))
        error("Couldn't create directory for saving configuration file. (%s)", sdir.str().c_str());
    return sdir.str().c_str();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_CLOSE:
        {
            g_engine.onClose();
            break;
        }
    case WM_GETMINMAXINFO:
        {
            MINMAXINFO *minMax = (MINMAXINFO*)lParam;
            minMax->ptMinTrackSize.x = win32.minWidth;
            minMax->ptMinTrackSize.y = win32.minHeight;
            break;
        }
    case WM_MOVE:
        {
            win32.x = LOWORD(lParam);
            win32.y = HIWORD(lParam);
            break;
        }
    case WM_SIZE:
        {
            switch(wParam)
            {
            case SIZE_MAXIMIZED:
                win32.maximized = true;
                break;
            case SIZE_RESTORED:
                win32.maximized = false;
                break;
            }

            win32.width = LOWORD(lParam);
            win32.height = HIWORD(lParam);
            g_engine.onResize(LOWORD(lParam), HIWORD(lParam));
            break;
        }
    default:
        {
            return DefWindowProc(hWnd,uMsg,wParam,lParam);
        }
    }
    return 0;
}
