/*
 * Copyright (c) 2010-2016 OTClient <https://github.com/edubart/otclient>
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

#ifndef WIN32

#include "x11window.h"
#include <framework/core/resourcemanager.h>
#include <framework/graphics/image.h>
#include <unistd.h>

#define LSB_BIT_SET(p, n) (p[(n)/8] |= (1 <<((n)%8)))

X11Window::X11Window()
{
    m_display = nullptr;
    m_visual = nullptr;
    m_window = 0;
    m_rootWindow = 0;
    m_colormap = 0;
    m_cursor = 0;
    m_hiddenCursor = 0;
    m_xim = nullptr;
    m_xic = nullptr;
    m_screen = 0;
    m_wmDelete = 0;
    m_minimumSize = Size(600,480);
    m_size = Size(600,480);

#ifdef OPENGL_ES
    m_eglConfig = 0;
    m_eglContext = 0;
    m_eglDisplay = 0;
    m_eglSurface = 0;
#else
    m_fbConfig = nullptr;
    m_glxContext = nullptr;
#endif

    m_keyMap[XK_Escape] = Fw::KeyEscape;
    m_keyMap[XK_Tab] = Fw::KeyTab;
    m_keyMap[XK_Return] = Fw::KeyEnter;
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
    m_keyMap[XK_Alt_R] = Fw::KeyAlt;
    m_keyMap[XK_Alt_L] = Fw::KeyAlt;
    m_keyMap[XK_Meta_L] = Fw::KeyMeta;
    m_keyMap[XK_Meta_R] = Fw::KeyMeta;
    m_keyMap[XK_Menu] = Fw::KeyMenu;

#if defined(__APPLE__)
    m_keyMap[65406] = Fw::KeyAlt;// Option
    m_keyMap[65511] = Fw::KeyMeta;// Command
#endif

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
    /*
    m_keyMap[XK_KP_Add] = Fw::KeyPlus;
    m_keyMap[XK_KP_Subtract] = Fw::KeyMinus;
    m_keyMap[XK_KP_Decimal] = Fw::KeyPeriod;
    m_keyMap[XK_KP_Divide] = Fw::KeySlash;
    m_keyMap[XK_KP_Multiply] = Fw::KeyAsterisk;
    */
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
    internalOpenDisplay();
    internalCheckGL();
    internalChooseGLVisual();
    internalCreateGLContext();
    internalCreateWindow();
}

void X11Window::terminate()
{
    if(m_cursor != None) {
        XUndefineCursor(m_display, m_window);
        m_cursor = None;
    }

    if(m_hiddenCursor) {
        XFreeCursor(m_display, m_hiddenCursor);
        m_hiddenCursor = 0;
    }

    for(Cursor cursor : m_cursors)
        XFreeCursor(m_display, cursor);
    m_cursors.clear();

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
        m_visual = nullptr;
    }

    if(m_xic) {
        XDestroyIC(m_xic);
        m_xic = nullptr;
    }

    if(m_xim) {
        XCloseIM(m_xim);
        m_xim = nullptr;
    }

    if(m_display) {
        XCloseDisplay(m_display);
        m_display = nullptr;
    }

    m_visible = false;
}

void X11Window::internalOpenDisplay()
{
    m_display = XOpenDisplay(nullptr);
    if(!m_display)
        g_logger.fatal("Unable to open X11 display");
    m_screen = DefaultScreen(m_display);
}

