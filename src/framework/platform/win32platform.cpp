/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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
#include "platformlistener.h"

#include <windows.h>
#include <dir.h>
#include <physfs.h>

#include <framework/thirdparty/apngloader.h>
#include <framework/core/resourcemanager.h>

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
    std::map<int, Fw::Key> keyMap;
    PlatformListener* listener;
    PlatformEvent platformEvent;
} win32;

Platform g_platform;

#ifdef HANDLE_EXCEPTIONS
#include <dbghelp.h>

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
  HANDLE hProcess,
  DWORD ProcessId,
  HANDLE hFile,
  MINIDUMP_TYPE DumpType,
  PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
  PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
  PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

LONG WINAPI crashHandler(EXCEPTION_POINTERS* exceptionPointers)
{
    logError("Application crashed");
    HMODULE hDbgHelp = LoadLibraryA("DBGHELP.DLL");
    char fileName[128];

    if(hDbgHelp) {
        MINIDUMPWRITEDUMP minuDumpWriteDump = (MINIDUMPWRITEDUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");

        SYSTEMTIME systemTime;
        GetSystemTime(&systemTime);
        snprintf(fileName, 128, "%s_%02u-%02u-%04u_%02u-%02u-%02u.mdmp", win32.appName.c_str(),
                systemTime.wDay, systemTime.wMonth, systemTime.wYear,
                systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

        HANDLE hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile) {
            MINIDUMP_EXCEPTION_INFORMATION exceptionInformation;
            exceptionInformation.ClientPointers = FALSE;
            exceptionInformation.ExceptionPointers = exceptionPointers;
            exceptionInformation.ThreadId = GetCurrentThreadId();

            HANDLE hProcess = GetCurrentProcess();
            DWORD ProcessId = GetProcessId(hProcess);
            MINIDUMP_TYPE flags = (MINIDUMP_TYPE)(MiniDumpNormal);

            BOOL dumpResult = minuDumpWriteDump(hProcess, ProcessId, hFile, flags, &exceptionInformation, NULL, NULL);
            if(!dumpResult){
                logError("Cannot generate minidump: ", GetLastError());
                CloseHandle(hFile);
                DeleteFileA(fileName);
                return EXCEPTION_CONTINUE_SEARCH;
            } else {
                logInfo("Crash minidump genarated on file ", fileName);
            }
        } else {
            logError("Cannot create dump file: ", GetLastError());
        }
    } else {
        logError("Cannot create dump file: dbghlp.dll not found");
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif

void Platform::init(PlatformListener* platformListener, const char *appName)
{
    // install crash handler
#ifdef HANDLE_EXCEPTIONS
    SetUnhandledExceptionFilter(crashHandler);
#endif

    // seend random numbers
    std::srand(std::time(NULL));

    win32.appName = appName;
    win32.instance = GetModuleHandle(NULL);
    win32.listener = platformListener;
    win32.keyMap[VK_ESCAPE] = Fw::KeyEscape;
    win32.keyMap[VK_TAB] = Fw::KeyTab;
    win32.keyMap[VK_RETURN] = Fw::KeyReturn;
    win32.keyMap[VK_BACK] = Fw::KeyBackspace;
    win32.keyMap[VK_SPACE] = Fw::KeySpace;

    win32.keyMap[VK_PRIOR] = Fw::KeyPageUp;
    win32.keyMap[VK_NEXT] = Fw::KeyPageDown;
    win32.keyMap[VK_HOME] = Fw::KeyHome;
    win32.keyMap[VK_END] = Fw::KeyEnd;
    win32.keyMap[VK_INSERT] = Fw::KeyInsert;
    win32.keyMap[VK_DELETE] = Fw::KeyDelete;

    win32.keyMap[VK_UP] = Fw::KeyUp;
    win32.keyMap[VK_DOWN] = Fw::KeyDown;
    win32.keyMap[VK_LEFT] = Fw::KeyLeft;
    win32.keyMap[VK_RIGHT] = Fw::KeyRight;

    win32.keyMap[VK_NUMLOCK] = Fw::KeyNumLock;
    win32.keyMap[VK_SCROLL] = Fw::KeyScrollLock;
    win32.keyMap[VK_CAPITAL] = Fw::KeyCapsLock;
    win32.keyMap[VK_SNAPSHOT] = Fw::KeyPrintScreen;
    win32.keyMap[VK_PAUSE] = Fw::KeyPause;

    win32.keyMap[VK_LCONTROL] = Fw::KeyCtrl;
    win32.keyMap[VK_RCONTROL] = Fw::KeyCtrl;
    win32.keyMap[VK_LSHIFT] = Fw::KeyShift;
    win32.keyMap[VK_RSHIFT] = Fw::KeyShift;
    win32.keyMap[VK_LMENU] = Fw::KeyAlt;
    win32.keyMap[VK_RMENU] = Fw::KeyAltGr;
    win32.keyMap[VK_LWIN] = Fw::KeyMeta;
    win32.keyMap[VK_RWIN] = Fw::KeyMeta;
    win32.keyMap[VK_MENU] = Fw::KeyMenu;

    // ascii characters
    /*
    win32.keyMap['!'] = Fw::KeyExclamation;
    win32.keyMap['"'] = Fw::KeyQuote;
    win32.keyMap['#'] = Fw::KeyNumberSign;
    win32.keyMap['$'] = Fw::KeyDollar;
    win32.keyMap['%'] = Fw::KeyPercent;
    win32.keyMap['$'] = Fw::KeyAmpersand;
    win32.keyMap['\''] = Fw::KeyApostrophe;
    win32.keyMap['('] = Fw::KeyLeftParen;
    win32.keyMap[')'] = Fw::KeyRightParen;
    win32.keyMap['*'] = Fw::KeyAsterisk;
    win32.keyMap['+'] = Fw::KeyPlus;
    win32.keyMap['.'] = Fw::KeyComma;
    win32.keyMap['-'] = Fw::KeyMinus;
    win32.keyMap['.'] = Fw::KeyPeriod;
    win32.keyMap['/'] = Fw::KeySlash;
    */

    win32.keyMap['0'] = Fw::Key0;
    win32.keyMap['1'] = Fw::Key1;
    win32.keyMap['2'] = Fw::Key2;
    win32.keyMap['3'] = Fw::Key3;
    win32.keyMap['4'] = Fw::Key4;
    win32.keyMap['5'] = Fw::Key5;
    win32.keyMap['6'] = Fw::Key6;
    win32.keyMap['7'] = Fw::Key7;
    win32.keyMap['8'] = Fw::Key8;
    win32.keyMap['9'] = Fw::Key9;

    /*
    win32.keyMap[':'] = Fw::KeyColon;
    win32.keyMap[';'] = Fw::KeySemicolon;
    win32.keyMap['<'] = Fw::KeyLess;
    win32.keyMap['='] = Fw::KeyEqual;
    win32.keyMap['>'] = Fw::KeyGreater;
    win32.keyMap['?'] = Fw::KeyQuestion;
    win32.keyMap['@'] = Fw::KeyAtSign;
    */

    win32.keyMap['A'] = Fw::KeyA;
    win32.keyMap['B'] = Fw::KeyB;
    win32.keyMap['C'] = Fw::KeyC;
    win32.keyMap['D'] = Fw::KeyD;
    win32.keyMap['E'] = Fw::KeyE;
    win32.keyMap['F'] = Fw::KeyF;
    win32.keyMap['G'] = Fw::KeyG;
    win32.keyMap['H'] = Fw::KeyH;
    win32.keyMap['I'] = Fw::KeyI;
    win32.keyMap['J'] = Fw::KeyJ;
    win32.keyMap['K'] = Fw::KeyK;
    win32.keyMap['L'] = Fw::KeyL;
    win32.keyMap['M'] = Fw::KeyM;
    win32.keyMap['N'] = Fw::KeyN;
    win32.keyMap['O'] = Fw::KeyO;
    win32.keyMap['P'] = Fw::KeyP;
    win32.keyMap['Q'] = Fw::KeyQ;
    win32.keyMap['R'] = Fw::KeyR;
    win32.keyMap['S'] = Fw::KeyS;
    win32.keyMap['T'] = Fw::KeyT;
    win32.keyMap['U'] = Fw::KeyU;
    win32.keyMap['V'] = Fw::KeyV;
    win32.keyMap['W'] = Fw::KeyW;
    win32.keyMap['X'] = Fw::KeyX;
    win32.keyMap['Y'] = Fw::KeyY;
    win32.keyMap['Z'] = Fw::KeyZ;

    /*
    win32.keyMap['['] = Fw::KeyLeftBracket;
    win32.keyMap['\\'] = Fw::KeyBackslash;
    win32.keyMap[']'] = Fw::KeyRightBracket;
    win32.keyMap['^'] = Fw::KeyCaret;
    win32.keyMap['_'] = Fw::KeyUnderscore;
    win32.keyMap['`'] = Fw::KeyGrave;
    win32.keyMap['{'] = Fw::KeyLeftCurly;
    win32.keyMap['|'] = Fw::KeyBar;
    win32.keyMap['}'] = Fw::KeyRightCurly;
    win32.keyMap['~'] = Fw::KeyTilde;
    */

    // keypad
    win32.keyMap[VK_ADD] = Fw::KeyPlus;
    win32.keyMap[VK_SUBTRACT] = Fw::KeyMinus;
    win32.keyMap[VK_DECIMAL] = Fw::KeyPeriod;
    win32.keyMap[VK_DIVIDE] = Fw::KeySlash;
    win32.keyMap[VK_MULTIPLY] = Fw::KeyAsterisk;
    win32.keyMap[VK_SEPARATOR] = Fw::KeyEnter;

    // keypad with numlock off
    win32.keyMap[VK_NUMPAD0] = Fw::KeyNumpad0;
    win32.keyMap[VK_NUMPAD1] = Fw::KeyNumpad1;
    win32.keyMap[VK_NUMPAD2] = Fw::KeyNumpad2;
    win32.keyMap[VK_NUMPAD3] = Fw::KeyNumpad3;
    win32.keyMap[VK_NUMPAD4] = Fw::KeyNumpad4;
    win32.keyMap[VK_NUMPAD5] = Fw::KeyNumpad5;
    win32.keyMap[VK_NUMPAD6] = Fw::KeyNumpad6;
    win32.keyMap[VK_NUMPAD7] = Fw::KeyNumpad7;
    win32.keyMap[VK_NUMPAD8] = Fw::KeyNumpad8;
    win32.keyMap[VK_NUMPAD9] = Fw::KeyNumpad9;

    win32.keyMap[VK_OEM_1] = Fw::KeySemicolon;
    win32.keyMap[VK_OEM_2] = Fw::KeySlash;
    win32.keyMap[VK_OEM_3] = Fw::KeyGrave;
    win32.keyMap[VK_OEM_4] = Fw::KeyLeftBracket;
    win32.keyMap[VK_OEM_5] = Fw::KeyBackslash;
    win32.keyMap[VK_OEM_6] = Fw::KeyRightBracket;
    win32.keyMap[VK_OEM_7] = Fw::KeyApostrophe;
    win32.keyMap[VK_OEM_MINUS] = Fw::KeyMinus;
    win32.keyMap[VK_OEM_PLUS] = Fw::KeyPlus;
    win32.keyMap[VK_OEM_COMMA] = Fw::KeyComma;
    win32.keyMap[VK_OEM_PERIOD] = Fw::KeyPeriod;

    win32.keyMap[VK_F1] = Fw::KeyF1;
    win32.keyMap[VK_F2] = Fw::KeyF2;
    win32.keyMap[VK_F3] = Fw::KeyF3;
    win32.keyMap[VK_F4] = Fw::KeyF4;
    win32.keyMap[VK_F5] = Fw::KeyF5;
    win32.keyMap[VK_F6] = Fw::KeyF6;
    win32.keyMap[VK_F7] = Fw::KeyF7;
    win32.keyMap[VK_F8] = Fw::KeyF8;
    win32.keyMap[VK_F9] = Fw::KeyF9;
    win32.keyMap[VK_F10] = Fw::KeyF10;
    win32.keyMap[VK_F11] = Fw::KeyF11;
    win32.keyMap[VK_F12] = Fw::KeyF12;

    // win class
    WNDCLASSA wc;
    wc.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;   // Redraw On Size, And Own DC For Window.
    wc.lpfnWndProc      = (WNDPROC)WndProc;         // WndProc Handles Messages
    wc.cbClsExtra       = 0;                    // No Extra Window Data
    wc.cbWndExtra       = 0;                    // No Extra Window Data
    wc.hInstance        = win32.instance;           // Set The Instance
    wc.hIcon            = LoadIcon(NULL, IDI_WINLOGO);      // Load The Default Icon
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);      // Load The Arrow Pointer
    wc.hbrBackground            = NULL;                 // No Background Required For GL
    wc.lpszMenuName     = NULL;                 // We Don't Want A Menu
    wc.lpszClassName            = win32.appName.c_str();                        // Set The Class Name

    if(!RegisterClassA(&wc))
        logFatal("Failed to register the window class.");

    // force first tick
    updateTicks();
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

void Platform::updateTicks()
{
    static ulong firstTick = 0;
    if(!firstTick)
        firstTick = GetTickCount();

    m_lastTicks = (uint32_t)(GetTickCount() - firstTick);
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
                                   NULL,                        // No Parent Window
                                   NULL,                        // No Menu
                                   win32.instance,                      // Instance
                                   NULL);

    if(!win32.window) {
        terminate();
        logFatal("Window creation error.");
        return false;
    }

    uint pixelFormat;
    static PIXELFORMATDESCRIPTOR pfd=               // pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR),              // Size Of This Pixel Format Descriptor
        1,                          // Version Number
        PFD_DRAW_TO_WINDOW |                    // Format Must Support Window
        PFD_SUPPORT_OPENGL |                    // Format Must Support OpenGL
        PFD_DOUBLEBUFFER,                   // Must Support Double Buffering
        PFD_TYPE_RGBA,                      // Request An RGBA Format
        32,                         // Select Our Color Depth
        0, 0, 0, 0, 0, 0,                   // Color Bits Ignored
        0,                          // No Alpha Buffer
        0,                          // Shift Bit Ignored
        0,                          // No Accumulation Buffer
        0, 0, 0, 0,                     // Accumulation Bits Ignored
        16,                         // 16Bit Z-Buffer (Depth Buffer)
        0,                          // No Stencil Buffer
        0,                          // No Auxiliary Buffer
        PFD_MAIN_PLANE,                     // Main Drawing Layer
        0,                          // Reserved
        0, 0, 0                         // Layer Masks Ignored
    };

    if(!(win32.hdc = GetDC(win32.window))) {
        terminate();
        logFatal("Could not create OpenGL device context.");
        return false;
    }

    if(!(pixelFormat = ChoosePixelFormat(win32.hdc, &pfd))) {
        terminate();
        logFatal("Could not find a suitable pixel format.");
        return false;
    }

    if(!SetPixelFormat(win32.hdc, pixelFormat, &pfd)) {
        terminate();
        logFatal("Could not set the pixel format.");
        return false;
    }

    if(!(win32.hrc = wglCreateContext(win32.hdc))) {
        terminate();
        logFatal("Could not create OpenGL rendering context.");
        return false;
    }

    if(!wglMakeCurrent(win32.hdc, win32.hrc)) {
        terminate();
        logFatal("Can't activate the GL rendering context.");
        return false;
    }

    return true;
}

void Platform::setWindowIcon(const std::string& pngIcon)
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

        SendMessage(win32.window, WM_SETICON, ICON_SMALL, (LPARAM)icon);
        SendMessage(win32.window, WM_SETICON, ICON_BIG, (LPARAM)icon);

        free_apng(&apng);
    } else
        logError("could not load app icon");
}

