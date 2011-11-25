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

#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>

#include <physfs.h>

#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <framework/thirdparty/apngloader.h>
#include <framework/core/resourcemanager.h>

struct X11PlatformPrivate {
    Display *display;
    XVisualInfo *visual;
    GLXContext glxContext;
    XIM xim;
    XIC xic;
    Colormap colormap;
    Window window;
    Cursor cursor;
    Atom atomDeleteWindow;
    Atom atomClipboard;
    Atom atomTargets;
    Atom atomText;
    Atom atomCompoundText;
    Atom atomUTF8String;
    Atom atomWindowState;
    Atom atomWindowMaximizedVert;
    Atom atomWindowMaximizedHorz;
    bool visible;
    bool focused;
    bool maximizeOnFirstShow;
    std::string appName;
    int width;
    int height;
    int x;
    int y;
    int lastTicks;
    std::string clipboardText;
    std::map<int, Fw::Key> keyMap;
    PlatformListener* listener;
    PlatformEvent platformEvent;
} x11;

Platform g_platform;

#ifdef HANDLE_EXCEPTIONS

#include <csignal>
#include <execinfo.h>

#define MAX_BACKTRACE_DEPTH 128
#define DEMANGLE_BACKTRACE_SYMBOLS

void crashHandler(int signum, siginfo_t* info, void* secret)
{
    logError("Application crashed");

    ucontext_t context = *(ucontext_t*)secret;
    time_t tnow;
    char fileName[128];
    time(&tnow);
    tm *ts = localtime(&tnow);
    strftime(fileName, 128, (x11.appName + "-crash_-%d-%m-%Y_%H:%M:%S.txt").c_str(), ts);

    std::stringstream ss;
    ss.flags(std::ios::hex | std::ios::showbase);
#if __WORDSIZE == 32
    ss <<
    ss << "  at eip = " << context.uc_mcontext.gregs[REG_EIP] << std::endl;
    ss << "     eax = " << context.uc_mcontext.gregs[REG_EAX] << std::endl;
    ss << "     ebx = " << context.uc_mcontext.gregs[REG_EBX] << std::endl;
    ss << "     ecx = " << context.uc_mcontext.gregs[REG_ECX] << std::endl;
    ss << "     edx = " << context.uc_mcontext.gregs[REG_EDX] << std::endl;
    ss << "     esi = " << context.uc_mcontext.gregs[REG_ESI] << std::endl;
    ss << "     edi = " << context.uc_mcontext.gregs[REG_EDI] << std::endl;
    ss << "     ebp = " << context.uc_mcontext.gregs[REG_EBP] << std::endl;
    ss << "     esp = " << context.uc_mcontext.gregs[REG_ESP] << std::endl;
    ss << "     efl = " << context.uc_mcontext.gregs[REG_EFL] << std::endl;
#else // 64-bit
    ss << "  at rip = " << context.uc_mcontext.gregs[REG_RIP] << std::endl;
    ss << "     rax = " << context.uc_mcontext.gregs[REG_RAX] << std::endl;
    ss << "     rbx = " << context.uc_mcontext.gregs[REG_RBX] << std::endl;
    ss << "     rcx = " << context.uc_mcontext.gregs[REG_RCX] << std::endl;
    ss << "     rdx = " << context.uc_mcontext.gregs[REG_RDX] << std::endl;
    ss << "     rsi = " << context.uc_mcontext.gregs[REG_RSI] << std::endl;
    ss << "     rdi = " << context.uc_mcontext.gregs[REG_RDI] << std::endl;
    ss << "     rbp = " << context.uc_mcontext.gregs[REG_RBP] << std::endl;
    ss << "     rsp = " << context.uc_mcontext.gregs[REG_RSP] << std::endl;
    ss << "     efl = " << context.uc_mcontext.gregs[REG_EFL] << std::endl;
#endif
    ss << std::endl;
    ss.flags(std::ios::dec);
    ss << "  backtrace:" << std::endl;

    void* buffer[MAX_BACKTRACE_DEPTH];
    int numLevels = backtrace(buffer, MAX_BACKTRACE_DEPTH);
    char **tracebackBuffer = backtrace_symbols(buffer, numLevels);
    if(tracebackBuffer) {
        for(int i = 2; i < numLevels; i++) {
            std::string line = tracebackBuffer[i];
            if(line.find("__libc_start_main") != std::string::npos)
                break;
#ifdef DEMANGLE_BACKTRACE_SYMBOLS
            std::size_t demanglePos = line.find("(_Z");
            if(demanglePos != std::string::npos) {
                demanglePos++;
                int len = std::min(line.find_first_of("+", demanglePos), line.find_first_of(")", demanglePos)) - demanglePos;
                std::string funcName = line.substr(demanglePos, len);
                line.replace(demanglePos, len, Fw::demangleName(funcName.c_str()));
            }
#endif
            ss << "    " << i-1 << ": " << line << std::endl;
        }
        free(tracebackBuffer);
    }

    logInfo(ss.str());

    std::ofstream out(fileName);
    out << ss.str();
    out.close();
    logInfo("Crash report saved to file ", fileName);

    signal(SIGILL, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
}
#endif

void Platform::init(PlatformListener* platformListener, const char *appName)
{
#ifdef HANDLE_EXCEPTIONS
    struct sigaction sa;
    sa.sa_sigaction = &crashHandler;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    sigaction(SIGILL, &sa, NULL);   // illegal instruction
    sigaction(SIGSEGV, &sa, NULL);  // segmentation fault
    sigaction(SIGFPE, &sa, NULL);   // floating-point exception
#endif

    // seend random numbers
    srand(time(NULL));

    x11.appName = appName;
    x11.display = NULL;
    x11.visual = NULL;
    x11.glxContext = NULL;
    x11.xim = NULL;
    x11.xic = NULL;
    x11.colormap = None;
    x11.window = None;
    x11.cursor = None;
    x11.visible = false;
    x11.focused = false;
    x11.width = 0;
    x11.height = 0;
    x11.maximizeOnFirstShow = false;
    x11.listener = platformListener;

    x11.keyMap[XK_Escape] = Fw::KeyEscape;
    x11.keyMap[XK_Tab] = Fw::KeyTab;
    x11.keyMap[XK_Return] = Fw::KeyReturn;
    x11.keyMap[XK_BackSpace] = Fw::KeyBackspace;

    x11.keyMap[XK_Page_Up] = Fw::KeyPageUp;
    x11.keyMap[XK_Page_Down] = Fw::KeyPageDown;
    x11.keyMap[XK_Home] = Fw::KeyHome;
    x11.keyMap[XK_End] = Fw::KeyEnd;
    x11.keyMap[XK_Insert] = Fw::KeyInsert;
    x11.keyMap[XK_Delete] = Fw::KeyDelete;

    x11.keyMap[XK_Up] = Fw::KeyUp;
    x11.keyMap[XK_Down] = Fw::KeyDown;
    x11.keyMap[XK_Left] = Fw::KeyLeft;
    x11.keyMap[XK_Right] = Fw::KeyRight;

    x11.keyMap[XK_Num_Lock] = Fw::KeyNumLock;
    x11.keyMap[XK_Scroll_Lock] = Fw::KeyScrollLock;
    x11.keyMap[XK_Caps_Lock] = Fw::KeyCapsLock;
    x11.keyMap[XK_Print] = Fw::KeyPrintScreen;
    x11.keyMap[XK_Pause] = Fw::KeyPause;

    x11.keyMap[XK_Control_L] = Fw::KeyCtrl;
    x11.keyMap[XK_Control_R] = Fw::KeyCtrl;
    x11.keyMap[XK_Shift_R] = Fw::KeyShift;
    x11.keyMap[XK_Shift_L] = Fw::KeyShift;
    x11.keyMap[XK_Alt_R] = Fw::KeyAlt;
    x11.keyMap[XK_Alt_L] = Fw::KeyAltGr;
    x11.keyMap[XK_Meta_L] = Fw::KeyMeta;
    x11.keyMap[XK_Meta_R] = Fw::KeyMeta;
    x11.keyMap[XK_Menu] = Fw::KeyMenu;

    // ascii characters
    x11.keyMap[XK_space] = Fw::KeySpace;
    x11.keyMap[XK_exclam] = Fw::KeyExclamation;
    x11.keyMap[XK_quotedbl] = Fw::KeyQuote;
    x11.keyMap[XK_numbersign] = Fw::KeyNumberSign;
    x11.keyMap[XK_dollar] = Fw::KeyDollar;
    x11.keyMap[XK_percent] = Fw::KeyPercent;
    x11.keyMap[XK_ampersand] = Fw::KeyAmpersand;
    x11.keyMap[XK_apostrophe] = Fw::KeyApostrophe;
    x11.keyMap[XK_parenleft] = Fw::KeyLeftParen;
    x11.keyMap[XK_parenright] = Fw::KeyRightParen;
    x11.keyMap[XK_asterisk] = Fw::KeyAsterisk;
    x11.keyMap[XK_plus] = Fw::KeyPlus;
    x11.keyMap[XK_comma] = Fw::KeyComma;
    x11.keyMap[XK_minus] = Fw::KeyMinus;
    x11.keyMap[XK_period] = Fw::KeyPeriod;
    x11.keyMap[XK_slash] = Fw::KeySlash;
    x11.keyMap[XK_1] = Fw::Key1;
    x11.keyMap[XK_2] = Fw::Key2;
    x11.keyMap[XK_3] = Fw::Key3;
    x11.keyMap[XK_4] = Fw::Key4;
    x11.keyMap[XK_5] = Fw::Key5;
    x11.keyMap[XK_6] = Fw::Key6;
    x11.keyMap[XK_7] = Fw::Key7;
    x11.keyMap[XK_8] = Fw::Key8;
    x11.keyMap[XK_9] = Fw::Key9;
    x11.keyMap[XK_0] = Fw::Key0;
    x11.keyMap[XK_colon] = Fw::KeyColon;
    x11.keyMap[XK_semicolon] = Fw::KeySemicolon;
    x11.keyMap[XK_less] = Fw::KeyLess;
    x11.keyMap[XK_equal] = Fw::KeyEqual;
    x11.keyMap[XK_greater] = Fw::KeyGreater;
    x11.keyMap[XK_question] = Fw::KeyQuestion;
    x11.keyMap[XK_at] = Fw::KeyAtSign;
    x11.keyMap[XK_a] = Fw::KeyA;
    x11.keyMap[XK_b] = Fw::KeyB;
    x11.keyMap[XK_c] = Fw::KeyC;
    x11.keyMap[XK_d] = Fw::KeyD;
    x11.keyMap[XK_e] = Fw::KeyE;
    x11.keyMap[XK_f] = Fw::KeyF;
    x11.keyMap[XK_g] = Fw::KeyG;
    x11.keyMap[XK_h] = Fw::KeyH;
    x11.keyMap[XK_i] = Fw::KeyI;
    x11.keyMap[XK_j] = Fw::KeyJ;
    x11.keyMap[XK_k] = Fw::KeyK;
    x11.keyMap[XK_l] = Fw::KeyL;
    x11.keyMap[XK_m] = Fw::KeyM;
    x11.keyMap[XK_n] = Fw::KeyN;
    x11.keyMap[XK_o] = Fw::KeyO;
    x11.keyMap[XK_p] = Fw::KeyP;
    x11.keyMap[XK_q] = Fw::KeyQ;
    x11.keyMap[XK_r] = Fw::KeyR;
    x11.keyMap[XK_s] = Fw::KeyS;
    x11.keyMap[XK_t] = Fw::KeyT;
    x11.keyMap[XK_u] = Fw::KeyU;
    x11.keyMap[XK_v] = Fw::KeyV;
    x11.keyMap[XK_w] = Fw::KeyW;
    x11.keyMap[XK_x] = Fw::KeyX;
    x11.keyMap[XK_y] = Fw::KeyY;
    x11.keyMap[XK_z] = Fw::KeyZ;
    x11.keyMap[XK_bracketleft] = Fw::KeyLeftBracket;
    x11.keyMap[XK_backslash] = Fw::KeyBackslash;
    x11.keyMap[XK_bracketright] = Fw::KeyRightBracket;
    x11.keyMap[XK_asciicircum] = Fw::KeyCaret;
    x11.keyMap[XK_underscore] = Fw::KeyUnderscore;
    x11.keyMap[XK_grave] = Fw::KeyGrave;
    x11.keyMap[XK_braceleft] = Fw::KeyLeftCurly;
    x11.keyMap[XK_bar] = Fw::KeyBar;
    x11.keyMap[XK_braceright] = Fw::KeyRightCurly;
    x11.keyMap[XK_asciitilde] = Fw::KeyTilde;

    // keypad
    x11.keyMap[XK_KP_Add] = Fw::KeyPlus;
    x11.keyMap[XK_KP_Subtract] = Fw::KeyMinus;
    x11.keyMap[XK_KP_Decimal] = Fw::KeyPeriod;
    x11.keyMap[XK_KP_Divide] = Fw::KeySlash;
    x11.keyMap[XK_KP_Multiply] = Fw::KeyAsterisk;
    x11.keyMap[XK_KP_Enter] = Fw::KeyEnter;

    // keypad with numlock off
    x11.keyMap[XK_KP_Insert] = Fw::KeyNumpad0;
    x11.keyMap[XK_KP_End] = Fw::KeyNumpad1;
    x11.keyMap[XK_KP_Down] = Fw::KeyNumpad2;
    x11.keyMap[XK_KP_Page_Down] = Fw::KeyNumpad3;
    x11.keyMap[XK_KP_Left] = Fw::KeyNumpad4;
    x11.keyMap[XK_KP_Begin] = Fw::KeyNumpad5;
    x11.keyMap[XK_KP_Right] = Fw::KeyNumpad6;
    x11.keyMap[XK_KP_Home] = Fw::KeyNumpad7;
    x11.keyMap[XK_KP_Up] = Fw::KeyNumpad8;
    x11.keyMap[XK_KP_Page_Up] = Fw::KeyNumpad9;
    x11.keyMap[XK_KP_Delete] = Fw::KeyDelete;

    // keypad with numlock on
    x11.keyMap[XK_KP_0] = Fw::Key0;
    x11.keyMap[XK_KP_1] = Fw::Key1;
    x11.keyMap[XK_KP_2] = Fw::Key2;
    x11.keyMap[XK_KP_3] = Fw::Key3;
    x11.keyMap[XK_KP_4] = Fw::Key4;
    x11.keyMap[XK_KP_5] = Fw::Key5;
    x11.keyMap[XK_KP_6] = Fw::Key6;
    x11.keyMap[XK_KP_7] = Fw::Key7;
    x11.keyMap[XK_KP_8] = Fw::Key8;
    x11.keyMap[XK_KP_9] = Fw::Key9;

    x11.keyMap[XK_F1] = Fw::KeyF1;
    x11.keyMap[XK_F2] = Fw::KeyF2;
    x11.keyMap[XK_F3] = Fw::KeyF3;
    x11.keyMap[XK_F4] = Fw::KeyF4;
    x11.keyMap[XK_F5] = Fw::KeyF5;
    x11.keyMap[XK_F6] = Fw::KeyF6;
    x11.keyMap[XK_F7] = Fw::KeyF7;
    x11.keyMap[XK_F8] = Fw::KeyF8;
    x11.keyMap[XK_F9] = Fw::KeyF9;
    x11.keyMap[XK_F10] = Fw::KeyF10;
    x11.keyMap[XK_F11] = Fw::KeyF11;
    x11.keyMap[XK_F12] = Fw::KeyF12;

    // try to set a latin1 locales otherwise fallback to standard C locale
    static char locales[][32] = { "en_US.iso88591", "iso88591", "en_US", "C" };
    for(int i=0;i<4;++i) {
        if(setlocale(LC_ALL, locales[i]))
            break;
    }

    // open display
    x11.display = XOpenDisplay(0);
    if(!x11.display)
        logFatal("Failed to open X display");

    // check if GLX is supported on this display
    if(!glXQueryExtension(x11.display, 0, 0))
        logFatal("GLX not supported");

    // retrieve GLX version
    int glxMajor;
    int glxMinor;
    if(!glXQueryVersion(x11.display, &glxMajor, &glxMinor))
        logFatal("Unable to query GLX version");
    logInfo("GLX version ",glxMajor,".",glxMinor);

    // clipboard related atoms
    x11.atomClipboard = XInternAtom(x11.display, "CLIPBOARD", False);
    x11.atomTargets = XInternAtom(x11.display, "TARGETS", False);
    x11.atomUTF8String = XInternAtom(x11.display, "UTF8_STRING", False);
    x11.atomText = XInternAtom(x11.display, "TEXT", False);
    x11.atomCompoundText = XInternAtom(x11.display, "COMPOUND_TEXT", False);
    x11.atomWindowState = XInternAtom(x11.display, "_NET_WM_STATE", False);
    x11.atomWindowMaximizedVert = XInternAtom(x11.display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
    x11.atomWindowMaximizedHorz = XInternAtom(x11.display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);

    // force first tick
    updateTicks();
}

void Platform::terminate()
{
    if(x11.window) {
        destroyWindow();
        x11.window = None;
    }

    // close display
    if(x11.display) {
        XCloseDisplay(x11.display);
        x11.display = NULL;
    }
}

void Platform::poll()
{
    XEvent event, peekevent;
    PlatformEvent& platformEvent = x11.platformEvent;
    static int oldWidth = -1;
    static int oldHeight = -1;
    while(XPending(x11.display) > 0) {
        XNextEvent(x11.display, &event);

        // call filter because xim will discard KeyPress events when keys still composing
        if(XFilterEvent(&event, x11.window))
            continue;

        // discard events of repeated key releases
        if(event.type == KeyRelease && XPending(x11.display)) {
            XPeekEvent(x11.display, &peekevent);
            if((peekevent.type == KeyPress) &&
              (peekevent.xkey.keycode == event.xkey.keycode) &&
              ((peekevent.xkey.time-event.xkey.time) < 2))
                continue;
        }

        switch(event.type) {
            case Expose:
                // needs redraw
                break;
            case ConfigureNotify:
                // window resize
                if(oldWidth != event.xconfigure.width || oldHeight != event.xconfigure.height) {
                    x11.listener->onResize(Size(event.xconfigure.width, event.xconfigure.height));
                    oldWidth = event.xconfigure.width;
                    oldHeight = event.xconfigure.height;
                }

                if(!isWindowMaximized()) {
                    x11.width = event.xconfigure.width;
                    x11.height = event.xconfigure.height;

                    // hack to fix x11 windows move gaps
                    static int gap_x = -1, gap_y = -1;
                    if(gap_x == -1 && gap_y == -1) {
                        gap_x = event.xconfigure.x;
                        gap_y = event.xconfigure.y;
                    }
                    x11.x = event.xconfigure.x - gap_x;
                    x11.y = event.xconfigure.y - gap_y;
                }
                break;

            case KeyPress:
            case KeyRelease: {
                KeySym keysym;
                char buf[32];
                int len;

                platformEvent.ctrl = (event.xkey.state & ControlMask);
                platformEvent.shift = (event.xkey.state & ShiftMask);
                platformEvent.alt = (event.xkey.state & Mod1Mask);
                platformEvent.keychar = 0;

                // fire enter text event
                if(event.type == KeyPress && !platformEvent.ctrl && !platformEvent.alt) {
                    if(x11.xic) { // with xim we can get latin1 input correctly
                        Status status;
                        len = XmbLookupString(x11.xic, &event.xkey, buf, sizeof(buf), &keysym, &status);
                    } else { // otherwise use XLookupString, but it doesn't work right with dead keys often
                        static XComposeStatus compose = {NULL, 0};
                        len = XLookupString(&event.xkey, buf, sizeof(buf), &keysym, &compose);
                    }

                    if(len > 0 &&
                       // these keys produces characters that we don't want to capture
                       keysym != XK_BackSpace &&
                       keysym != XK_Return &&
                       keysym != XK_Delete &&
                       keysym != XK_Escape &&
                       (uchar)(buf[0]) >= 32
                    ) {
                        //logDebug("char: ", buf[0], " code: ", (uint)buf[0]);
                        platformEvent.keychar = buf[0];
                    }
                } else {
                    //event.xkey.state &= ~(ShiftMask | LockMask);
                    len = XLookupString(&event.xkey, buf, sizeof(buf), &keysym, 0);

                    if(len > 0 && (uchar)platformEvent.keychar >= 32)
                        platformEvent.keychar = (len > 0) ? buf[0] : 0;
                }

                if(x11.keyMap.find(keysym) != x11.keyMap.end())
                    platformEvent.keycode = x11.keyMap[keysym];
                else
                    platformEvent.keycode = Fw::KeyUnknown;

                platformEvent.keycode = x11.keyMap[keysym];
                platformEvent.type = (event.type == KeyPress) ? EventKeyDown : EventKeyUp;

                if(platformEvent.keycode != Fw::KeyUnknown || platformEvent.keychar != 0)
                    x11.listener->onPlatformEvent(platformEvent);
                break;
            }
            case ButtonPress:
            case ButtonRelease:
                switch(event.xbutton.button) {
                    case Button1:
                        platformEvent.type = (event.type == ButtonPress) ? EventMouseLeftButtonDown : EventMouseLeftButtonUp;
                        break;
                    case Button3:
                        platformEvent.type = (event.type == ButtonPress) ? EventMouseRightButtonDown : EventMouseRightButtonUp;
                        break;
                    case Button2:
                        platformEvent.type = (event.type == ButtonPress) ? EventMouseMiddleButtonDown : EventMouseMiddleButtonUp;
                        break;
                    case Button4:
                        platformEvent.type = EventMouseWheelUp;
                        break;
                    case Button5:
                        platformEvent.type = EventMouseWheelDown;
                        break;
                }
                x11.listener->onPlatformEvent(platformEvent);
                break;

            case MotionNotify:
            {
                platformEvent.type = EventMouseMove;
                Point newMousePos(event.xbutton.x, event.xbutton.y);
                platformEvent.mouseMoved = newMousePos - platformEvent.mousePos;
                platformEvent.mousePos = newMousePos;
                x11.listener->onPlatformEvent(platformEvent);
                break;
            }

            case MapNotify:
                x11.visible = true;
                break;

            case UnmapNotify:
                x11.visible = false;
                break;

            case FocusIn:
                x11.focused = true;
                break;

            case FocusOut:
                x11.focused = false;
                break;

            // clipboard data request
            case SelectionRequest:
            {
                XEvent respond;
                XSelectionRequestEvent *req = &(event.xselectionrequest);

                if(req->target == x11.atomTargets ) {
                    Atom typeList[] = {x11.atomText, x11.atomCompoundText, x11.atomUTF8String, XA_STRING};

                    XChangeProperty(x11.display, req->requestor,
                                    req->property, req->target,
                                    8, PropModeReplace,
                                    (uchar *) &typeList,
                                    sizeof(typeList));
                    respond.xselection.property = req->property;
                } else {
                    XChangeProperty(x11.display,
                                    req->requestor,
                                    req->property, req->target,
                                    8,
                                    PropModeReplace,
                                    (uchar*) x11.clipboardText.c_str(),
                                    x11.clipboardText.size());
                    respond.xselection.property = req->property;
                }

                respond.xselection.type = SelectionNotify;
                respond.xselection.display = req->display;
                respond.xselection.requestor = req->requestor;
                respond.xselection.selection = req->selection;
                respond.xselection.target = req->target;
                respond.xselection.time = req->time;
                XSendEvent(x11.display, req->requestor, 0, 0, &respond);
                XFlush(x11.display);
                break;
            }

            case ClientMessage:
            {
                if((Atom)event.xclient.data.l[0] == x11.atomDeleteWindow)
                    x11.listener->onClose();
                break;
            }
        }
    }
}

void Platform::updateTicks()
{
    static timeval tv;
    static ulong firstTick = 0;

    gettimeofday(&tv, 0);
    if(!firstTick)
        firstTick = tv.tv_sec;

    m_lastTicks = ((tv.tv_sec - firstTick) * 1000) + (tv.tv_usec / 1000);
}

void Platform::sleep(ulong miliseconds)
{
    timespec tv;
    tv.tv_sec  = miliseconds / 1000;
    tv.tv_nsec = (miliseconds % 1000) * 1000000;
    nanosleep(&tv, NULL);
}

bool Platform::createWindow(int x, int y, int width, int height, int minWidth, int minHeight, bool maximized)
{
    static int attrList[] = {
        GLX_USE_GL,
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        None
    };

    // choose OpenGL, RGBA, double buffered, visual
    x11.visual = glXChooseVisual(x11.display, DefaultScreen(x11.display), attrList);
    if(!x11.visual)
        logFatal("RGBA/Double buffered visual not supported");

    // create GLX context
    x11.glxContext = glXCreateContext(x11.display, x11.visual, NULL, False);
    if(!x11.glxContext)
        logFatal("Unable to create GLX context");

    logInfo("Direct rendering: ", glXIsDirect(x11.display, x11.glxContext) ? "Yes" : "No");

    // color map
    x11.colormap  = XCreateColormap(x11.display,
                                    RootWindow(x11.display, x11.visual->screen),
                                    x11.visual->visual,
                                    AllocNone);

    // setup window type
    XSetWindowAttributes attr;
    attr.colormap = x11.colormap;
    attr.border_pixel = 0;
    attr.backing_store = Always;
    attr.background_pixel = 0;
    attr.event_mask = KeyPressMask | KeyReleaseMask |
                         ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                         ExposureMask | VisibilityChangeMask |
                         StructureNotifyMask | FocusChangeMask;
    unsigned int mask = CWBorderPixel | CWColormap | CWEventMask | CWBackingStore | CWBackPixel;

    x11.x = x;
    x11.y = y;

    // create the window
    x11.window = XCreateWindow(x11.display,
                             RootWindow(x11.display, x11.visual->screen),
                             0, 0,
                             width, height,
                             0,
                             x11.visual->depth,
                             InputOutput,
                             x11.visual->visual,
                             mask,
                             &attr);
    if(!x11.window)
        logFatal("XCreateWindow failed");

    //  create input context (to have better key input handling)
    if(XSupportsLocale()) {
        XSetLocaleModifiers("");
        x11.xim = XOpenIM(x11.display, NULL, NULL, NULL);
        if(x11.xim) {
            x11.xic = XCreateIC(x11.xim,
                                    XNInputStyle,
                                    XIMPreeditNothing | XIMStatusNothing,
                                    XNClientWindow, x11.window, NULL);
            if(!x11.xic)
                logError("Unable to create the input context");
        } else
            logError("Failed to open an input method");
    } else
        logError("X11 does not support the current locale");

    if(!x11.xic)
        logWarning("Input of special keys maybe messed up because we couldn't create an input context");

    // set window minimum size
    XSizeHints xsizehints;
    xsizehints.flags = PMinSize;
    xsizehints.min_width = minWidth;
    xsizehints.min_height= minHeight;
    XSetWMSizeHints(x11.display, x11.window, &xsizehints, XA_WM_NORMAL_HINTS);

    // handle delete window event
    x11.atomDeleteWindow = XInternAtom(x11.display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(x11.display, x11.window, &x11.atomDeleteWindow , 1);

    // connect the GLX-context to the window
    if(!glXMakeCurrent(x11.display, x11.window, x11.glxContext))
        logFatal("glXMakeCurrent failed");

    x11.width = width;
    x11.height = height;
    x11.maximizeOnFirstShow = maximized;

    // call first onResize
    x11.listener->onResize(Size(width, height));

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
            return;
        }

        int n = apng.width * apng.height;
        std::vector<unsigned long int> iconData(n + 2);
        iconData[0] = apng.width;
        iconData[1] = apng.height;
        for(int i=0; i < n;++i) {
            uint8 *pixel = (uint8*)&iconData[2 + i];
            pixel[2] = *(apng.pdata + (i * 4) + 0);
            pixel[1] = *(apng.pdata + (i * 4) + 1);
            pixel[0] = *(apng.pdata + (i * 4) + 2);
            pixel[3] = *(apng.pdata + (i * 4) + 3);
        }

        Atom property = XInternAtom(x11.display, "_NET_WM_ICON", 0);
        if(!XChangeProperty(x11.display, x11.window, property, XA_CARDINAL, 32, PropModeReplace, (const unsigned char*)&iconData[0], iconData.size()))
            logError("could not set app icon");

        free_apng(&apng);
    } else
        logError("could not load app icon");
}

void Platform::destroyWindow()
{
    if(x11.glxContext) {
        glXMakeCurrent(x11.display, None, NULL);
        glXDestroyContext(x11.display, x11.glxContext);
        x11.glxContext = NULL;
    }

    if(x11.visual) {
        XFree(x11.visual);
        x11.visual = NULL;
    }

    if(x11.colormap != None) {
        XFreeColormap(x11.display, x11.colormap);
        x11.colormap = 0;
    }

    if(x11.window != None) {
        XDestroyWindow(x11.display, x11.window);
        x11.window = None;
    }

    if(x11.xic) {
        XDestroyIC(x11.xic);
        x11.xic = NULL;
    }

    if(x11.xim) {
        XCloseIM(x11.xim);
        x11.xim = NULL;
    }
}

void Platform::showWindow()
{
    XMapWindow(x11.display, x11.window);

    static bool firstShow = true;
    if(firstShow) {
        // move window
        XMoveWindow(x11.display, x11.window, x11.x, x11.y);

        // set window maximized if needed
        if(x11.maximizeOnFirstShow) {
            XEvent e;
            memset(&e, 0, sizeof(XEvent));
            e.xany.type = ClientMessage;
            e.xclient.message_type = x11.atomWindowState;
            e.xclient.format = 32;
            e.xclient.window = x11.window;
            e.xclient.data.l[0] = 1l;
            e.xclient.data.l[1] = x11.atomWindowMaximizedVert;
            e.xclient.data.l[2] = x11.atomWindowMaximizedHorz;
            e.xclient.data.l[3] = 0l;

            XSendEvent(x11.display, RootWindow(x11.display, x11.visual->screen), 0,
                    SubstructureNotifyMask | SubstructureRedirectMask, &e);
        }
        firstShow = false;
    }
}

void Platform::hideWindow()
{
    XUnmapWindow(x11.display, x11.window);
}

void Platform::setWindowTitle(const char *title)
{
    XStoreName(x11.display, x11.window, title);
    XSetIconName(x11.display, x11.window, title);
}

void *Platform::getExtensionProcAddress(const char *ext)
{
    return (void*)glXGetProcAddressARB((const GLubyte*)ext);
}

bool Platform::isExtensionSupported(const char *ext)
{
    const char *exts = glXQueryExtensionsString(x11.display, DefaultScreen(x11.display));
    if(strstr(exts, ext))
        return true;
    return false;
}

const char *Platform::getClipboardText()
{
    Window ownerWindow = XGetSelectionOwner(x11.display, x11.atomClipboard);
    if(ownerWindow ==  x11.window)
        return x11.clipboardText.c_str();

    std::string clipboard = "";
    if(ownerWindow != None) {
        XConvertSelection(x11.display, x11.atomClipboard, XA_STRING, 0, ownerWindow, CurrentTime);
        XFlush(x11.display);

        // hack to wait SelectioNotify event, otherwise we will get wrong clipboard pastes
        sleep(100);

        // check for data
        Atom type;
        int format;
        ulong numItems, bytesLeft, dummy;
        uchar *data;
        XGetWindowProperty(x11.display, ownerWindow,
                            XA_STRING,
                            0, 0, 0,
                            AnyPropertyType,
                            &type,
                            &format,
                            &numItems,
                            &bytesLeft,
                            &data);
        if(bytesLeft > 0)  {
            // get the data get
            int result = XGetWindowProperty(x11.display, ownerWindow,
                                            XA_STRING,
                                            0,
                                            bytesLeft,
                                            0,
                                            AnyPropertyType,
                                            &type,
                                            &format,
                                            &numItems,
                                            &dummy,
                                            &data);
            if(result == Success)
                clipboard = (const char*)data;
            XFree(data);
        }
    }
    return clipboard.c_str();
}

void Platform::setClipboardText(const char *text)
{
    x11.clipboardText = text;
    XSetSelectionOwner(x11.display, x11.atomClipboard, x11.window, CurrentTime);
    XFlush(x11.display);
}

void Platform::hideMouseCursor()
{
    if(x11.cursor == None) {
        char bm[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        Pixmap pix = XCreateBitmapFromData(x11.display, x11.window, bm, 8, 8);
        XColor black;
        memset(&black, 0, sizeof(XColor));
        black.flags = DoRed | DoGreen | DoBlue;
        x11.cursor = XCreatePixmapCursor(x11.display, pix, pix, &black, &black, 0, 0);
        XFreePixmap(x11.display, pix);
    }
    XDefineCursor(x11.display, x11.window, x11.cursor);
}

void Platform::showMouseCursor()
{
    XUndefineCursor(x11.display, x11.window);
    if(x11.cursor != None) {
        XFreeCursor(x11.display, x11.cursor);
        x11.cursor = None;
    }
}

Point Platform::getMouseCursorPos()
{
    return x11.platformEvent.mousePos;
}

void Platform::setVerticalSync(bool enable)
{
    typedef GLint (*glSwapIntervalProc)(GLint);
    glSwapIntervalProc glSwapInterval = NULL;

    if(isExtensionSupported("GLX_MESA_swap_control"))
        glSwapInterval = (glSwapIntervalProc)getExtensionProcAddress("glXSwapIntervalMESA");
    else if(isExtensionSupported("GLX_SGI_swap_control"))
        glSwapInterval = (glSwapIntervalProc)getExtensionProcAddress("glXSwapIntervalSGI");

    if(glSwapInterval)
        glSwapInterval(enable ? 1 : 0);
}

void Platform::swapBuffers()
{
    //glFlush();
    //glFinish();
    glXSwapBuffers(x11.display, x11.window);
}

bool Platform::isWindowFocused()
{
    return x11.focused;
}

bool Platform::isWindowVisible()
{
    return x11.visible;
}

int Platform::getWindowX()
{
    return x11.x;
}

int Platform::getWindowY()
{
    return x11.y;
}

int Platform::getWindowWidth()
{
    return x11.width;
}

int Platform::getWindowHeight()
{
    return x11.height;
}

int Platform::getDisplayWidth()
{
    return XDisplayWidth(x11.display, DefaultScreen(x11.display));
}

int Platform::getDisplayHeight()
{
    return XDisplayHeight(x11.display, DefaultScreen(x11.display));
}

bool Platform::isWindowMaximized()
{
    bool ret = false;
    Atom actualType;
    int actualFormat;
    ulong i, numItems, bytesAfter;
    uchar *propertyValue = NULL;
    long maxLength = 1024;

    if(XGetWindowProperty(x11.display, x11.window, x11.atomWindowState,
                            0l, maxLength, False, XA_ATOM, &actualType,
                            &actualFormat, &numItems, &bytesAfter,
                            &propertyValue) == Success) {
        Atom *atoms = (Atom *)propertyValue;
        int maximized = 0;

        for(i=0; i<numItems; ++i) {
            if(atoms[i] == x11.atomWindowMaximizedVert)
                maximized |= 1;
            else if(atoms[i] == x11.atomWindowMaximizedHorz)
                maximized |= 2;
        }
        if(maximized == 3)
            ret = true;
        XFree(propertyValue);
    }
    return ret;
}

std::string Platform::getAppUserDir()
{
    std::stringstream sdir;
    sdir << PHYSFS_getUserDir() << "." << x11.appName;
    if((mkdir(sdir.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) && (errno != EEXIST))
        logError("Couldn't create directory for saving configuration file. (",sdir.str(),")");
    return sdir.str();
}

void Platform::displayFatalError(const std::string& message)
{
    // nothing to do
}