void X11Window::internalCreateWindow()
{
    Visual *vis;
    int depth;
    unsigned int attrsMask;
    XSetWindowAttributes attrs;
    memset(&attrs, 0, sizeof(attrs));

    attrs.event_mask = KeyPressMask | KeyReleaseMask |
                       ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                       ExposureMask | VisibilityChangeMask |
                       StructureNotifyMask | FocusChangeMask;

    m_colormap = XCreateColormap(m_display, m_rootWindow, m_visual->visual, AllocNone);
    attrs.colormap = m_colormap;
    attrs.border_pixel = 0;
    attrs.override_redirect = False;
    vis = m_visual->visual;
    depth = m_visual->depth;
    attrsMask = CWEventMask | CWBorderPixel | CWColormap;

#ifdef OPENGL_ES
    attrs.override_redirect = False;
    attrsMask |= CWOverrideRedirect;
#endif

    updateUnmaximizedCoords();
    m_window = XCreateWindow(m_display, m_rootWindow,
                             m_position.x, m_position.y, m_size.width(), m_size.height(),
                             0,
                             depth,
                             InputOutput,
                             vis,
                             attrsMask, &attrs);
    m_visible = true;

    if(!m_window)
        g_logger.fatal("Unable to create X11 window!");

    // ensure window input focus
    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(m_display, m_window, &hints);

    // ensure window position
    XMoveWindow(m_display, m_window, m_position.x, m_position.y);

    // handle wm_delete events
    m_wmDelete = XInternAtom(m_display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(m_display, m_window, &m_wmDelete , 1);

    if(!internalSetupWindowInput())
        g_logger.warning("Input of special keys may be messed up, because window input initialization failed");

    internalConnectGLContext();
}

bool X11Window::internalSetupWindowInput()
{
    //  create input context (to have better key input handling)
    if(!XSupportsLocale()) {
        g_logger.error("X11 doesn't support the current locale");
        return false;
    }

    XSetLocaleModifiers("");
    m_xim = XOpenIM(m_display, nullptr, nullptr, nullptr);
    if(!m_xim) {
        g_logger.error("XOpenIM failed");
        return false;
    }

    m_xic = XCreateIC(m_xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, m_window, NULL);
    if(!m_xic) {
        g_logger.error("Unable to create the input context");
        return false;
    }

    return true;
}

void X11Window::internalCheckGL()
{
#ifdef OPENGL_ES
    m_eglDisplay = eglGetDisplay((EGLNativeDisplayType)m_display);
    if(m_eglDisplay == EGL_NO_DISPLAY)
        g_logger.fatal("EGL not supported");

    if(!eglInitialize(m_eglDisplay, NULL, NULL))
        g_logger.fatal("Unable to initialize EGL");
#else
    if(!glXQueryExtension(m_display, nullptr, nullptr))
        g_logger.fatal("GLX not supported");
#endif
}

void X11Window::internalChooseGLVisual()
{
#ifdef OPENGL_ES
    static int attrList[] = {
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
    XVisualInfo visTemplate;
    int numVisuals;

    if(!eglChooseConfig(m_eglDisplay, attrList, &m_eglConfig, 1, &numConfig))
        g_logger.fatal("Failed to choose EGL config");

    if(numConfig != 1)
        g_logger.warning("Didn't got the exact EGL config");

    EGLint vid;
    if(!eglGetConfigAttrib(m_eglDisplay, m_eglConfig, EGL_NATIVE_VISUAL_ID, &vid))
        g_logger.fatal("Unable to get visual EGL visual id");

    memset(&visTemplate, 0, sizeof(visTemplate));
    visTemplate.visualid = vid;
    m_visual = XGetVisualInfo(m_display, VisualIDMask, &visTemplate, &numVisuals);
    if(!m_visual)
        g_logger.fatal("Couldn't choose RGBA, double buffered visual");

    m_rootWindow = DefaultRootWindow(m_display);
#else
    static int attrList[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER, True,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        None
    };

    int nelements;
    m_fbConfig = glXChooseFBConfig(m_display, m_screen, attrList, &nelements);
    if(!m_fbConfig)
        g_logger.fatal("Couldn't choose RGBA, double buffered fbconfig");

    m_visual = glXGetVisualFromFBConfig(m_display, *m_fbConfig);
    if(!m_visual)
        g_logger.fatal("Couldn't choose RGBA, double buffered visual");

    m_rootWindow = RootWindow(m_display, m_visual->screen);
#endif
}

void X11Window::internalCreateGLContext()
{
#ifdef OPENGL_ES
    EGLint attrList[] = {
#if OPENGL_ES==2
        EGL_CONTEXT_CLIENT_VERSION, 2,
#else
        EGL_CONTEXT_CLIENT_VERSION, 1,
#endif
        EGL_NONE
    };

    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, attrList);
    if(m_eglContext == EGL_NO_CONTEXT )
        g_logger.fatal(stdext::format("Unable to create EGL context: %s", eglGetError()));
#else
    m_glxContext = glXCreateContext(m_display, m_visual, nullptr, True);

    if(!m_glxContext)
        g_logger.fatal("Unable to create GLX context");

    if(!glXIsDirect(m_display, m_glxContext))
        g_logger.warning("GL direct rendering is not possible");
#endif
}

void X11Window::internalDestroyGLContext()
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
    if(m_glxContext) {
        glXMakeCurrent(m_display, None, nullptr);
        glXDestroyContext(m_display, m_glxContext);
        m_glxContext = nullptr;
    }
#endif
}

