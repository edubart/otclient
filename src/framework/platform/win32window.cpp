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

#ifdef WIN32

#include "win32window.h"
#include <framework/graphics/image.h>
#include <framework/core/application.h>
#include <framework/core/resourcemanager.h>

#define HSB_BIT_SET(p, n) (p[(n)/8] |= (128 >>((n)%8)))

WIN32Window::WIN32Window()
{
    m_window = 0;
    m_instance = 0;
    m_deviceContext = 0;
    m_cursor = 0;
    m_minimumSize = Size(600,480);
    m_size = Size(600,480);
    m_hidden = true;

#ifdef OPENGL_ES
    m_eglConfig = 0;
    m_eglContext = 0;
    m_eglDisplay = 0;
    m_eglSurface = 0;
#else
    m_wglContext = 0;
#endif

    m_keyMap[VK_ESCAPE] = Fw::KeyEscape;
    m_keyMap[VK_TAB] = Fw::KeyTab;
    m_keyMap[VK_RETURN] = Fw::KeyEnter;
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
    m_keyMap[VK_RMENU] = Fw::KeyAlt;
    m_keyMap[VK_LWIN] = Fw::KeyMeta;
    m_keyMap[VK_RWIN] = Fw::KeyMeta;
    m_keyMap[VK_APPS] = Fw::KeyMenu;

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
    /*
    m_keyMap[VK_ADD] = Fw::KeyPlus;
    m_keyMap[VK_SUBTRACT] = Fw::KeyMinus;
    m_keyMap[VK_DECIMAL] = Fw::KeyPeriod;
    m_keyMap[VK_DIVIDE] = Fw::KeySlash;
    m_keyMap[VK_MULTIPLY] = Fw::KeyAsterisk;
    */

    m_keyMap[VK_OEM_1] = Fw::KeySemicolon;
    m_keyMap[VK_OEM_2] = Fw::KeySlash;
    m_keyMap[VK_OEM_3] = Fw::KeyGrave;
    m_keyMap[VK_OEM_4] = Fw::KeyLeftBracket;
    m_keyMap[VK_OEM_5] = Fw::KeyBackslash;
    m_keyMap[VK_OEM_6] = Fw::KeyRightBracket;
    m_keyMap[VK_OEM_7] = Fw::KeyApostrophe;
    m_keyMap[VK_OEM_MINUS] = Fw::KeyMinus;
    m_keyMap[VK_OEM_PLUS] = Fw::KeyEqual;
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

#ifdef DIRECTX
    m_d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

    D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

    ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
    d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.hDeviceWindow = m_window;    // set the window to be used by Direct3D

    // create a device class using this information and information from the d3dpp stuct
    m_d3d->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      m_window,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                      &d3dpp,
                      &m_d3ddev);

#endif

    internalCreateWindow();
    internalCreateGLContext();
    internalRestoreGLContext();
}

void WIN32Window::terminate()
{
    SetCursor(NULL);
    if(m_defaultCursor) {
        DestroyCursor(m_defaultCursor);
        m_defaultCursor = NULL;
    }

    for(HCURSOR& cursor : m_cursors)
        DestroyCursor(cursor);
    m_cursors.clear();

    internalDestroyGLContext();

    if(m_deviceContext) {
        if(!ReleaseDC(m_window, m_deviceContext))
            g_logger.error("Release device context failed.");
        m_deviceContext = NULL;
    }

    if(m_window) {
        if(!DestroyWindow(m_window))
            g_logger.error("ERROR: Destroy window failed.");
        m_window = NULL;
    }

    if(m_instance) {
        if(!UnregisterClassA(g_app.getCompactName().c_str(), m_instance))
            g_logger.error("UnregisterClassA failed");
        m_instance = NULL;
    }
}

struct WindowProcProxy {
    static LRESULT CALLBACK call(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        WIN32Window *window = (WIN32Window*)&g_window;
        return window->windowProc(hWnd, uMsg, wParam, lParam);
    }
};