void Platform::destroyWindow()
{
    if(win32.hrc) {
        if(!wglMakeCurrent(NULL, NULL))
            logError("Release of dc and rc failed.");

        if(!wglDeleteContext(win32.hrc))
            logError("Release rendering context failed.");

        win32.hrc = NULL;
    }

    if(win32.hdc) {
        if(!ReleaseDC(win32.window, win32.hdc))
            logError("Release device context failed.");

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

void Platform::hideWindow()
{
    ShowWindow(win32.window, SW_HIDE);
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

    if(wglGetExtensionsStringARB) {
        const char *exts = wglGetExtensionsStringARB(win32.hdc);
        if(strstr(exts, ext))
            return true;
    }
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

Point Platform::getMouseCursorPos()
{
    return win32.platformEvent.mousePos;
}

void Platform::setVerticalSync(bool enable /*= true*/)
{
    typedef int (*glSwapIntervalProc)(int);
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
        logError("Couldn't create directory for saving configuration file. (%s)", sdir.str().c_str());
    return sdir.str();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static int lastX, lastY;
    PlatformEvent& platformEvent = win32.platformEvent;

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
                platformEvent.type = EventKeyDown;
                platformEvent.ctrl = HIWORD(GetKeyState(VK_CONTROL)) == 0 ? false : true;
                platformEvent.alt = HIWORD(GetKeyState(VK_MENU)) == 0 ? false : true;
                platformEvent.shift = HIWORD(GetKeyState(VK_SHIFT)) == 0 ? false : true;
                platformEvent.keychar = wParam;
                platformEvent.keycode = Fw::KeyUnknown;
                win32.listener->onPlatformEvent(platformEvent);
            }
            break;
        }
    case WM_CLOSE:
        {
            win32.listener->onClose();
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
                platformEvent.type = uMsg == WM_KEYDOWN ? EventKeyDown : EventKeyUp;
                platformEvent.ctrl = HIWORD(GetKeyState(VK_CONTROL)) == 0 ? false : true;
                platformEvent.alt = HIWORD(GetKeyState(VK_MENU)) == 0 ? false : true;
                platformEvent.shift = HIWORD(GetKeyState(VK_SHIFT)) == 0 ? false : true;
                platformEvent.keycode = win32.keyMap[wParam];
                platformEvent.keychar = 0;
            }
            win32.listener->onPlatformEvent(platformEvent);
            break;
        }
    case WM_LBUTTONDOWN:
        {
            platformEvent.type = EventMouseLeftButtonDown;
            win32.listener->onPlatformEvent(platformEvent);
            break;
        }
    case WM_LBUTTONUP:
        {
            platformEvent.type = EventMouseLeftButtonUp;
            win32.listener->onPlatformEvent(platformEvent);
            break;
        }
    case WM_MBUTTONDOWN:
        {
            platformEvent.type = EventMouseMiddleButtonDown;
            win32.listener->onPlatformEvent(platformEvent);
            break;
        }
    case WM_MBUTTONUP:
        {
            platformEvent.type = EventMouseMiddleButtonUp;
            win32.listener->onPlatformEvent(platformEvent);
            break;
        }
    case WM_RBUTTONDOWN:
        {
            platformEvent.type = EventMouseRightButtonDown;
            win32.listener->onPlatformEvent(platformEvent);
            break;
        }
    case WM_RBUTTONUP:
        {
            platformEvent.type = EventMouseRightButtonUp;
            win32.listener->onPlatformEvent(platformEvent);
            break;
        }
    case WM_MOUSEMOVE:
        {
            platformEvent.type = EventMouseMove;
            Point newMousePos(LOWORD(lParam), HIWORD(lParam));
            platformEvent.mouseMoved = newMousePos - platformEvent.mousePos;
            platformEvent.mousePos = newMousePos;
            win32.listener->onPlatformEvent(platformEvent);
            break;
        }
    case WM_MOUSEWHEEL:
        {
            platformEvent.type = HIWORD(wParam) > 0 ? EventMouseWheelUp : EventMouseWheelDown;
            win32.listener->onPlatformEvent(platformEvent);
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

            win32.listener->onResize(Size(LOWORD(lParam), HIWORD(lParam)));
            break;
        }
    default:
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
    return 0;
}

void Platform::displayFatalError(const std::string& message)
{
    MessageBoxA(NULL, message.c_str(), "Fatal Error", MB_OK | MB_ICONERROR);
}

