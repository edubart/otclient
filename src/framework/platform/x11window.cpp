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

#include "x11window.h"
#include <framework/core/resourcemanager.h>
#include <framework/thirdparty/apngloader.h>

#define LSB_BIT_SET(p, n) (p[(n)/8] |= (1 <<((n)%8)))

X11Window::X11Window()
{
    m_display = 0;
    m_visual = 0;
    m_window = 0;
    m_rootWindow = 0;
    m_colormap = 0;
    m_cursor = 0;
    m_xim = 0;
    m_xic = 0;
    m_screen = 0;
    m_wmDelete = 0;
    m_size = Size(16,16);

#ifndef OPENGL_ES2
    m_glxContext = 0;
#else
    m_eglConfig = 0;
    m_eglContext = 0;
    m_eglDisplay = 0;
    m_eglSurface = 0;
#endif

    m_keyMap[XK_Escape] = Fw::KeyEscape;
    m_keyMap[XK_Tab] = Fw::KeyTab;
    m_keyMap[XK_Return] = Fw::KeyReturn;
    m_keyMap[XK_BackSpace] = Fw::KeyBackspace;

    m_keyMap[XK_Page_Up] = Fw::KeyPageUp;
    m_keyMap[XK_Page_Down] = Fw::KeyPageDown;
    m_keyMap[XK_Home] = Fw::KeyHome;
    m_keyMap[XK_End] = Fw::KeyEnd;
    m_keyMap[XK_Insert] = Fw::KeyInsert;
    m_keyMap[XK_Delete] = Fw::KeyDelete;

    m_keyMap[XK_Up] = Fw::KeyUp;
    m_keyMap[XK_Down] = Fw::KeyDown;
    m_keyMap[XK_Left] = Fw::KeyLeft;
    m_keyMap[XK_Right] = Fw::KeyRight;

    m_keyMap[XK_Num_Lock] = Fw::KeyNumLock;
    m_keyMap[XK_Scroll_Lock] = Fw::KeyScrollLock;
    m_keyMap[XK_Caps_Lock] = Fw::KeyCapsLock;
    m_keyMap[XK_Print] = Fw::KeyPrintScreen;
    m_keyMap[XK_Pause] = Fw::KeyPause;

    m_keyMap[XK_Control_L] = Fw::KeyCtrl;
    m_keyMap[XK_Control_R] = Fw::KeyCtrl;
    m_keyMap[XK_Shift_R] = Fw::KeyShift;
    m_keyMap[XK_Shift_L] = Fw::KeyShift;
    m_keyMap[XK_Alt_R] = Fw::KeyAltGr;
    m_keyMap[XK_Alt_L] = Fw::KeyAlt;
    m_keyMap[XK_Meta_L] = Fw::KeyMeta;
    m_keyMap[XK_Meta_R] = Fw::KeyMeta;
    m_keyMap[XK_Menu] = Fw::KeyMenu;

    // ascii characters
    m_keyMap[XK_space] = Fw::KeySpace;
    m_keyMap[XK_exclam] = Fw::KeyExclamation;
    m_keyMap[XK_quotedbl] = Fw::KeyQuote;
    m_keyMap[XK_numbersign] = Fw::KeyNumberSign;
    m_keyMap[XK_dollar] = Fw::KeyDollar;
    m_keyMap[XK_percent] = Fw::KeyPercent;
    m_keyMap[XK_ampersand] = Fw::KeyAmpersand;
    m_keyMap[XK_apostrophe] = Fw::KeyApostrophe;
    m_keyMap[XK_parenleft] = Fw::KeyLeftParen;
    m_keyMap[XK_parenright] = Fw::KeyRightParen;
    m_keyMap[XK_asterisk] = Fw::KeyAsterisk;
    m_keyMap[XK_plus] = Fw::KeyPlus;
    m_keyMap[XK_comma] = Fw::KeyComma;
    m_keyMap[XK_minus] = Fw::KeyMinus;
    m_keyMap[XK_period] = Fw::KeyPeriod;
    m_keyMap[XK_slash] = Fw::KeySlash;
    m_keyMap[XK_1] = Fw::Key1;
    m_keyMap[XK_2] = Fw::Key2;
    m_keyMap[XK_3] = Fw::Key3;
    m_keyMap[XK_4] = Fw::Key4;
    m_keyMap[XK_5] = Fw::Key5;
    m_keyMap[XK_6] = Fw::Key6;
    m_keyMap[XK_7] = Fw::Key7;
    m_keyMap[XK_8] = Fw::Key8;
    m_keyMap[XK_9] = Fw::Key9;
    m_keyMap[XK_0] = Fw::Key0;
    m_keyMap[XK_colon] = Fw::KeyColon;
    m_keyMap[XK_semicolon] = Fw::KeySemicolon;
    m_keyMap[XK_less] = Fw::KeyLess;
    m_keyMap[XK_equal] = Fw::KeyEqual;
    m_keyMap[XK_greater] = Fw::KeyGreater;
    m_keyMap[XK_question] = Fw::KeyQuestion;
    m_keyMap[XK_at] = Fw::KeyAtSign;
    m_keyMap[XK_a] = Fw::KeyA;
    m_keyMap[XK_b] = Fw::KeyB;
    m_keyMap[XK_c] = Fw::KeyC;
    m_keyMap[XK_d] = Fw::KeyD;
    m_keyMap[XK_e] = Fw::KeyE;
    m_keyMap[XK_f] = Fw::KeyF;
    m_keyMap[XK_g] = Fw::KeyG;
    m_keyMap[XK_h] = Fw::KeyH;
    m_keyMap[XK_i] = Fw::KeyI;
    m_keyMap[XK_j] = Fw::KeyJ;
    m_keyMap[XK_k] = Fw::KeyK;
    m_keyMap[XK_l] = Fw::KeyL;
    m_keyMap[XK_m] = Fw::KeyM;
    m_keyMap[XK_n] = Fw::KeyN;
    m_keyMap[XK_o] = Fw::KeyO;
    m_keyMap[XK_p] = Fw::KeyP;
    m_keyMap[XK_q] = Fw::KeyQ;
    m_keyMap[XK_r] = Fw::KeyR;
    m_keyMap[XK_s] = Fw::KeyS;
    m_keyMap[XK_t] = Fw::KeyT;
    m_keyMap[XK_u] = Fw::KeyU;
    m_keyMap[XK_v] = Fw::KeyV;
    m_keyMap[XK_w] = Fw::KeyW;
    m_keyMap[XK_x] = Fw::KeyX;
    m_keyMap[XK_y] = Fw::KeyY;
    m_keyMap[XK_z] = Fw::KeyZ;
    m_keyMap[XK_bracketleft] = Fw::KeyLeftBracket;
    m_keyMap[XK_backslash] = Fw::KeyBackslash;
    m_keyMap[XK_bracketright] = Fw::KeyRightBracket;
    m_keyMap[XK_asciicircum] = Fw::KeyCaret;
    m_keyMap[XK_underscore] = Fw::KeyUnderscore;
    m_keyMap[XK_grave] = Fw::KeyGrave;
    m_keyMap[XK_braceleft] = Fw::KeyLeftCurly;
    m_keyMap[XK_bar] = Fw::KeyBar;
    m_keyMap[XK_braceright] = Fw::KeyRightCurly;
    m_keyMap[XK_asciitilde] = Fw::KeyTilde;

    // keypad
    m_keyMap[XK_KP_Add] = Fw::KeyPlus;
    m_keyMap[XK_KP_Subtract] = Fw::KeyMinus;
    m_keyMap[XK_KP_Decimal] = Fw::KeyPeriod;
    m_keyMap[XK_KP_Divide] = Fw::KeySlash;
    m_keyMap[XK_KP_Multiply] = Fw::KeyAsterisk;
    m_keyMap[XK_KP_Enter] = Fw::KeyEnter;

    // keypad with numlock off
    m_keyMap[XK_KP_Insert] = Fw::KeyNumpad0;
    m_keyMap[XK_KP_End] = Fw::KeyNumpad1;
    m_keyMap[XK_KP_Down] = Fw::KeyNumpad2;
    m_keyMap[XK_KP_Page_Down] = Fw::KeyNumpad3;
    m_keyMap[XK_KP_Left] = Fw::KeyNumpad4;
    m_keyMap[XK_KP_Begin] = Fw::KeyNumpad5;
    m_keyMap[XK_KP_Right] = Fw::KeyNumpad6;
    m_keyMap[XK_KP_Home] = Fw::KeyNumpad7;
    m_keyMap[XK_KP_Up] = Fw::KeyNumpad8;
    m_keyMap[XK_KP_Page_Up] = Fw::KeyNumpad9;
    m_keyMap[XK_KP_Delete] = Fw::KeyDelete;

    // keypad with numlock on
    m_keyMap[XK_KP_0] = Fw::Key0;
    m_keyMap[XK_KP_1] = Fw::Key1;
    m_keyMap[XK_KP_2] = Fw::Key2;
    m_keyMap[XK_KP_3] = Fw::Key3;
    m_keyMap[XK_KP_4] = Fw::Key4;
    m_keyMap[XK_KP_5] = Fw::Key5;
    m_keyMap[XK_KP_6] = Fw::Key6;
    m_keyMap[XK_KP_7] = Fw::Key7;
    m_keyMap[XK_KP_8] = Fw::Key8;
    m_keyMap[XK_KP_9] = Fw::Key9;

    m_keyMap[XK_F1] = Fw::KeyF1;
    m_keyMap[XK_F2] = Fw::KeyF2;
    m_keyMap[XK_F3] = Fw::KeyF3;
    m_keyMap[XK_F4] = Fw::KeyF4;
    m_keyMap[XK_F5] = Fw::KeyF5;
    m_keyMap[XK_F6] = Fw::KeyF6;
    m_keyMap[XK_F7] = Fw::KeyF7;
    m_keyMap[XK_F8] = Fw::KeyF8;
    m_keyMap[XK_F9] = Fw::KeyF9;
    m_keyMap[XK_F10] = Fw::KeyF10;
    m_keyMap[XK_F11] = Fw::KeyF11;
    m_keyMap[XK_F12] = Fw::KeyF12;
}