void WIN32Window::internalCreateWindow()
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
    wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = g_app.getCompactName().c_str();

    if(!RegisterClassA(&wc))
        g_logger.fatal("Failed to register the window class.");
    DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD dwStyle = WS_OVERLAPPEDWINDOW;

    // initialize in the center of the screen
    m_position = ((getDisplaySize() - m_size) / 2).toPoint();

    Rect screenRect = adjustWindowRect(Rect(m_position, m_size));

    updateUnmaximizedCoords();
    m_window = CreateWindowExA(dwExStyle,
                               g_app.getCompactName().c_str(),
                               NULL,
                               dwStyle,
                               screenRect.left(),
                               screenRect.top(),
                               screenRect.width(),
                               screenRect.height(),
                               NULL,
                               NULL,
                               m_instance,
                               NULL);

    if(!m_window)
        g_logger.fatal("Unable to create window");

    ShowWindow(m_window, SW_HIDE);

    m_deviceContext = GetDC(m_window);
    if(!m_deviceContext)
        g_logger.fatal("GetDC failed");
}

void WIN32Window::internalCreateGLContext()
{
#ifdef OPENGL_ES
    m_eglDisplay = eglGetDisplay(m_deviceContext);
    if(m_eglDisplay == EGL_NO_DISPLAY)
        g_logger.fatal("EGL not supported");

    if(!eglInitialize(m_eglDisplay, NULL, NULL))
        g_logger.fatal("Unable to initialize EGL");

    static int configList[] = {
#if OPENGL_ES==2
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#else
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
#endif
        EGL_RED_SIZE, 4,
        EGL_GREEN_SIZE, 4,
        EGL_BLUE_SIZE, 4,
        EGL_ALPHA_SIZE, 4,
        EGL_NONE
    };

    EGLint numConfig;

    if(!eglGetConfigs(m_eglDisplay, NULL, 0, &numConfig))
        g_logger.fatal("No valid GL configurations");

    if(!eglChooseConfig(m_eglDisplay, configList, &m_eglConfig, 1, &numConfig))
        g_logger.fatal("Failed to choose EGL config");

    if(numConfig != 1)
        g_logger.warning("Didn't got the exact EGL config");

    EGLint contextAtrrList[] = {
#if OPENGL_ES==2
        EGL_CONTEXT_CLIENT_VERSION, 2,
#else
        EGL_CONTEXT_CLIENT_VERSION, 1,
#endif
        EGL_NONE
    };

    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_window, NULL);
    if(m_eglSurface == EGL_NO_SURFACE)
        g_logger.fatal(stdext::format("Unable to create EGL surface: %s", eglGetError()));

    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, contextAtrrList);
    if(m_eglContext == EGL_NO_CONTEXT )
        g_logger.fatal(stdext::format("Unable to create EGL context: %s", eglGetError()));

#else
    uint pixelFormat;
    static PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR),
                                         1,
                                         PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                                         PFD_TYPE_RGBA,
                                         32,                         // Select Our Color Depth
                                         8, 0, 8, 0, 8, 0,           // Color Bits Ignored
                                         8,                          // Alpha Buffer Bits
                                         0,                          // Shift Bit Ignored
                                         0,                          // No Accumulation Buffer
                                         0, 0, 0, 0,                 // Accumulation Bits Ignored
                                         0,                          // Z-Buffer (Depth Buffer)
                                         0,                          // No Stencil Buffer
                                         0,                          // No Auxiliary Buffer
                                         PFD_MAIN_PLANE,             // Main Drawing Layer
                                         0,                          // Reserved
                                         0, 0, 0 };                  // Layer Masks Ignored

    pixelFormat = ChoosePixelFormat(m_deviceContext, &pfd);
    if(!pixelFormat)
        g_logger.fatal("Could not find a suitable pixel format");

    if(!SetPixelFormat(m_deviceContext, pixelFormat, &pfd))
        g_logger.fatal("Could not set the pixel format");

    if(!(m_wglContext = wglCreateContext(m_deviceContext)))
        g_logger.fatal("Unable to create GL context");