void X11Window::internalConnectGLContext()
{
#ifdef OPENGL_ES
    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_window, NULL);
    if(m_eglSurface == EGL_NO_SURFACE)
        g_logger.fatal(stdext::format("Unable to create EGL surface: %s", eglGetError()));
    if(!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext))
        g_logger.fatal("Unable to connect EGL context into X11 window");
#else
    if(!glXMakeCurrent(m_display, m_window, m_glxContext))
        g_logger.fatal("Unable to set GLX context on X11 window");
#endif
}

void *X11Window::getExtensionProcAddress(const char *ext)
{
#ifdef OPENGL_ES
    //TODO
    return NULL;
#else
    return (void *)glXGetProcAddressARB((const GLubyte*)ext);
#endif
}

bool X11Window::isExtensionSupported(const char *ext)
{
#ifdef OPENGL_ES
    //TODO
    return false;
#else
    const char *exts = glXQueryExtensionsString(m_display, m_screen);
    if(strstr(exts, ext))
        return true;
#endif
    return false;
}

void X11Window::move(const Point& pos)
{
    m_position = pos;
    if(m_visible) {
        XMoveWindow(m_display, m_window, m_position.x, m_position.y);
        XFlush(m_display);
    }
}

void X11Window::resize(const Size& size)
{
    if(size.width() < m_minimumSize.width() || size.height() < m_minimumSize.height())
        return;
    XResizeWindow(m_display, m_window, size.width(), size.height());
    XFlush(m_display);
}

void X11Window::show()
{
    m_visible = true;
    XMapWindow(m_display, m_window);
    XMoveWindow(m_display, m_window, m_position.x, m_position.y);
    XFlush(m_display);
    if(m_maximized)
        maximize();
    if(m_fullscreen)
        setFullscreen(true);
}

void X11Window::hide()
{
    m_visible = false;
    XUnmapWindow(m_display, m_window);
    XFlush(m_display);
}

