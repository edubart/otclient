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

#include "win32window.h"

#include <framework/application.h>
#include <framework/thirdparty/apngloader.h>
#include <framework/core/resourcemanager.h>
#include <framework/util/utf8.h>

WIN32Window::WIN32Window()
{
    m_window = 0;
    m_instance = 0;
    m_deviceContext = 0;
    m_glContext = 0;
    m_cursor = 0;
    m_maximized = false;
    m_minimumSize = Size(16,16);
    m_size = m_minimumSize;

    m_keyMap[VK_ESCAPE] = Fw::KeyEscape;
    m_keyMap[VK_TAB] = Fw::KeyTab;
    m_keyMap[VK_RETURN] = Fw::KeyReturn;
    m_keyMap[VK_BACK] = Fw::KeyBackspace;
    m_keyMap[VK_SPACE] = Fw::KeySpace;

    m_keyMap[VK_PRIOR] = Fw::KeyPageUp;
    m_keyMap[VK_NEXT] = Fw::KeyPageDown;
    m_keyMap[VK_HOME] = Fw::KeyHome;
    m_keyMap[VK_END] = Fw::KeyEnd;
    m_keyMap[VK_INSERT] = Fw::KeyInsert;
    m_keyMap[VK_DELETE] = Fw::KeyDelete;

    m_keyMap[VK_UP] = Fw::KeyUp;
    m_keyMap[VK_DOWN] = Fw::KeyDown;
    m_keyMap[VK_LEFT] = Fw::KeyLeft;
    m_keyMap[VK_RIGHT] = Fw::KeyRight;

    m_keyMap[VK_NUMLOCK] = Fw::KeyNumLock;
    m_keyMap[VK_SCROLL] = Fw::KeyScrollLock;
    m_keyMap[VK_CAPITAL] = Fw::KeyCapsLock;
    m_keyMap[VK_SNAPSHOT] = Fw::KeyPrintScreen;
    m_keyMap[VK_PAUSE] = Fw::KeyPause;

    m_keyMap[VK_CONTROL] = Fw::KeyCtrl;
    m_keyMap[VK_LCONTROL] = Fw::KeyCtrl;
    m_keyMap[VK_RCONTROL] = Fw::KeyCtrl;
    m_keyMap[VK_SHIFT] = Fw::KeyShift;
    m_keyMap[VK_LSHIFT] = Fw::KeyShift;
    m_keyMap[VK_RSHIFT] = Fw::KeyShift;
    m_keyMap[VK_MENU] = Fw::KeyAlt;
    m_keyMap[VK_LMENU] = Fw::KeyAlt;
    m_keyMap[VK_RMENU] = Fw::KeyAltGr;
    m_keyMap[VK_LWIN] = Fw::KeyMeta;
    m_keyMap[VK_RWIN] = Fw::KeyMeta;
    m_keyMap[VK_MENU] = Fw::KeyMenu;

    // ascii characters
    /*
    m_keyMap['!'] = Fw::KeyExclamation;
    m_keyMap['"'] = Fw::KeyQuote;
    m_keyMap['#'] = Fw::KeyNumberSign;
    m_keyMap['$'] = Fw::KeyDollar;
    m_keyMap['%'] = Fw::KeyPercent;
    m_keyMap['$'] = Fw::KeyAmpersand;
    m_keyMap['\''] = Fw::KeyApostrophe;
    m_keyMap['('] = Fw::KeyLeftParen;
    m_keyMap[')'] = Fw::KeyRightParen;
    m_keyMap['*'] = Fw::KeyAsterisk;
    m_keyMap['+'] = Fw::KeyPlus;
    m_keyMap['.'] = Fw::KeyComma;
    m_keyMap['-'] = Fw::KeyMinus;
    m_keyMap['.'] = Fw::KeyPeriod;
    m_keyMap['/'] = Fw::KeySlash;
    */

    m_keyMap['0'] = Fw::Key0;
    m_keyMap['1'] = Fw::Key1;
    m_keyMap['2'] = Fw::Key2;
    m_keyMap['3'] = Fw::Key3;
    m_keyMap['4'] = Fw::Key4;
    m_keyMap['5'] = Fw::Key5;
    m_keyMap['6'] = Fw::Key6;
    m_keyMap['7'] = Fw::Key7;
    m_keyMap['8'] = Fw::Key8;
    m_keyMap['9'] = Fw::Key9;

    /*
    m_keyMap[':'] = Fw::KeyColon;
    m_keyMap[';'] = Fw::KeySemicolon;
    m_keyMap['<'] = Fw::KeyLess;
    m_keyMap['='] = Fw::KeyEqual;
    m_keyMap['>'] = Fw::KeyGreater;
    m_keyMap['?'] = Fw::KeyQuestion;
    m_keyMap['@'] = Fw::KeyAtSign;
    */

    m_keyMap['A'] = Fw::KeyA;
    m_keyMap['B'] = Fw::KeyB;
    m_keyMap['C'] = Fw::KeyC;
    m_keyMap['D'] = Fw::KeyD;
    m_keyMap['E'] = Fw::KeyE;
    m_keyMap['F'] = Fw::KeyF;
    m_keyMap['G'] = Fw::KeyG;
    m_keyMap['H'] = Fw::KeyH;
    m_keyMap['I'] = Fw::KeyI;
    m_keyMap['J'] = Fw::KeyJ;
    m_keyMap['K'] = Fw::KeyK;
    m_keyMap['L'] = Fw::KeyL;
    m_keyMap['M'] = Fw::KeyM;
    m_keyMap['N'] = Fw::KeyN;
    m_keyMap['O'] = Fw::KeyO;
    m_keyMap['P'] = Fw::KeyP;
    m_keyMap['Q'] = Fw::KeyQ;
    m_keyMap['R'] = Fw::KeyR;
    m_keyMap['S'] = Fw::KeyS;
    m_keyMap['T'] = Fw::KeyT;
    m_keyMap['U'] = Fw::KeyU;
    m_keyMap['V'] = Fw::KeyV;
    m_keyMap['W'] = Fw::KeyW;
    m_keyMap['X'] = Fw::KeyX;
    m_keyMap['Y'] = Fw::KeyY;
    m_keyMap['Z'] = Fw::KeyZ;

    /*
    m_keyMap['['] = Fw::KeyLeftBracket;
    m_keyMap['\\'] = Fw::KeyBackslash;
    m_keyMap[']'] = Fw::KeyRightBracket;
    m_keyMap['^'] = Fw::KeyCaret;
    m_keyMap['_'] = Fw::KeyUnderscore;
    m_keyMap['`'] = Fw::KeyGrave;
    m_keyMap['{'] = Fw::KeyLeftCurly;
    m_keyMap['|'] = Fw::KeyBar;
    m_keyMap['}'] = Fw::KeyRightCurly;
    m_keyMap['~'] = Fw::KeyTilde;
    */

    // keypad
    m_keyMap[VK_ADD] = Fw::KeyPlus;
    m_keyMap[VK_SUBTRACT] = Fw::KeyMinus;
    m_keyMap[VK_DECIMAL] = Fw::KeyPeriod;
    m_keyMap[VK_DIVIDE] = Fw::KeySlash;
    m_keyMap[VK_MULTIPLY] = Fw::KeyAsterisk;
    m_keyMap[VK_SEPARATOR] = Fw::KeyEnter;

    // keypad with numlock off
    m_keyMap[VK_NUMPAD0] = Fw::KeyNumpad0;
    m_keyMap[VK_NUMPAD1] = Fw::KeyNumpad1;
    m_keyMap[VK_NUMPAD2] = Fw::KeyNumpad2;
    m_keyMap[VK_NUMPAD3] = Fw::KeyNumpad3;
    m_keyMap[VK_NUMPAD4] = Fw::KeyNumpad4;
    m_keyMap[VK_NUMPAD5] = Fw::KeyNumpad5;
    m_keyMap[VK_NUMPAD6] = Fw::KeyNumpad6;
    m_keyMap[VK_NUMPAD7] = Fw::KeyNumpad7;
    m_keyMap[VK_NUMPAD8] = Fw::KeyNumpad8;
    m_keyMap[VK_NUMPAD9] = Fw::KeyNumpad9;

    m_keyMap[VK_OEM_1] = Fw::KeySemicolon;
    m_keyMap[VK_OEM_2] = Fw::KeySlash;
    m_keyMap[VK_OEM_3] = Fw::KeyGrave;
    m_keyMap[VK_OEM_4] = Fw::KeyLeftBracket;
    m_keyMap[VK_OEM_5] = Fw::KeyBackslash;
    m_keyMap[VK_OEM_6] = Fw::KeyRightBracket;
    m_keyMap[VK_OEM_7] = Fw::KeyApostrophe;
    m_keyMap[VK_OEM_MINUS] = Fw::KeyMinus;
    m_keyMap[VK_OEM_PLUS] = Fw::KeyPlus;
    m_keyMap[VK_OEM_COMMA] = Fw::KeyComma;
    m_keyMap[VK_OEM_PERIOD] = Fw::KeyPeriod;

    m_keyMap[VK_F1] = Fw::KeyF1;
    m_keyMap[VK_F2] = Fw::KeyF2;
    m_keyMap[VK_F3] = Fw::KeyF3;
    m_keyMap[VK_F4] = Fw::KeyF4;
    m_keyMap[VK_F5] = Fw::KeyF5;
    m_keyMap[VK_F6] = Fw::KeyF6;
    m_keyMap[VK_F7] = Fw::KeyF7;
    m_keyMap[VK_F8] = Fw::KeyF8;
    m_keyMap[VK_F9] = Fw::KeyF9;
    m_keyMap[VK_F10] = Fw::KeyF10;
    m_keyMap[VK_F11] = Fw::KeyF11;
    m_keyMap[VK_F12] = Fw::KeyF12;
}