#endif
}

void WIN32Window::internalDestroyGLContext()
{
#ifdef OPENGL_ES
    if(m_eglDisplay) {
        if(m_eglContext) {
            eglDestroyContext(m_eglDisplay, m_eglContext);
            m_eglContext = 0;
        }
        if(m_eglSurface) {
            eglDestroySurface(m_eglDisplay, m_eglSurface);
            m_eglSurface = 0;
        }
        eglTerminate(m_eglDisplay);
        m_eglDisplay = 0;
    }
#else
    if(m_wglContext) {
        if(!wglMakeCurrent(NULL, NULL))
            g_logger.error("Release of dc and rc failed.");
        if(!wglDeleteContext(m_wglContext))
            g_logger.error("Release rendering context failed.");
        m_wglContext = NULL;
    }
#endif
}

void WIN32Window::internalRestoreGLContext()
{
#ifdef OPENGL_ES
    if(!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext))
        g_logger.fatal("Unable to make current EGL context");
#else
    if(!wglMakeCurrent(m_deviceContext, m_wglContext))
        g_logger.fatal("Unable to make current WGL context");
#endif
}

bool WIN32Window::isExtensionSupported(const char *ext)
{
#ifdef OPENGL_ES
    //TODO
    return false;
#else
    typedef const char* (WINAPI * wglGetExtensionsStringProc)();
    wglGetExtensionsStringProc wglGetExtensionsString = (wglGetExtensionsStringProc)getExtensionProcAddress("wglGetExtensionsStringEXT");
    if(!wglGetExtensionsString)
        return false;

    const char *exts = wglGetExtensionsString();
    if(exts && strstr(exts, ext))
        return true;

    return false;
#endif
}

void *WIN32Window::getExtensionProcAddress(const char *ext)
{
#ifdef OPENGL_ES
    //TODO
    return NULL;
#else
    return (void*)wglGetProcAddress(ext);
#endif
}

void WIN32Window::move(const Point& pos)
{
    Rect clientRect(pos, getClientRect().size());
    Rect windowRect = adjustWindowRect(clientRect);
    MoveWindow(m_window, windowRect.x(), windowRect.y(), windowRect.width(), windowRect.height(), TRUE);
    if(m_hidden)
        ShowWindow(m_window, SW_HIDE);
}

void WIN32Window::resize(const Size& size)
{
    if(size.width() < m_minimumSize.width() || size.height() < m_minimumSize.height())
        return;
    Rect clientRect(getClientRect().topLeft(), size);
    Rect windowRect = adjustWindowRect(clientRect);
    MoveWindow(m_window, windowRect.x(), windowRect.y(), windowRect.width(), windowRect.height(), TRUE);
    if(m_hidden)
        ShowWindow(m_window, SW_HIDE);
}

void WIN32Window::show()
{
    m_hidden = false;
    if(m_maximized)
        ShowWindow(m_window, SW_MAXIMIZE);
    else
        ShowWindow(m_window, SW_SHOW);
}

void WIN32Window::hide()
{
    m_hidden = true;
    ShowWindow(m_window, SW_HIDE);
}

void WIN32Window::maximize()
{
    if(!m_hidden)
        ShowWindow(m_window, SW_MAXIMIZE);
    else
        m_maximized = true;
}

void WIN32Window::poll()
{
    fireKeysPress();

    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    updateUnmaximizedCoords();
}

