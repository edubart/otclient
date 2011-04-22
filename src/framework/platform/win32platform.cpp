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


#include <prerequisites.h>
#include <core/platform.h>
#include <core/engine.h>

#include <windows.h>
#include <dir.h>

#include <physfs.h>

#include <GL/gl.h>
#include <GL/glext.h>

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
    bool focused, visible, maximized;

    std::map<int, uchar> keyMap;
} win32;

void Platform::init(const char *appName)
{
    win32.appName = appName;
    win32.instance = GetModuleHandle(NULL);

    // setup keys
    win32.keyMap[0x30] = KC_1;
    win32.keyMap[0x31] = KC_2;
    win32.keyMap[0x32] = KC_3;
    win32.keyMap[0x33] = KC_4;
    win32.keyMap[0x34] = KC_5;
    win32.keyMap[0x35] = KC_6;
    win32.keyMap[0x36] = KC_7;
    win32.keyMap[0x37] = KC_8;
    win32.keyMap[0x38] = KC_9;
    win32.keyMap[0x39] = KC_0;

    win32.keyMap[VK_BACK] = KC_BACK;

    win32.keyMap[VK_OEM_MINUS] = KC_MINUS;
    win32.keyMap[VK_OEM_PLUS] = KC_EQUALS;
    win32.keyMap[VK_SPACE] = KC_SPACE;
    win32.keyMap[VK_OEM_COMMA] = KC_COMMA;
    win32.keyMap[VK_OEM_PERIOD] = KC_PERIOD;

    //win32.keyMap[VK_OEM_102] = KC_BACKSLASH;
    //win32.keyMap[XK_slash] = KC_SLASH;
    //win32.keyMap[XK_bracketleft] = KC_LBRACKET;
    //win32.keyMap[XK_bracketright] = KC_RBRACKET;

    win32.keyMap[VK_ESCAPE] = KC_ESCAPE;
    win32.keyMap[VK_CAPITAL] = KC_CAPITAL;

    win32.keyMap[VK_TAB] = KC_TAB;
    win32.keyMap[VK_RETURN] = KC_RETURN;
    win32.keyMap[VK_LCONTROL] = KC_LCONTROL;
    win32.keyMap[VK_RCONTROL] = KC_RCONTROL;

    //win32.keyMap[XK_colon] = KC_COLON;
    //win32.keyMap[XK_semicolon] = KC_SEMICOLON;
    //win32.keyMap[XK_apostrophe] = KC_APOSTROPHE;
    //win32.keyMap[XK_grave] = KC_GRAVE;

    win32.keyMap[0x41] = KC_A;
    win32.keyMap[0x42] = KC_B;
    win32.keyMap[0x43] = KC_C;
    win32.keyMap[0x44] = KC_D;
    win32.keyMap[0x45] = KC_E;
    win32.keyMap[0x46] = KC_F;
    win32.keyMap[0x47] = KC_G;
    win32.keyMap[0x48] = KC_H;
    win32.keyMap[0x49] = KC_I;
    win32.keyMap[0x4A] = KC_J;
    win32.keyMap[0x4B] = KC_K;
    win32.keyMap[0x4C] = KC_L;
    win32.keyMap[0x4D] = KC_M;
    win32.keyMap[0x4E] = KC_N;
    win32.keyMap[0x4F] = KC_O;
    win32.keyMap[0x50] = KC_P;
    win32.keyMap[0x51] = KC_Q;
    win32.keyMap[0x52] = KC_R;
    win32.keyMap[0x53] = KC_S;
    win32.keyMap[0x54] = KC_T;
    win32.keyMap[0x55] = KC_U;
    win32.keyMap[0x56] = KC_V;
    win32.keyMap[0x57] = KC_W;
    win32.keyMap[0x58] = KC_X;
    win32.keyMap[0x59] = KC_Y;
    win32.keyMap[0x5A] = KC_Z;

    win32.keyMap[VK_F1] = KC_F1;
    win32.keyMap[VK_F2] = KC_F2;
    win32.keyMap[VK_F3] = KC_F3;
    win32.keyMap[VK_F4] = KC_F4;
    win32.keyMap[VK_F5] = KC_F5;
    win32.keyMap[VK_F6] = KC_F6;
    win32.keyMap[VK_F7] = KC_F7;
    win32.keyMap[VK_F8] = KC_F8;
    win32.keyMap[VK_F9] = KC_F9;
    win32.keyMap[VK_F10] = KC_F10;
    win32.keyMap[VK_F11] = KC_F11;
    win32.keyMap[VK_F12] = KC_F12;
    win32.keyMap[VK_F13] = KC_F13;
    win32.keyMap[VK_F14] = KC_F14;
    win32.keyMap[VK_F15] = KC_F15;

    // keypad
    win32.keyMap[VK_NUMPAD0] = KC_NUMPAD0;
    win32.keyMap[VK_NUMPAD1] = KC_NUMPAD1;
    win32.keyMap[VK_NUMPAD2] = KC_NUMPAD2;
    win32.keyMap[VK_NUMPAD3] = KC_NUMPAD3;
    win32.keyMap[VK_NUMPAD4] = KC_NUMPAD4;
    win32.keyMap[VK_NUMPAD5] = KC_NUMPAD5;
    win32.keyMap[VK_NUMPAD6] = KC_NUMPAD6;
    win32.keyMap[VK_NUMPAD7] = KC_NUMPAD7;
    win32.keyMap[VK_NUMPAD8] = KC_NUMPAD8;
    win32.keyMap[VK_NUMPAD9] = KC_NUMPAD9;
    win32.keyMap[VK_ADD] = KC_ADD;
    win32.keyMap[VK_SUBTRACT] = KC_SUBTRACT;
    win32.keyMap[VK_DECIMAL] = KC_DECIMAL;
    //win32.keyMap[XK_KP_Equal] = KC_NUMPADEQUALS;
    win32.keyMap[VK_DIVIDE] = KC_DIVIDE;
    win32.keyMap[VK_MULTIPLY] = KC_MULTIPLY;
    win32.keyMap[VK_SEPARATOR] = KC_NUMPADENTER;

    // keypad with numlock off
    //win32.keyMap[XK_KP_Home] = KC_NUMPAD7;
    //win32.keyMap[XK_KP_Up] = KC_NUMPAD8;
    //win32.keyMap[XK_KP_Page_Up] = KC_NUMPAD9;
    //win32.keyMap[XK_KP_Left] = KC_NUMPAD4;
    //win32.keyMap[XK_KP_Begin] = KC_NUMPAD5;
    //win32.keyMap[XK_KP_Right] = KC_NUMPAD6;
    //win32.keyMap[XK_KP_End] = KC_NUMPAD1;
    //win32.keyMap[XK_KP_Down] = KC_NUMPAD2;
    //win32.keyMap[XK_KP_Page_Down] = KC_NUMPAD3;
    //win32.keyMap[XK_KP_Insert] = KC_NUMPAD0;
    //win32.keyMap[XK_KP_Delete] = KC_DECIMAL;

    win32.keyMap[VK_UP] = KC_UP;
    win32.keyMap[VK_DOWN] = KC_DOWN;
    win32.keyMap[VK_LEFT] = KC_LEFT;
    win32.keyMap[VK_RIGHT] = KC_RIGHT;

    win32.keyMap[VK_PRIOR] = KC_PGUP;
    win32.keyMap[VK_NEXT] = KC_PGDOWN;
    win32.keyMap[VK_HOME] = KC_HOME;
    win32.keyMap[VK_END] = KC_END;

    win32.keyMap[VK_NUMLOCK] = KC_NUMLOCK;
    win32.keyMap[VK_SNAPSHOT] = KC_SYSRQ;
    win32.keyMap[VK_SCROLL] = KC_SCROLL;
    win32.keyMap[VK_PAUSE] = KC_PAUSE;

    win32.keyMap[VK_RSHIFT] = KC_RSHIFT;
    win32.keyMap[VK_LSHIFT] = KC_LSHIFT;
    win32.keyMap[VK_RMENU] = KC_RALT;
    win32.keyMap[VK_LMENU] = KC_LALT;

    win32.keyMap[VK_INSERT] = KC_INSERT;
    win32.keyMap[VK_DELETE] = KC_DELETE;

    win32.keyMap[VK_LWIN] = KC_LWIN;
    win32.keyMap[VK_RWIN] = KC_RWIN;
    win32.keyMap[VK_APPS] = KC_APPS;


    // win class
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
        logFatal("FATAL ERROR: Failed to register the window class.");

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
            logError("ERROR: Unregister class failed.");

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
    static ulong firstTick = 0;
    if(!firstTick)
        firstTick = GetTickCount();
    
    return (uint32_t)(GetTickCount() - firstTick);
}