void X11Window::init()
{
    m_size = Size(200, 200);
    internalOpenDisplay();
    internalCheckGL();
    internalChooseGLVisual();
    internalCreateGLContext();
    internalCreateWindow();
}

void X11Window::terminate()
{
    if(m_window) {
        XDestroyWindow(m_display, m_window);
        m_window = 0;
    }

    if(m_colormap) {
        XFreeColormap(m_display, m_colormap);
        m_colormap = 0;
    }

    internalDestroyGLContext();

    if(m_visual) {
        XFree(m_visual);
        m_visual = 0;
    }

    if(m_xic) {
        XDestroyIC(m_xic);
        m_xic = 0;
    }

    if(m_xim) {
        XCloseIM(m_xim);
        m_xim = 0;
    }

    if(m_display) {
        XCloseDisplay(m_display);
        m_display = 0;
    }

    m_visible = false;
}

void X11Window::internalOpenDisplay()
{
    m_display = XOpenDisplay(NULL);
    if(!m_display)
        logFatal("Unable to open X11 display");
    m_screen = DefaultScreen(m_display);
}

void X11Window::internalCreateWindow()
{
    Visual *vis;
    int depth;
    unsigned int attrsMask = CWEventMask;
    XSetWindowAttributes attrs = {0};

    attrs.event_mask = KeyPressMask | KeyReleaseMask |
                       ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                       ExposureMask | VisibilityChangeMask |
                       StructureNotifyMask | FocusChangeMask;

    if(m_visual) {
        m_colormap = XCreateColormap(m_display, m_rootWindow, m_visual->visual, AllocNone);
        attrs.colormap = m_colormap;
        attrs.border_pixel = 0;
        attrsMask |= CWBorderPixel | CWColormap;
        vis = m_visual->visual;
        depth = m_visual->depth;
    } else {
        attrs.override_redirect = False;
        attrsMask |= CWOverrideRedirect;
        depth = CopyFromParent;
        vis = CopyFromParent;
    }

    updateUnmaximizedCoords();
    m_window = XCreateWindow(m_display, m_rootWindow,
                             m_pos.x, m_pos.y, m_size.width(), m_size.height(),
                             0,
                             depth,
                             InputOutput,
                             vis,
                             attrsMask, &attrs);
    if(!m_window)
        logFatal("Unable to create X11 window!");

    // ensure window input focus
    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(m_display, m_window, &hints);

    // ensure window position
    XMoveWindow(m_display, m_window, m_pos.x, m_pos.y);

    // handle wm_delete events
    m_wmDelete = XInternAtom(m_display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(m_display, m_window, &m_wmDelete , 1);

    if(!internalSetupWindowInput())
        logWarning("Input of special keys may be messed up, because window input initialization failed");

    internalConnectGLContext();
}

bool X11Window::internalSetupWindowInput()
{
    // try to set a latin1 locales, otherwise fallback to standard C locale
    static char locales[4][32] = { "en_US.iso88591", "iso88591", "en_US", "C" };
    for(int i=0;i<4;++i) {
        if(setlocale(LC_ALL, locales[i]))
            break;
    }

    //  create input context (to have better key input handling)
    if(!XSupportsLocale()) {
        logError("X11 doesn't support the current locale");
        return false;
    }

    XSetLocaleModifiers("");
    m_xim = XOpenIM(m_display, NULL, NULL, NULL);
    if(!m_xim) {
        logError("XOpenIM failed");
        return false;
    }

    m_xic = XCreateIC(m_xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, m_window, NULL);
    if(!m_xic) {
        logError("Unable to create the input context");
        return false;
    }

    return true;
}

void X11Window::internalCheckGL()
{
#ifndef OPENGL_ES2
    if(!glXQueryExtension(m_display, NULL, NULL))
        logFatal("GLX not supported");
#else
    m_eglDisplay = eglGetDisplay((EGLNativeDisplayType)m_display);
    if(m_eglDisplay == EGL_NO_DISPLAY)
        logFatal("EGL not supported");

    if(!eglInitialize(m_eglDisplay, NULL, NULL))
        logFatal("Unable to initialize EGL");
#endif
}

void X11Window::internalChooseGLVisual()
{
#ifndef OPENGL_ES2
    static int attrList[] = {
        GLX_USE_GL,
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        None
    };

    m_visual = glXChooseVisual(m_display, m_screen, attrList);
    if(!m_visual)
        logFatal("Couldn't choose RGBA, double buffered visual");

    m_rootWindow = RootWindow(m_display, m_visual->screen);
#else
    static int attrList[] = {
        //EGL_BUFFER_SIZE, 24,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLint numConfig;
    if(!eglChooseConfig(m_eglDisplay, attrList, &m_eglConfig, 1, &numConfig))
        logFatal("Failed to choose EGL config");

    if(numConfig != 1)
        logWarning("Didn't got the exact EGL config");

    m_rootWindow = DefaultRootWindow(m_display);
#endif
}

void X11Window::internalCreateGLContext()
{
#ifndef OPENGL_ES2
    m_glxContext = glXCreateContext(m_display, m_visual, NULL, True);
    if(!m_glxContext)
        logFatal("Unable to create GLX context");

    if(!glXIsDirect(m_display, m_glxContext))
        logWarning("GL direct rendering is not possible");
#else
    EGLint attrList[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, attrList);
    if(m_eglContext == EGL_NO_CONTEXT )
        logFatal("Unable to create EGL context: ", eglGetError());
#endif
}

void X11Window::internalDestroyGLContext()
{
#ifndef OPENGL_ES2
    if(m_glxContext) {
        glXMakeCurrent(m_display, None, NULL);
        glXDestroyContext(m_display, m_glxContext);
        m_glxContext = 0;
    }
#else
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
#endif
}

void X11Window::internalConnectGLContext()
{
#ifndef OPENGL_ES2
    if(!glXMakeCurrent(m_display, m_window, m_glxContext))
        logFatal("Unable to set GLX context on X11 window");
#else
    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_window, NULL);
    if(m_eglSurface == EGL_NO_SURFACE)
        logFatal("Unable to create EGL surface: ", eglGetError());
    if(!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext))
        logFatal("Unable to connect EGL context into X11 window");
#endif
}