Fw::Key WIN32Window::retranslateVirtualKey(WPARAM wParam, LPARAM lParam)
{
    // ignore numpad keys when numlock is on
    if((wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9) || wParam == VK_SEPARATOR)
        return Fw::KeyUnknown;

    // lParam will have this state when receiving insert,end,down,etc presses from numpad
    if(!(((HIWORD(lParam) >> 8) & 0xFF) & 1)) {
        bool numlockOn = GetKeyState(VK_NUMLOCK);
        // retranslate numpad keys
        switch(wParam) {
            case VK_INSERT:
                return numlockOn ? Fw::KeyUnknown : Fw::KeyNumpad0;
            case VK_END:
                return numlockOn ? Fw::KeyUnknown : Fw::KeyNumpad1;
            case VK_DOWN:
                return numlockOn ? Fw::KeyUnknown : Fw::KeyNumpad2;
            case VK_NEXT:
                return numlockOn ? Fw::KeyUnknown : Fw::KeyNumpad3;
            case VK_LEFT:
                return numlockOn ? Fw::KeyUnknown : Fw::KeyNumpad4;
            case VK_CLEAR:
                return numlockOn ? Fw::KeyUnknown : Fw::KeyNumpad5;
            case VK_RIGHT:
                return numlockOn ? Fw::KeyUnknown : Fw::KeyNumpad6;
            case VK_HOME:
                return numlockOn ? Fw::KeyUnknown : Fw::KeyNumpad7;
            case VK_UP:
                return numlockOn ? Fw::KeyUnknown : Fw::KeyNumpad8;
            case VK_PRIOR:
                return numlockOn ? Fw::KeyUnknown : Fw::KeyNumpad9;
        }
    }

    Fw::Key key = Fw::KeyUnknown;
    if(m_keyMap.find(wParam) != m_keyMap.end())
        key = m_keyMap[wParam];

    // actually ignore alt/ctrl/shift keys, they is states are already stored in m_inputEvent.keyboardModifiers
    if(key == Fw::KeyAlt || key == Fw::KeyCtrl || key == Fw::KeyShift)
        key = Fw::KeyUnknown;

    return key;
}

#define IsKeyDown(a) (GetKeyState(a) & 0x80)