void Platform::sleep(ulong miliseconds)
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
        logFatal("FATAL ERROR: Window creation error.");
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
        logFatal("FATAL ERROR: Can't Create A GL Device Context.");
        return false;
    }

    if(!(pixelFormat = ChoosePixelFormat(win32.hdc, &pfd))) {
        terminate();
        logFatal("FATAL ERROR: Can't Find A Suitable PixelFormat.");
        return false;
    }

    if(!SetPixelFormat(win32.hdc, pixelFormat, &pfd)) {
        terminate();
        logFatal("FATAL ERROR: Can't Set The PixelFormat.");
        return false;
    }

    if(!(win32.hrc = wglCreateContext(win32.hdc))) {
        terminate();
        logFatal("FATAL ERROR: Can't Create A GL Rendering Context.");
        return false;
    }

    if(!wglMakeCurrent(win32.hdc, win32.hrc)) {
        terminate();
        logFatal("FATAL ERROR: Can't Activate The GL Rendering Context.");
        return false;
    }

    return true;
}

void Platform::destroyWindow()
{
    if(win32.hrc) {
        if(!wglMakeCurrent(NULL, NULL))
            logError("ERROR: Release Of DC And RC Failed.");

        if(!wglDeleteContext(win32.hrc))
            logError("ERROR: Release Rendering Context Failed.");

        win32.hrc = NULL;
    }

    if(win32.hdc) {
        if(!ReleaseDC(win32.window, win32.hdc))
            logError("ERROR: Release Device Context Failed.");

        win32.hdc = NULL;
    }

    if(win32.window) {
        if(!DestroyWindow(win32.window))
            logError("ERROR: Destroy window failed.");

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

void Platform::setVsync(bool enable /*= true*/)
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

bool Platform::isWindowFocused()
{
    return win32.focused;
}

bool Platform::isWindowVisible()
{
    return win32.visible;
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

std::string Platform::getAppUserDir()
{
    std::stringstream sdir;
    sdir << PHYSFS_getUserDir() << "/." << win32.appName << "/";
    if((mkdir(sdir.str().c_str()) != 0) && (errno != EEXIST))
        logError("ERROR: Couldn't create directory for saving configuration file. (%s)", sdir.str().c_str());
    return sdir.str();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static int lastX, lastY;
    static InputEvent inputEvent;

    switch(uMsg)
    {
    case WM_ACTIVATE:
        {
            win32.focused = !(wParam == WA_INACTIVE);
            break;
        }
    case WM_CHAR:
        {
            if(wParam >= 32 && wParam <= 255) {
                inputEvent.type = EV_TEXT_ENTER;
                inputEvent.keychar = wParam;
                inputEvent.keycode = KC_UNKNOWN;
                g_engine.onInputEvent(inputEvent);
            }
            break;
        }
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
    case WM_KEYDOWN:
    case WM_KEYUP:
        {
            if(win32.keyMap.find(wParam) != win32.keyMap.end()) {
                inputEvent.type = uMsg == WM_KEYDOWN ? EV_KEY_DOWN : EV_KEY_UP;
                inputEvent.ctrl = HIWORD(GetKeyState(VK_CONTROL)) == 0 ? false : true;
                inputEvent.alt = HIWORD(GetKeyState(VK_MENU)) == 0 ? false : true;
                inputEvent.shift = HIWORD(GetKeyState(VK_SHIFT)) == 0 ? false : true;
                inputEvent.keycode = win32.keyMap[wParam];
            }
            g_engine.onInputEvent(inputEvent);
            break;
        }
    case WM_LBUTTONDOWN:
        {
            inputEvent.type = EV_MOUSE_LDOWN;
            g_engine.onInputEvent(inputEvent);
            break;
        }
    case WM_LBUTTONUP:
        {
            inputEvent.type = EV_MOUSE_LUP;
            g_engine.onInputEvent(inputEvent);
            break;
        }
    case WM_MBUTTONDOWN:
        {
            inputEvent.type = EV_MOUSE_MDOWN;
            g_engine.onInputEvent(inputEvent);
            break;
        }
    case WM_MBUTTONUP:
        {
            inputEvent.type = EV_MOUSE_MUP;
            g_engine.onInputEvent(inputEvent);
            break;
        }
    case WM_RBUTTONDOWN:
        {
            inputEvent.type = EV_MOUSE_RDOWN;
            g_engine.onInputEvent(inputEvent);
            break;
        }
    case WM_RBUTTONUP:
        {
            inputEvent.type = EV_MOUSE_RUP;
            g_engine.onInputEvent(inputEvent);
            break;
        }
    case WM_MOUSEMOVE:
        {
            inputEvent.type = EV_MOUSE_MOVE;
            Point newMousePos(LOWORD(lParam), HIWORD(lParam));
            inputEvent.mouseMoved = newMousePos - inputEvent.mousePos;
            inputEvent.mousePos = newMousePos;
            g_engine.onInputEvent(inputEvent);
            break;
        }
    case WM_MOUSEWHEEL:
        {
            inputEvent.type = HIWORD(wParam) > 0 ? EV_MOUSE_WHEEL_UP : EV_MOUSE_WHEEL_DOWN;
            g_engine.onInputEvent(inputEvent);
            break;
        }
    case WM_MOVE:
        {
            lastX = win32.x;
            lastY = win32.y;
            win32.x = LOWORD(lParam);
            win32.y = HIWORD(lParam);
            break;
        }
    case WM_SIZE:
        {
            switch(wParam)
            {
            case SIZE_MAXIMIZED:
                win32.x = lastX;
                win32.y = lastY;
                win32.maximized = true;
                break;
            case SIZE_RESTORED:
                win32.maximized = false;
                break;
            }

            win32.visible = !(wParam == SIZE_MINIMIZED);

            if(!win32.maximized) {
                win32.width = LOWORD(lParam);
                win32.height = HIWORD(lParam);
            }

            g_engine.onResize(Size(LOWORD(lParam), HIWORD(lParam)));
            break;
        }
    default:
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
    return 0;
}