void *X11Window::getExtensionProcAddress(const char *ext)
{
#ifndef OPENGL_ES2
    return (void *)glXGetProcAddressARB((const GLubyte*)ext);
#else
    //TODO
    return false;
#endif
}

bool X11Window::isExtensionSupported(const char *ext)
{
#ifndef OPENGL_ES2
    const char *exts = glXQueryExtensionsString(m_display, m_screen);
    if(strstr(exts, ext))
        return true;
#else
    //TODO
#endif
    return false;
}

void X11Window::move(const Point& pos)
{
    bool wasVisible = isVisible();
    if(!wasVisible)
        show();
    XMoveWindow(m_display, m_window, pos.x, pos.y);
    if(!wasVisible)
        hide();
}

void X11Window::resize(const Size& size)
{
    XResizeWindow(m_display, m_window, size.width(), size.height());
}

void X11Window::show()
{
    XMapWindow(m_display, m_window);
}

void X11Window::hide()
{
    XUnmapWindow(m_display, m_window);
}

void X11Window::maximize()
{
    Atom wmState = XInternAtom(m_display, "_NET_WM_STATE", False);
    Atom wmStateMaximizedVert = XInternAtom(m_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
    Atom wmStateMaximizedHorz = XInternAtom(m_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);

    XEvent e = {0};
    e.xany.type = ClientMessage;
    e.xclient.send_event = True;
    e.xclient.message_type = wmState;
    e.xclient.format = 32;
    e.xclient.window = m_window;
    e.xclient.data.l[0] = 1;
    e.xclient.data.l[1] = wmStateMaximizedVert;
    e.xclient.data.l[2] = wmStateMaximizedHorz;
    e.xclient.data.l[3] = 0;

    XSendEvent(m_display, m_rootWindow, 0, SubstructureNotifyMask | SubstructureRedirectMask, &e);
    XFlush(m_display);
}

void X11Window::poll()
{
    bool needsResizeUpdate = false;

    XEvent event, peekEvent;
    while(XPending(m_display) > 0) {
        XNextEvent(m_display, &event);

        // check for repeated key releases
        bool repatedKeyRelease = false;
        if(event.type == KeyRelease && XPending(m_display)) {
            XPeekEvent(m_display, &peekEvent);
            if((peekEvent.type == KeyPress) && (peekEvent.xkey.keycode == event.xkey.keycode) && ((peekEvent.xkey.time-event.xkey.time) < 2))
                repatedKeyRelease = true;
        }

        // process keydown and keyrelease events first
        if(event.type == KeyPress || (event.type == KeyRelease && !repatedKeyRelease)) {
            // remove caps lock and shift maks
            XKeyEvent xkey = event.xkey;
            xkey.state &= ~(ShiftMask | LockMask);

            // lookup keysym and translate it
            KeySym keysym;
            char buf[32];
            int len = XLookupString(&xkey, buf, sizeof(buf), &keysym, 0);
            Fw::Key keyCode = Fw::KeyUnknown;
            if(m_keyMap.find(keysym) != m_keyMap.end())
                keyCode = m_keyMap[keysym];

            if(event.type == KeyPress)
                processKeyDown(keyCode);
            else if(event.type == KeyRelease)
                processKeyUp(keyCode);
        }

        // call filter because xim will discard KeyPress events when keys still composing
        if(XFilterEvent(&event, m_window))
            continue;

        // discard repated key releases
        if(repatedKeyRelease)
            continue;

        switch(event.type) {
            case ClientMessage: {
                // close event
                if((Atom)event.xclient.data.l[0] == m_wmDelete && m_onClose)
                    m_onClose();
                break;
            }
            case ConfigureNotify: {
                Size newSize(event.xconfigure.width, event.xconfigure.height);
                Point newPos(event.xconfigure.x, event.xconfigure.y);

                // updates window size
                if(m_size != newSize) {
                    m_size = newSize;
                    needsResizeUpdate = true;
                }

                // updates window pos
                m_pos = newPos;
                updateUnmaximizedCoords();
                break;
            }
            case SelectionRequest: {
                XEvent respond;
                XSelectionRequestEvent *req = &(event.xselectionrequest);

                Atom targets = XInternAtom(m_display, "TARGETS", False);
                if(req->target == targets) {
                    Atom typeList[] = { XInternAtom(m_display, "UTF8_STRING", False),
                                        XInternAtom(m_display, "TEXT", False),
                                        XInternAtom(m_display, "COMPOUND_TEXT", False),
                                        XA_STRING };

                    XChangeProperty(m_display, req->requestor,
                                    req->property, req->target,
                                    8, PropModeReplace,
                                    (uchar *)&typeList,
                                    sizeof(typeList));
                    respond.xselection.property = req->property;
                } else {
                    XChangeProperty(m_display,
                                    req->requestor,
                                    req->property, req->target,
                                    8,
                                    PropModeReplace,
                                    (uchar *)m_clipboardText.c_str(),
                                    m_clipboardText.length());
                    respond.xselection.property = req->property;
                }

                respond.xselection.type = SelectionNotify;
                respond.xselection.display = req->display;
                respond.xselection.requestor = req->requestor;
                respond.xselection.selection = req->selection;
                respond.xselection.target = req->target;
                respond.xselection.time = req->time;
                XSendEvent(m_display, req->requestor, 0, 0, &respond);
                XFlush(m_display);
                break;
            }
            // process text events
            case KeyPress: {
                // text cant be insert while holding ctrl or alt
                if(event.xkey.state & ControlMask || event.xkey.state & Mod1Mask)
                    break;

                // process key text events
                KeySym keysym;
                char buf[32];
                memset(buf, 0, 32);
                int len;

                // lookup for keyText
                if(m_xic) { // with xim we can get latin1 input correctly
                    Status status;
                    len = XmbLookupString(m_xic, &event.xkey, buf, sizeof(buf), &keysym, &status);
                } else { // otherwise use XLookupString, but often it doesn't work right with dead keys
                    static XComposeStatus compose = {NULL, 0};
                    len = XLookupString(&event.xkey, buf, sizeof(buf), &keysym, &compose);
                }

                // filter unwanted characters
                if(len == 0 || (uchar)(buf[0]) < 32 || keysym == XK_BackSpace || keysym == XK_Return || keysym == XK_Delete || keysym == XK_Escape)
                    break;
                std::string text = buf;

                //logDebug("char: ", buf[0], " code: ", (int)((uchar)buf[0]));

                if(m_onInputEvent && text.length() > 0) {
                    m_inputEvent.reset(Fw::KeyTextInputEvent);
                    m_inputEvent.keyText = text;
                    m_onInputEvent(m_inputEvent);
                }
                break;
            }
            case ButtonPress:
            case ButtonRelease: {
                m_inputEvent.reset();
                m_inputEvent.type = (event.type == ButtonPress) ? Fw::MousePressInputEvent : Fw::MouseReleaseInputEvent;
                switch(event.xbutton.button) {
                    case Button1:
                        m_inputEvent.mouseButton = Fw::MouseLeftButton;
                        break;
                    case Button3:
                        m_inputEvent.mouseButton = Fw::MouseRightButton;
                        break;
                    case Button2:
                        m_inputEvent.mouseButton = Fw::MouseMidButton;
                        break;
                    case Button4:
                        m_inputEvent.type = Fw::MouseWheelInputEvent;
                        m_inputEvent.mouseButton = Fw::MouseMidButton;
                        m_inputEvent.wheelDirection = Fw::MouseWheelUp;
                        break;
                    case Button5:
                        m_inputEvent.type = Fw::MouseWheelInputEvent;
                        m_inputEvent.mouseButton = Fw::MouseMidButton;
                        m_inputEvent.wheelDirection = Fw::MouseWheelDown;
                        break;
                    default:
                        m_inputEvent.type = Fw::NoInputEvent;
                        break;
                }
                if(m_inputEvent.type != Fw::NoInputEvent && m_onInputEvent)
                    m_onInputEvent(m_inputEvent);
                break;
            }

            case MotionNotify: {
                m_inputEvent.reset();
                m_inputEvent.type = Fw::MouseMoveInputEvent;
                Point newMousePos(event.xbutton.x, event.xbutton.y);
                m_inputEvent.mouseMoved = newMousePos - m_inputEvent.mousePos;
                m_inputEvent.mousePos = newMousePos;
                if(m_onInputEvent)
                    m_onInputEvent(m_inputEvent);
                break;
            }
            case MapNotify:
                m_visible = true;
                break;
            case UnmapNotify:
                m_visible = false;
                releaseAllKeys();
                break;
            case FocusIn:
                m_focused = true;
                break;
            case FocusOut:
                m_focused = false;
                releaseAllKeys();
                break;
            case Expose:
                // window needs redraw
                break;
        }
    }

    if(needsResizeUpdate && m_onResize)
        m_onResize(m_size);

    fireKeysPress();
}

void X11Window::swapBuffers()
{
#ifndef OPENGL_ES2
    glXSwapBuffers(m_display, m_window);
#else
    eglSwapBuffers(m_eglDisplay, m_eglSurface);
#endif
}

void X11Window::showMouse()
{
    restoreMouseCursor();
}

void X11Window::hideMouse()
{
    if(m_cursor != None)
        restoreMouseCursor();

    char bm[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    Pixmap pix = XCreateBitmapFromData(m_display, m_window, bm, 8, 8);
    XColor black = {0};
    black.flags = DoRed | DoGreen | DoBlue;
    m_cursor = XCreatePixmapCursor(m_display, pix, pix, &black, &black, 0, 0);
    XFreePixmap(m_display, pix);
    XDefineCursor(m_display, m_window, m_cursor);
}

void X11Window::restoreMouseCursor()
{
    XUndefineCursor(m_display, m_window);
    if(m_cursor != None) {
        XFreeCursor(m_display, m_cursor);
        m_cursor = None;
    }
}

void X11Window::setMouseCursor(const std::string& file)
{
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

    if(apng.width % 8 != 0 || apng.height % 8 != 0) {
        logError("the cursor png must have dimensions multiple of 8");
        free_apng(&apng);
        return;
    }

    if(m_cursor != None)
        restoreMouseCursor();

    int width = apng.width;
    int height = apng.height;
    int numbits = width * height;
    int numbytes = (width * height)/8;

    XColor bg, fg;
    bg.red   = 255 << 8;
    bg.green = 255 << 8;
    bg.blue  = 255 << 8;
    fg.red   = 0;
    fg.green = 0;
    fg.blue  = 0;

    std::vector<uchar> mapBits(numbytes, 0);
    std::vector<uchar> maskBits(numbytes, 0);

    for(int i=0;i<numbits;++i) {
        uchar r = apng.pdata[i*4+0];
        uchar g = apng.pdata[i*4+1];
        uchar b = apng.pdata[i*4+2];
        uchar a = apng.pdata[i*4+3];
        Color color(r,g,b,a);
        if(color == Fw::white) { //background
            LSB_BIT_SET(maskBits, i);
        } else if(color == Fw::black) { //foreground
            LSB_BIT_SET(mapBits, i);
            LSB_BIT_SET(maskBits, i);
        } //otherwise alpha
    }
    free_apng(&apng);

    Pixmap cp = XCreateBitmapFromData(m_display, m_window, (char*)&mapBits[0], width, height);
    Pixmap mp = XCreateBitmapFromData(m_display, m_window, (char*)&maskBits[0], width, height);
    m_cursor = XCreatePixmapCursor(m_display, cp, mp, &fg, &bg, width/2, height/2);
    XDefineCursor(m_display, m_window, m_cursor);
    XFreePixmap(m_display, cp);
    XFreePixmap(m_display, mp);
}

void X11Window::setTitle(const std::string& title)
{
    XStoreName(m_display, m_window, title.c_str());
    XSetIconName(m_display, m_window, title.c_str());
}

void X11Window::setMinimumSize(const Size& minimumSize)
{
    XSizeHints sizeHints = {0};
    sizeHints.flags = PMinSize;
    sizeHints.min_width = minimumSize.width();
    sizeHints.min_height= minimumSize.height();
    XSetWMSizeHints(m_display, m_window, &sizeHints, XA_WM_NORMAL_HINTS);
}

void X11Window::setFullscreen(bool fullscreen)
{
    if(m_fullscreen != fullscreen) {
        Atom wmState = XInternAtom(m_display, "_NET_WM_STATE", False);
        Atom wmStateFullscreen = XInternAtom(m_display, "_NET_WM_STATE_FULLSCREEN", False);

        XEvent xev;
        xev.xclient.type = ClientMessage;
        xev.xclient.serial = 0;
        xev.xclient.send_event = True;
        xev.xclient.window = m_window;
        xev.xclient.message_type = wmState;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = fullscreen ? 1 : 0;
        xev.xclient.data.l[1] = wmStateFullscreen;
        xev.xclient.data.l[2] = 0;

        XSendEvent(m_display, m_rootWindow, False,  SubstructureRedirectMask | SubstructureNotifyMask, &xev);
        XFlush(m_display);

        m_fullscreen = fullscreen;
    }
}

void X11Window::setVerticalSync(bool enable)
{
#ifndef OPENGL_ES2
    typedef GLint (*glSwapIntervalProc)(GLint);
    glSwapIntervalProc glSwapInterval = NULL;

    if(isExtensionSupported("GLX_MESA_swap_control"))
        glSwapInterval = (glSwapIntervalProc)getExtensionProcAddress("glXSwapIntervalMESA");
    else if(isExtensionSupported("GLX_SGI_swap_control"))
        glSwapInterval = (glSwapIntervalProc)getExtensionProcAddress("glXSwapIntervalSGI");

    if(glSwapInterval)
        glSwapInterval(enable ? 1 : 0);
#else
    //TODO
#endif
}

void X11Window::setIcon(const std::string& iconFile)
{
    std::stringstream fin;
    g_resources.loadFile(iconFile, fin);

    apng_data apng;
    if(load_apng(fin, &apng) != 0) {
        logError("Unable to load window icon");
        return;
    }

    if(apng.bpp != 4) {
        logError("Could not set window icon, icon image must have 4 channels");
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

    Atom property = XInternAtom(m_display, "_NET_WM_ICON", 0);
    if(!XChangeProperty(m_display, m_window, property, XA_CARDINAL, 32, PropModeReplace, (const unsigned char*)&iconData[0], iconData.size()))
        logError("Couldn't set app icon");

    free_apng(&apng);
}

void X11Window::setClipboardText(const std::string& text)
{
    m_clipboardText = text;
    Atom clipboard = XInternAtom(m_display, "CLIPBOARD", False);
    XSetSelectionOwner(m_display, clipboard, m_window, CurrentTime);
    XFlush(m_display);
}

Size X11Window::getDisplaySize()
{
    return Size(XDisplayWidth(m_display, m_screen), XDisplayHeight(m_display, m_screen));
}

std::string X11Window::getClipboardText()
{
    Atom clipboard = XInternAtom(m_display, "CLIPBOARD", False);
    Window ownerWindow = XGetSelectionOwner(m_display, clipboard);
    if(ownerWindow == m_window)
        return m_clipboardText;

    std::string clipboardText;
    if(ownerWindow != None) {
        XConvertSelection(m_display, clipboard, XA_STRING, 0, ownerWindow, CurrentTime);
        XFlush(m_display);

        // hack to wait SelectioNotify event, otherwise we will get wrong clipboard pastes
        // TODO: fix this in a correct way
        usleep(100 * 1000);

        // check for data
        Atom type;
        int format;
        ulong numItems, bytesLeft, dummy;
        uchar *data;
        XGetWindowProperty(m_display, ownerWindow,
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
            int result = XGetWindowProperty(m_display, ownerWindow,
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
                clipboardText = (const char*)data;
            XFree(data);
        }
    }

    return clipboardText;
}

std::string X11Window::getPlatformType()
{
#ifndef OPENGL_ES2
    return "X11-GLX";
#else
    return "X11-EGL";
#endif
}

bool X11Window::isMaximized()
{
    if(!m_display || !m_window)
        return false;

    Atom wmState = XInternAtom(m_display, "_NET_WM_STATE", False);
    Atom wmStateMaximizedVert = XInternAtom(m_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
    Atom wmStateMaximizedHorz = XInternAtom(m_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    Atom actualType;
    ulong i, numItems, bytesAfter;
    uchar *propertyValue = NULL;
    int actualFormat;
    bool maximized = false;

    if(XGetWindowProperty(m_display, m_window, wmState,
                          0, 1024, False, XA_ATOM, &actualType,
                          &actualFormat, &numItems, &bytesAfter,
                          &propertyValue) == Success) {
        Atom *atoms = (Atom*)propertyValue;
        int maximizedMask = 0;

        for(i=0; i<numItems; ++i) {
            if(atoms[i] == wmStateMaximizedVert)
                maximizedMask |= 1;
            else if(atoms[i] == wmStateMaximizedHorz)
                maximizedMask |= 2;
        }

        if(maximizedMask == 3)
            maximized = true;

        XFree(propertyValue);
    }

    return maximized;
}