void X11Window::maximize()
{
    if(m_visible) {
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
    m_maximized = true;
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
            XLookupString(&xkey, buf, sizeof(buf), &keysym, nullptr);
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

                // checks if the window is maximized
                if(m_visible) {
                    m_maximized = false;
                    Atom wmState = XInternAtom(m_display, "_NET_WM_STATE", False);
                    Atom wmStateMaximizedVert = XInternAtom(m_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
                    Atom wmStateMaximizedHorz = XInternAtom(m_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
                    Atom actualType;
                    ulong i, numItems, bytesAfter;
                    uchar *propertyValue = nullptr;
                    int actualFormat;

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
                            m_maximized = true;

                        XFree(propertyValue);
                    }
                }

                // updates window pos
                if(m_visible)
                    m_position = newPos;
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
                                        XInternAtom(m_display, "STRING", False),
                                        XInternAtom(m_display, "text/plain;charset=UTF-8", False),
                                        XInternAtom(m_display, "text/plain", False),
                                        XInternAtom(m_display, "COMPOUND_TEXT", False),
                                        XA_STRING };

                    XChangeProperty(m_display, req->requestor,
                                    req->property, req->target,
                                    8, PropModeReplace,
                                    (uchar *)&typeList,
                                    sizeof(typeList));
                    respond.xselection.property = req->property;
                } else {
                    std::string clipboardText = stdext::latin1_to_utf8(m_clipboardText);
                    XChangeProperty(m_display,
                                    req->requestor,
                                    req->property, req->target,
                                    8,
                                    PropModeReplace,
                                    (uchar *)clipboardText.c_str(),
                                    clipboardText.length());
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
                    static XComposeStatus compose = {nullptr, 0};
                    len = XLookupString(&event.xkey, buf, sizeof(buf), &keysym, &compose);
                }

                // filter unwanted characters
                if(len == 0 || (uchar)(buf[0]) < 32 || keysym == XK_BackSpace || keysym == XK_Return || keysym == XK_Delete || keysym == XK_Escape)
                    break;
                std::string text = buf;

                //g_logger.debug("char: ", buf[0], " code: ", (int)((uchar)buf[0]));

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
                        m_mouseButtonStates[Fw::MouseLeftButton] = (event.type == ButtonPress);
                        break;
                    case Button3:
                        m_inputEvent.mouseButton = Fw::MouseRightButton;
                        m_mouseButtonStates[Fw::MouseRightButton] = (event.type == ButtonPress);
                        break;
                    case Button2:
                        m_inputEvent.mouseButton = Fw::MouseMidButton;
                        m_mouseButtonStates[Fw::MouseMidButton] = (event.type == ButtonPress);
                        break;
                    case Button4:
                        if(event.type == ButtonPress) {
                            m_inputEvent.type = Fw::MouseWheelInputEvent;
                            m_inputEvent.mouseButton = Fw::MouseMidButton;
                            m_inputEvent.wheelDirection = Fw::MouseWheelUp;
                        } else
                            m_inputEvent.type = Fw::NoInputEvent;
                        break;
                    case Button5:
                        if(event.type == ButtonPress) {
                            m_inputEvent.type = Fw::MouseWheelInputEvent;
                            m_inputEvent.mouseButton = Fw::MouseMidButton;
                            m_inputEvent.wheelDirection = Fw::MouseWheelDown;
                        } else
                            m_inputEvent.type = Fw::NoInputEvent;
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
                needsResizeUpdate = true;
                break;
            case UnmapNotify:
                m_visible = false;
                releaseAllKeys();
                break;
            case FocusIn:
                m_focused = true;
                releaseAllKeys();
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
#ifdef OPENGL_ES
    eglSwapBuffers(m_eglDisplay, m_eglSurface);
#else
    glXSwapBuffers(m_display, m_window);
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

    if(m_hiddenCursor == None) {
        char bm[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        Pixmap pix = XCreateBitmapFromData(m_display, m_window, bm, 8, 8);
        XColor black;
        memset(&black, 0, sizeof(black));
        black.flags = DoRed | DoGreen | DoBlue;
        m_hiddenCursor = XCreatePixmapCursor(m_display, pix, pix, &black, &black, 0, 0);
        XFreePixmap(m_display, pix);
    }

    m_cursor = m_hiddenCursor;
    XDefineCursor(m_display, m_window, m_cursor);
}

void X11Window::setMouseCursor(int cursorId)
{
    if(cursorId >= (int)m_cursors.size() || cursorId < 0)
        return;

    if(m_cursor != None)
        restoreMouseCursor();

    m_cursor = m_cursors[cursorId];
    XDefineCursor(m_display, m_window, m_cursor);
}

void X11Window::restoreMouseCursor()
{
    XUndefineCursor(m_display, m_window);
    m_cursor = None;
}

int X11Window::internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot)
{
    int width = image->getWidth();
    int height = image->getHeight();
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
        uint32 rgba = stdext::readULE32(image->getPixelData() + i*4);
        if(rgba == 0xffffffff) { //white, background
            LSB_BIT_SET(maskBits, i);
        } else if(rgba == 0xff000000) { //black, foreground
            LSB_BIT_SET(mapBits, i);
            LSB_BIT_SET(maskBits, i);
        } //otherwise 0x00000000 => alpha
    }

    Pixmap cp = XCreateBitmapFromData(m_display, m_window, (char*)&mapBits[0], width, height);
    Pixmap mp = XCreateBitmapFromData(m_display, m_window, (char*)&maskBits[0], width, height);
    Cursor cursor = XCreatePixmapCursor(m_display, cp, mp, &fg, &bg, hotSpot.x, hotSpot.y);
    XFreePixmap(m_display, cp);
    XFreePixmap(m_display, mp);

    m_cursors.push_back(cursor);
    return m_cursors.size()-1;
}