void WIN32Window::init()
{
    m_instance = GetModuleHandle(NULL);

    internalRegisterWindowClass();
    internalCreateWindow();
    internalChooseGLVisual();
    internalCreateGLContext();
}

void WIN32Window::terminate()
{
    if(m_glContext) {
        if(!wglMakeCurrent(NULL, NULL))
            logError("Release of dc and rc failed.");
        if(!wglDeleteContext(m_glContext))
            logError("Release rendering context failed.");
        m_glContext = NULL;
    }

    if(m_deviceContext) {
        if(!ReleaseDC(m_window, m_deviceContext))
            logError("Release device context failed.");
        m_deviceContext = NULL;
    }

    if(m_cursor) {
        DestroyCursor(m_cursor);
        m_cursor = NULL;
    }

    if(m_window) {
        if(!DestroyWindow(m_window))
            logError("ERROR: Destroy window failed.");
        m_window = NULL;
    }

    if(m_instance) {
        if(!UnregisterClassA(g_app->getAppName().c_str(), m_instance))
            logError("UnregisterClassA failed");
        m_instance = NULL;
    }
}

struct WindowProcProxy {
    static LRESULT CALLBACK call(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        WIN32Window *window = (WIN32Window*)&g_window;
        return window->windowProc(hWnd, uMsg, wParam, lParam);
    }
};