LRESULT WIN32Window::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_inputEvent.keyboardModifiers = 0;
    if(IsKeyDown(VK_CONTROL))
        m_inputEvent.keyboardModifiers |= Fw::KeyboardCtrlModifier;
    if(IsKeyDown(VK_SHIFT))
        m_inputEvent.keyboardModifiers |= Fw::KeyboardShiftModifier;
    if(IsKeyDown(VK_MENU))
        m_inputEvent.keyboardModifiers |= Fw::KeyboardAltModifier;

    switch(uMsg)
    {
        case WM_SETCURSOR: {
            if(m_cursor)
                SetCursor(m_cursor);
            else
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
        }
        case WM_ACTIVATE: {
            m_focused = !(wParam == WA_INACTIVE);
            releaseAllKeys();
            break;
        }
        case WM_SETFOCUS:
        case WM_KILLFOCUS: {
            releaseAllKeys();
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
            processKeyDown(retranslateVirtualKey(wParam, lParam));
            break;
        }
        case WM_KEYUP: {
            processKeyUp(retranslateVirtualKey(wParam, lParam));
            break;
        }
        case WM_SYSKEYUP: {
            processKeyUp(retranslateVirtualKey(wParam, lParam));
            break;
        }
        case WM_SYSKEYDOWN: {
            if(wParam == VK_F4 && m_inputEvent.keyboardModifiers & Fw::KeyboardAltModifier)
                return DefWindowProc(hWnd, uMsg, wParam, lParam);

            processKeyDown(retranslateVirtualKey(wParam, lParam));
            break;
        }
        case WM_LBUTTONDOWN: {
            SetCapture(m_window);
            m_inputEvent.reset(Fw::MousePressInputEvent);
            m_inputEvent.mouseButton = Fw::MouseLeftButton;
            m_mouseButtonStates[Fw::MouseLeftButton] = true;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_LBUTTONUP: {
            SetCapture(NULL);
            m_inputEvent.reset(Fw::MouseReleaseInputEvent);
            m_inputEvent.mouseButton = Fw::MouseLeftButton;
            m_mouseButtonStates[Fw::MouseLeftButton] = false;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_MBUTTONDOWN: {
            SetCapture(m_window);
            m_inputEvent.reset(Fw::MousePressInputEvent);
            m_inputEvent.mouseButton = Fw::MouseMidButton;
            m_mouseButtonStates[Fw::MouseMidButton] = true;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_MBUTTONUP: {
            SetCapture(NULL);
            m_inputEvent.reset(Fw::MouseReleaseInputEvent);
            m_inputEvent.mouseButton = Fw::MouseMidButton;
            m_mouseButtonStates[Fw::MouseMidButton] = false;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_RBUTTONDOWN: {
            SetCapture(m_window);
            m_inputEvent.reset(Fw::MousePressInputEvent);
            m_inputEvent.mouseButton = Fw::MouseRightButton;
            m_mouseButtonStates[Fw::MouseRightButton] = true;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_RBUTTONUP: {
            SetCapture(NULL);
            m_inputEvent.reset(Fw::MouseReleaseInputEvent);
            m_inputEvent.mouseButton = Fw::MouseRightButton;
            m_mouseButtonStates[Fw::MouseRightButton] = false;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_MOUSEMOVE: {
            m_inputEvent.reset(Fw::MouseMoveInputEvent);

            Point newMousePos(LOWORD(lParam), HIWORD(lParam));
            if(newMousePos.x >= 32767)
                newMousePos.x = 0;
            else
                newMousePos.x = std::min<int32>(newMousePos.x, m_size.width());

            if(newMousePos.y >= 32767)
                newMousePos.y = 0;
            else
                newMousePos.y = std::min<int32>(newMousePos.y, m_size.height());

            m_inputEvent.mouseMoved = newMousePos - m_inputEvent.mousePos;
            m_inputEvent.mousePos = newMousePos;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_MOUSEWHEEL: {
            m_inputEvent.reset(Fw::MouseWheelInputEvent);
            m_inputEvent.mouseButton = Fw::MouseMidButton;
            m_inputEvent.wheelDirection = ((short)HIWORD(wParam)) > 0 ? Fw::MouseWheelUp : Fw::MouseWheelDown;
            if(m_onInputEvent)
                m_onInputEvent(m_inputEvent);
            break;
        }
        case WM_MOVE: {
            m_position.x = (short)LOWORD(lParam);
            m_position.y = (short)HIWORD(lParam);
            break;
        }
        case WM_GETMINMAXINFO: {
            LPMINMAXINFO pMMI = (LPMINMAXINFO)lParam;
            Rect adjustedRect = adjustWindowRect(Rect(0, 0, m_minimumSize));
            pMMI->ptMinTrackSize.x = adjustedRect.width();
            pMMI->ptMinTrackSize.y = adjustedRect.height();
            break;
        }
        case WM_SIZE: {
            bool forceResize = false;
            switch(wParam) {
                case SIZE_MAXIMIZED:
                    m_maximized = true;
                    m_visible = true;
                    forceResize = true;
                    break;
                case SIZE_RESTORED:
                    m_maximized = false;
                    m_visible = true;
                    forceResize = true;
                    break;
                case SIZE_MINIMIZED:
                    m_visible = false;
                    break;
            }

            if(m_visible && m_deviceContext)
                internalRestoreGLContext();

            Size size = Size(LOWORD(lParam), HIWORD(lParam));
            size.setWidth(std::max<int32>(std::min<int32>(size.width(), 7680), 32));
            size.setHeight(std::max<int32>(std::min<int32>(size.height(), 4320), 32));

            if(m_visible && (forceResize || m_size != size)) {
                m_size = size;
                m_onResize(m_size);
            }

            break;
        }
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

void WIN32Window::swapBuffers()
{
#ifdef OPENGL_ES
    eglSwapBuffers(m_eglDisplay, m_eglSurface);
#else
    SwapBuffers(m_deviceContext);
#endif
}

void WIN32Window::showMouse()
{
    ShowCursor(true);
}

void WIN32Window::hideMouse()
{
    ShowCursor(false);
}

void WIN32Window::displayFatalError(const std::string& message)
{
    MessageBoxW(m_window, stdext::latin1_to_utf16(message).c_str(), L"FATAL ERROR", MB_OK | MB_ICONERROR);
}

int WIN32Window::internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot)
{
    int width = image->getWidth();
    int height = image->getHeight();
    int numbits = width * height;
    int numbytes = (width * height)/8;

    std::vector<uchar> andMask(numbytes, 0);
    std::vector<uchar> xorMask(numbytes, 0);

    for(int i=0;i<numbits;++i) {
        uint32 rgba = stdext::readULE32(image->getPixelData() + i*4);
        if(rgba == 0xffffffff) { //white
            HSB_BIT_SET(xorMask, i);
        } else if(rgba == 0x00000000) { //alpha
            HSB_BIT_SET(andMask, i);
        } // otherwise 0xff000000 => black
    }

    HCURSOR cursor = CreateCursor(m_instance, hotSpot.x, hotSpot.y, width, height, &andMask[0], &xorMask[0]);
    m_cursors.push_back(cursor);
    return m_cursors.size()-1;
}

void WIN32Window::setMouseCursor(int cursorId)
{
    if(cursorId >= (int)m_cursors.size() || cursorId < 0)
        return;

    m_cursor = m_cursors[cursorId];
    SetCursor(m_cursor);
    ShowCursor(true);
}

void WIN32Window::restoreMouseCursor()
{
    if(m_cursor) {
        m_cursor = NULL;
        SetCursor(m_defaultCursor);
        ShowCursor(true);
    }
}

void WIN32Window::setTitle(const std::string& title)
{
    SetWindowTextW(m_window, stdext::latin1_to_utf16(title).c_str());
}

void WIN32Window::setMinimumSize(const Size& minimumSize)
{
    m_minimumSize = minimumSize;
}

void WIN32Window::setFullscreen(bool fullscreen)
{
    if(m_fullscreen == fullscreen)
        return;

    m_fullscreen = fullscreen;

    DWORD dwStyle = GetWindowLong(m_window, GWL_STYLE);
    static WINDOWPLACEMENT wpPrev;
    wpPrev.length = sizeof(wpPrev);

    if(fullscreen) {
        MONITORINFO mi;
        HMONITOR m = MonitorFromWindow(m_window, MONITOR_DEFAULTTONEAREST);
        mi.cbSize = sizeof(mi);
        GetMonitorInfoW(m, &mi);
        uint x = mi.rcMonitor.left;
        uint y = mi.rcMonitor.top;
        uint width = mi.rcMonitor.right - mi.rcMonitor.left;
        uint height = mi.rcMonitor.bottom - mi.rcMonitor.top;

        GetWindowPlacement(m_window, &wpPrev);

        SetWindowLong(m_window, GWL_STYLE, (dwStyle & ~WS_OVERLAPPEDWINDOW) | WS_POPUP | WS_EX_TOPMOST);
        SetWindowPos(m_window, HWND_TOPMOST, x, y, width, height, SWP_FRAMECHANGED);
    } else {
        SetWindowLong(m_window, GWL_STYLE, (dwStyle & ~(WS_POPUP | WS_EX_TOPMOST)) | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(m_window, &wpPrev);
        SetWindowPos(m_window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    }
}

void WIN32Window::setVerticalSync(bool enable)
{
#ifdef OPENGL_ES
    eglSwapInterval(m_eglDisplay, enable ? 1 : 0);
#else
    if(!isExtensionSupported("WGL_EXT_swap_control"))
        return;

    typedef BOOL (WINAPI * wglSwapIntervalProc)(int);
    wglSwapIntervalProc wglSwapInterval = (wglSwapIntervalProc)getExtensionProcAddress("wglSwapIntervalEXT");
    if(!wglSwapInterval)
        return;

    wglSwapInterval(enable ? 1 : 0);
#endif
}

void WIN32Window::setIcon(const std::string& file)
{
    ImagePtr image = Image::load(file);

    if(!image) {
        g_logger.traceError(stdext::format("unable to load icon file %s", file));
        return;
    }

    if(image->getBpp() != 4) {
        g_logger.error("the app icon must have 4 channels");
        return;
    }

    int n = image->getWidth() * image->getHeight();
    std::vector<uint32> iconData(n);
    for(int i=0; i < n;++i) {
        uint8 *pixel = (uint8*)&iconData[i];
        pixel[2] = *(image->getPixelData() + (i * 4) + 0);
        pixel[1] = *(image->getPixelData() + (i * 4) + 1);
        pixel[0] = *(image->getPixelData() + (i * 4) + 2);
        pixel[3] = *(image->getPixelData() + (i * 4) + 3);
    }

    HBITMAP hbmColor = CreateBitmap(image->getWidth(), image->getHeight(), 1, 32, &iconData[0]);
    HBITMAP hbmMask = CreateCompatibleBitmap(GetDC(NULL), image->getWidth(), image->getHeight());

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
}

void WIN32Window::setClipboardText(const std::string& text)
{
    if(!OpenClipboard(m_window))
        return;

    HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE, (text.length() + 1) * sizeof(WCHAR));
    if(!hglb)
        return;

    std::wstring wtext = stdext::latin1_to_utf16(text);

    LPWSTR lpwstr = (LPWSTR)GlobalLock(hglb);
    memcpy(lpwstr, (char*)&wtext[0], wtext.length() * sizeof(WCHAR));
    lpwstr[text.length()] = (WCHAR)0;
    GlobalUnlock(hglb);

    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, hglb);
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

    HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);
    if(hglb) {
        LPWSTR lpwstr = (LPWSTR)GlobalLock(hglb);
        if(lpwstr) {
            text = stdext::utf16_to_latin1(lpwstr);
            GlobalUnlock(hglb);
        }
    }
    CloseClipboard();
    return text;
}


std::string WIN32Window::getPlatformType()
{
#ifndef OPENGL_ES
    return "WIN32-WGL";
#else
    return "WIN32-EGL";
#endif
}

Rect WIN32Window::getClientRect()
{
    if(m_window) {
        RECT clientRect = {0,0,0,0};
        int ret = GetClientRect(m_window, &clientRect);
        assert(ret != 0);
        return Rect(Point(clientRect.left, clientRect.top), Point(clientRect.right, clientRect.bottom));
    } else {
        return Rect(m_position, m_size);
    }
}

Rect WIN32Window::getWindowRect()
{
    if(m_window) {
        RECT windowRect = {0,0,0,0};
        int ret = GetWindowRect(m_window, &windowRect);
        assert(ret != 0);
        return Rect(Point(windowRect.left, windowRect.top), Point(windowRect.right, windowRect.bottom));
    } else {
        return adjustWindowRect(getClientRect());
    }
}

Rect WIN32Window::adjustWindowRect(const Rect& clientRect)
{
    Rect rect;
    DWORD dwStyle;
    DWORD dwExStyle;
    RECT windowRect = { clientRect.left(), clientRect.top(), clientRect.right(), clientRect.bottom() };
    if(m_window) {
        dwStyle = GetWindowLong(m_window, GWL_STYLE);
        dwExStyle = GetWindowLong(m_window, GWL_EXSTYLE);
    } else {
        dwStyle = WS_OVERLAPPEDWINDOW;
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    }
    if(AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle) != 0) {
        rect = Rect(Point(windowRect.left, windowRect.top), Point(windowRect.right, windowRect.bottom));
    } else {
        g_logger.traceError("AdjustWindowRectEx failed");
        rect = Rect(0,0, m_minimumSize);
    }
    return rect;
}

#endif