void X11Window::setTitle(const std::string& title)
{
    XStoreName(m_display, m_window, title.c_str());
    XSetIconName(m_display, m_window, title.c_str());
}

void X11Window::setMinimumSize(const Size& minimumSize)
{
    XSizeHints sizeHints;
    memset(&sizeHints, 0, sizeof(sizeHints));
    sizeHints.flags = PMinSize;
    sizeHints.min_width = minimumSize.width();
    sizeHints.min_height= minimumSize.height();
    XSetWMSizeHints(m_display, m_window, &sizeHints, XA_WM_NORMAL_HINTS);
}

void X11Window::setFullscreen(bool fullscreen)
{
    if(m_visible) {
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
    }
    m_fullscreen = fullscreen;
}

void X11Window::setVerticalSync(bool enable)
{
#ifdef OPENGL_ES
    //TODO
#else
    typedef GLint (*glSwapIntervalProc)(GLint);
    glSwapIntervalProc glSwapInterval = nullptr;

    if(isExtensionSupported("GLX_MESA_swap_control"))
        glSwapInterval = (glSwapIntervalProc)getExtensionProcAddress("glXSwapIntervalMESA");
    else if(isExtensionSupported("GLX_SGI_swap_control"))
        glSwapInterval = (glSwapIntervalProc)getExtensionProcAddress("glXSwapIntervalSGI");

    if(glSwapInterval)
        glSwapInterval(enable ? 1 : 0);
#endif
}

void X11Window::setIcon(const std::string& file)
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
    std::vector<unsigned long int> iconData(n + 2);
    iconData[0] = image->getWidth();
    iconData[1] = image->getHeight();
    for(int i=0; i < n;++i) {
        uint8 *pixel = (uint8*)&iconData[2 + i];
        pixel[2] = *(image->getPixelData() + (i * 4) + 0);
        pixel[1] = *(image->getPixelData() + (i * 4) + 1);
        pixel[0] = *(image->getPixelData() + (i * 4) + 2);
        pixel[3] = *(image->getPixelData() + (i * 4) + 3);
    }

    Atom property = XInternAtom(m_display, "_NET_WM_ICON", 0);
    if(!XChangeProperty(m_display, m_window, property, XA_CARDINAL, 32, PropModeReplace, (const unsigned char*)&iconData[0], iconData.size()))
        g_logger.error("Couldn't set app icon");
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
        XConvertSelection(m_display, clipboard, XA_STRING, XA_PRIMARY, ownerWindow, CurrentTime);
        XFlush(m_display);

        // hack to wait SelectioNotify event, otherwise we will get wrong clipboard pastes
        // TODO: fix this in a correct way
        stdext::millisleep(100);

        // check for data
        Atom type;
        int format;
        ulong len, bytesLeft;
        char *data;
        XGetWindowProperty(m_display, ownerWindow,
                            XA_PRIMARY, 0, 10000000L, 0, XA_STRING,
                            &type,
                            &format,
                            &len,
                            &bytesLeft,
                            (uchar**)&data);
        if(len > 0) {
            if(stdext::is_valid_utf8(data))
                clipboardText = stdext::utf8_to_latin1(data);
            else
                clipboardText = data;
        }
    }

    return clipboardText;
}

std::string X11Window::getPlatformType()
{
#ifndef OPENGL_ES
    return "X11-GLX";
#else
    return "X11-EGL";
#endif
}

#endif