void WIN32Window::internalRegisterWindowClass()
{
    m_defaultCursor = LoadCursor(NULL, IDC_ARROW);
    WNDCLASSA wc;
    wc.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc      = (WNDPROC)WindowProcProxy::call;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = m_instance;
    wc.hIcon            = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor          = m_defaultCursor;
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = g_app->getAppName().c_str();

    if(!RegisterClassA(&wc))
        logFatal("Failed to register the window class.");
}

void WIN32Window::internalCreateWindow()
{
    DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD dwStyle = WS_OVERLAPPEDWINDOW;

    RECT windowRect = {m_pos.x, m_pos.y, m_pos.x + m_size.width(), m_pos.y + m_size.height()};
    AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

    updateUnmaximizedCoords();
    m_window = CreateWindowExA(dwExStyle,
                               g_app->getAppName().c_str(),
                               NULL,
                               dwStyle,
                               windowRect.left,
                               windowRect.top,
                               windowRect.right - windowRect.left,
                               windowRect.bottom - windowRect.top,
                               NULL,
                               NULL,
                               m_instance,
                               NULL);

    if(!m_window)
        logFatal("Unable to create window");

    m_deviceContext = GetDC(m_window);
    if(!m_deviceContext)
        logFatal("GetDC failed");
}

void WIN32Window::internalChooseGLVisual()
{
    uint pixelFormat;
    static PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR),
                                         1,
                                         PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                                         PFD_TYPE_RGBA,
                                         32,                         // Select Our Color Depth
                                         0, 0, 0, 0, 0, 0,           // Color Bits Ignored
                                         0,                          // No Alpha Buffer
                                         0,                          // Shift Bit Ignored
                                         0,                          // No Accumulation Buffer
                                         0, 0, 0, 0,                 // Accumulation Bits Ignored
                                         16,                         // 16Bit Z-Buffer (Depth Buffer)
                                         0,                          // No Stencil Buffer
                                         0,                          // No Auxiliary Buffer
                                         PFD_MAIN_PLANE,             // Main Drawing Layer
                                         0,                          // Reserved
                                         0, 0, 0 };                  // Layer Masks Ignored

    pixelFormat = ChoosePixelFormat(m_deviceContext, &pfd);
    if(!pixelFormat)
        logFatal("Could not find a suitable pixel format");

    if(!SetPixelFormat(m_deviceContext, pixelFormat, &pfd))
        logFatal("Could not set the pixel format");
}

void WIN32Window::internalCreateGLContext()
{
    if(!(m_glContext = wglCreateContext(m_deviceContext)))
        logFatal("Unable to create GL context");

    if(!wglMakeCurrent(m_deviceContext, m_glContext))
        logFatal("Unable to set GLX context on WIN32 window");
}

bool WIN32Window::isExtensionSupported(const char *ext)
{
    typedef const char* (WINAPI * wglGetExtensionsStringProc)();
    wglGetExtensionsStringProc wglGetExtensionsString = (wglGetExtensionsStringProc)getExtensionProcAddress("wglGetExtensionsStringEXT");
    if(!wglGetExtensionsString)
        return false;

    const char *exts = wglGetExtensionsString();
    if(exts && strstr(exts, ext))
        return true;

    return false;
}

void *WIN32Window::getExtensionProcAddress(const char *ext)
{
    return (void*)wglGetProcAddress(ext);
}

void WIN32Window::move(const Point& pos)
{
    RECT windowRect = {pos.x, pos.y, m_pos.x + m_size.width(), m_pos.y + m_size.height()};
    AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
    MoveWindow(m_window, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, TRUE);
}

void WIN32Window::resize(const Size& size)
{
    RECT windowRect = {m_pos.x, m_pos.y, m_pos.x + size.width(), m_pos.y + size.height()};
    AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
    MoveWindow(m_window, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, TRUE);
}

void WIN32Window::show()
{
    ShowWindow(m_window, SW_SHOW);
}

void WIN32Window::hide()
{
    ShowWindow(m_window, SW_HIDE);
}

void WIN32Window::maximize()
{
    ShowWindow(m_window, SW_MAXIMIZE);
}

void WIN32Window::poll()
{
    fireKeysPress();

    for(int i=0;i<2;++i) {
        MSG msg;
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        updateUnmaximizedCoords();

        if(m_size < m_minimumSize) {
            resize(m_size.expandedTo(m_minimumSize));
        } else
            break;
    }
}

LRESULT WIN32Window::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_SETCURSOR: {
            if(m_cursor)
                SetCursor(m_cursor);
            else
                SetCursor(m_defaultCursor);
            break;
        }
        case WM_ACTIVATE: {
            m_focused = !(wParam == WA_INACTIVE);
            break;
        }
        case WM_CHAR: {
            if(wParam >= 32 && wParam <= 255) {
                m_inputEvent.reset(Fw::KeyTextInputEvent);
                m_inputEvent.keyText = wParam;
                if(m_onInputEvent)
                    m_onInputEvent(m_inputEvent);
            }
            break;
        }
        case WM_CLOSE: {
            m_onClose();
            break;
        }
        case WM_KEYDOWN: {
            processKeyDown(m_keyMap[wParam]);
            break;
        }
        case WM_KEYUP: {
            processKeyUp(m_keyMap[wParam]);
            break;
        }
        case WM_LBUTTONDOWN: {
            m_inputEvent.reset(Fw::MousePressInputEvent);
            m_inputEvent.mouseButton = Fw::MouseLeftButton;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_LBUTTONUP: {
            m_inputEvent.reset(Fw::MouseReleaseInputEvent);
            m_inputEvent.mouseButton = Fw::MouseLeftButton;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_MBUTTONDOWN: {
            m_inputEvent.reset(Fw::MousePressInputEvent);
            m_inputEvent.mouseButton = Fw::MouseMidButton;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_MBUTTONUP: {
            m_inputEvent.reset(Fw::MouseReleaseInputEvent);
            m_inputEvent.mouseButton = Fw::MouseMidButton;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_RBUTTONDOWN: {
            m_inputEvent.reset(Fw::MousePressInputEvent);
            m_inputEvent.mouseButton = Fw::MouseRightButton;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_RBUTTONUP: {
            m_inputEvent.reset(Fw::MouseReleaseInputEvent);
            m_inputEvent.mouseButton = Fw::MouseRightButton;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_MOUSEMOVE: {
            m_inputEvent.reset(Fw::MouseMoveInputEvent);
            Point newMousePos(LOWORD(lParam), HIWORD(lParam));
            m_inputEvent.mouseMoved = newMousePos - m_inputEvent.mousePos;
            m_inputEvent.mousePos = newMousePos;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_MOUSEWHEEL: {
            m_inputEvent.mouseButton = Fw::MouseMidButton;
            m_inputEvent.wheelDirection = HIWORD(wParam) > 0 ? Fw::MouseWheelUp : Fw::MouseWheelDown;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_MOVE: {
            m_pos.x = LOWORD(lParam);
            m_pos.y = HIWORD(lParam);
            break;
        }
        case WM_SIZE: {
            switch(wParam) {
                case SIZE_MAXIMIZED:
                    m_maximized = true;
                    break;
                case SIZE_RESTORED:
                    m_maximized = false;
                    break;
            }

            m_visible = !(wParam == SIZE_MINIMIZED);
            m_size.setWidth(LOWORD(lParam));
            m_size.setHeight(HIWORD(lParam));

            m_onResize(m_size);
            break;
        }
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

void WIN32Window::swapBuffers()
{
    SwapBuffers(m_deviceContext);
}

void WIN32Window::restoreMouseCursor()
{
    logTraceDebug();
    if(m_cursor) {
        DestroyCursor(m_cursor);
        m_cursor = NULL;
        SetCursor(m_defaultCursor);
        ShowCursor(true);
    }
}

void WIN32Window::showMouse()
{
    logTraceDebug();
    ShowCursor(true);
}

void WIN32Window::hideMouse()
{
    logTraceDebug();
    ShowCursor(false);
}

void WIN32Window::displayFatalError(const std::string& message)
{
    MessageBoxA(m_window, message.c_str(), "FATAL ERROR", MB_OK | MB_ICONERROR);
}

#define LSB_BIT_SET(p, n) (p[(n)/8] |= (1 <<((n)%8)))
#define HSB_BIT_SET(p, n) (p[(n)/8] |= (128 >>((n)%8)))

void WIN32Window::setMouseCursor(const std::string& file, const Point& hotSpot)
{
    logTraceDebug();
    std::stringstream fin;
    g_resources.loadFile(file, fin);

    apng_data apng;
    if(load_apng(fin, &apng) != 0) {
        logTraceError("unable to load png file ", file);
        return;
    }

    if(apng.bpp != 4) {
        logError("the cursor png must have 4 channels");
        free_apng(&apng);
        return;
    }

    if(apng.width != 32|| apng.height != 32) {
        logError("the cursor png must have 32x32 dimension");
        free_apng(&apng);
        return;
    }

    if(m_cursor != NULL)
        DestroyCursor(m_cursor);

    int width = apng.width;
    int height = apng.height;
    int numbits = width * height;
    int numbytes = (width * height)/8;

    std::vector<uchar> andMask(numbytes, 0);
    std::vector<uchar> xorMask(numbytes, 0);

    for(int i=0;i<numbits;++i) {
        uchar r = apng.pdata[i*4+0];
        uchar g = apng.pdata[i*4+1];
        uchar b = apng.pdata[i*4+2];
        uchar a = apng.pdata[i*4+3];
        Color color(r,g,b,a);
        if(color == Fw::white) { //white
            HSB_BIT_SET(xorMask, i);
        } else if(color == Fw::alpha) { //alpha
            HSB_BIT_SET(andMask, i);
        } //otherwise black
    }
    free_apng(&apng);

    m_cursor = CreateCursor(m_instance, hotSpot.x, hotSpot.y, width, height, &andMask[0], &xorMask[0]);
    SetCursor(m_cursor);
}

void WIN32Window::setTitle(const std::string& title)
{
    SetWindowTextA(m_window, title.c_str());
}

void WIN32Window::setMinimumSize(const Size& minimumSize)
{
    m_minimumSize = minimumSize;
    if(m_size < m_minimumSize)
        resize(m_size.expandedTo(m_minimumSize));
}

void WIN32Window::setFullscreen(bool fullscreen)
{
    if(m_fullscreen == fullscreen)
        return;

    DWORD dwStyle = GetWindowLong(m_window, GWL_STYLE);
    static WINDOWPLACEMENT wpPrev = { sizeof(wpPrev) };
    if(fullscreen) {
        GetWindowPlacement(m_window, &wpPrev);
        SetWindowLong(m_window, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos(m_window, HWND_TOP, 0, 0, getDisplayWidth(), getDisplayHeight(),
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    } else {
        SetWindowLong(m_window, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(m_window, &wpPrev);
        SetWindowPos(m_window, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }

    m_fullscreen = fullscreen;
}

void WIN32Window::setVerticalSync(bool enable)
{
    if(!isExtensionSupported("WGL_EXT_swap_control"))
        return;

    typedef BOOL (WINAPI * wglSwapIntervalProc)(int);
    wglSwapIntervalProc wglSwapInterval = (wglSwapIntervalProc)getExtensionProcAddress("wglSwapIntervalEXT");
    if(!wglSwapInterval)
        return;

    wglSwapInterval(enable ? 1 : 0);
}

void WIN32Window::setIcon(const std::string& pngIcon)
{
    apng_data apng;
    std::stringstream fin;
    g_resources.loadFile(pngIcon, fin);
    if(load_apng(fin, &apng) == 0) {
        if(apng.bpp != 4) {
            logError("could not set app icon, icon image must have 4 channels");
            free_apng(&apng);
        }

        int n = apng.width * apng.height;
        std::vector<uint32> iconData(n);
        for(int i=0; i < n;++i) {
            uint8 *pixel = (uint8*)&iconData[i];
            pixel[2] = *(apng.pdata + (i * 4) + 0);
            pixel[1] = *(apng.pdata + (i * 4) + 1);
            pixel[0] = *(apng.pdata + (i * 4) + 2);
            pixel[3] = *(apng.pdata + (i * 4) + 3);
        }

        HBITMAP hbmColor = CreateBitmap(apng.width, apng.height, 1, 32, &iconData[0]);
        HBITMAP hbmMask = CreateCompatibleBitmap(GetDC(NULL), apng.width, apng.height);

        ICONINFO ii;
        ii.fIcon = TRUE;
        ii.hbmColor = hbmColor;
        ii.hbmMask = hbmMask;
        ii.xHotspot = 0;
        ii.yHotspot = 0;

        HICON icon = CreateIconIndirect(&ii);
        DeleteObject(hbmMask);
        DeleteObject(hbmColor);

        SendMessage(m_window, WM_SETICON, ICON_SMALL, (LPARAM)icon);
        SendMessage(m_window, WM_SETICON, ICON_BIG, (LPARAM)icon);

        free_apng(&apng);
    } else
        logError("could not load app icon");
}

void WIN32Window::setClipboardText(const std::string& text)
{
    if(!OpenClipboard(m_window))
        return;

    HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE, (text.length() + 1) * sizeof(TCHAR));
    if(!hglb)
        return;

    LPTSTR lptstr = (LPTSTR)GlobalLock(hglb);
    memcpy(lptstr, &text[0], text.length() * sizeof(TCHAR));
    lptstr[text.length()] = (TCHAR)0;
    GlobalUnlock(hglb);

    EmptyClipboard();
    SetClipboardData(CF_TEXT, hglb);
    CloseClipboard();
}

Size WIN32Window::getDisplaySize()
{
    return Size(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
}

std::string WIN32Window::getClipboardText()
{
    std::string text;

    if(!OpenClipboard(m_window))
        return text;

    HGLOBAL hglb = GetClipboardData(CF_TEXT);
    if(hglb) {
        LPTSTR lptstr = (LPTSTR)GlobalLock(hglb);
        if(lptstr) {
            text = Fw::utf8StringToLatin1((uchar*)lptstr);
            GlobalUnlock(hglb);
        }
    }
    CloseClipboard();
    return text;
}

std::string WIN32Window::getPlatformType()
{
    return "WIN32-WGL";
}
