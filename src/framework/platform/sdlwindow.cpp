/*
 * Copyright (c) 2010-2014 OTClient <https://github.com/edubart/otclient>
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

#ifdef ANDROID

#include "sdlwindow.h"

SDLWindow::SDLWindow() {
    m_minimumSize = Size(600,480);
    m_size = Size(600,480);
    m_window = 0;

    //m_keyMap[SDLK_ESCAPE] = Fw::KeyEscape;
    //m_keyMap[SDLK_TAB] = Fw::KeyTab;
    m_keyMap[SDLK_RETURN] = Fw::KeyEnter;
    m_keyMap[SDLK_BACKSPACE] = Fw::KeyBackspace;
    m_keyMap[SDLK_SPACE] = Fw::KeySpace;

    /*
    m_keyMap[SDLK_PAGEUP] = Fw::KeyPageUp;
    m_keyMap[SDLK_PAGEDOWN] = Fw::KeyPageDown;
    m_keyMap[SDLK_HOME] = Fw::KeyHome;
    m_keyMap[SDLK_END] = Fw::KeyEnd;
    m_keyMap[SDLK_INSERT] = Fw::KeyInsert;
    m_keyMap[SDLK_DELETE] = Fw::KeyDelete;

    m_keyMap[SDLK_UP] = Fw::KeyUp;
    m_keyMap[SDLK_DOWN] = Fw::KeyDown;
    m_keyMap[SDLK_LEFT] = Fw::KeyLeft;
    m_keyMap[SDLK_RIGHT] = Fw::KeyRight;

    m_keyMap[SDLK_NUMLOCKCLEAR] = Fw::KeyNumLock;
    m_keyMap[SDLK_SCROLLLOCK] = Fw::KeyScrollLock;
    m_keyMap[SDLK_CAPSLOCK] = Fw::KeyCapsLock;
    m_keyMap[SDLK_PRINTSCREEN] = Fw::KeyPrintScreen;
    m_keyMap[SDLK_PAUSE] = Fw::KeyPause;

    m_keyMap[SDLK_LCTRL] = Fw::KeyCtrl;
    m_keyMap[SDLK_RCTRL] = Fw::KeyCtrl;
    m_keyMap[SDLK_LSHIFT] = Fw::KeyShift;
    m_keyMap[SDLK_RSHIFT] = Fw::KeyShift;
    m_keyMap[SDLK_LALT] = Fw::KeyAlt;
    m_keyMap[SDLK_RALT] = Fw::KeyAlt;
    m_keyMap[SDLK_LGUI] = Fw::KeyMeta;
    m_keyMap[SDLK_RGUI] = Fw::KeyMeta;
    */

    // ascii characters
    m_keyMap[SDLK_EXCLAIM] = Fw::KeyExclamation;
    m_keyMap[SDLK_QUOTEDBL] = Fw::KeyQuote;
    m_keyMap[SDLK_HASH] = Fw::KeyNumberSign;
    m_keyMap[SDLK_DOLLAR] = Fw::KeyDollar;
    m_keyMap[SDLK_PERCENT] = Fw::KeyPercent;
    m_keyMap[SDLK_AMPERSAND] = Fw::KeyAmpersand;
    m_keyMap[SDLK_QUOTE] = Fw::KeyApostrophe;
    m_keyMap[SDLK_LEFTPAREN] = Fw::KeyLeftParen;
    m_keyMap[SDLK_RIGHTPAREN] = Fw::KeyRightParen;
    m_keyMap[SDLK_ASTERISK] = Fw::KeyAsterisk;
    m_keyMap[SDLK_PLUS] = Fw::KeyPlus;
    m_keyMap[SDLK_COMMA] = Fw::KeyComma;
    m_keyMap[SDLK_MINUS] = Fw::KeyMinus;
    m_keyMap[SDLK_PERIOD] = Fw::KeyPeriod;
    m_keyMap[SDLK_SLASH] = Fw::KeySlash;

    m_keyMap[SDLK_0] = Fw::Key0;
    m_keyMap[SDLK_1] = Fw::Key1;
    m_keyMap[SDLK_2] = Fw::Key2;
    m_keyMap[SDLK_3] = Fw::Key3;
    m_keyMap[SDLK_4] = Fw::Key4;
    m_keyMap[SDLK_5] = Fw::Key5;
    m_keyMap[SDLK_6] = Fw::Key6;
    m_keyMap[SDLK_7] = Fw::Key7;
    m_keyMap[SDLK_8] = Fw::Key8;
    m_keyMap[SDLK_9] = Fw::Key9;

    m_keyMap[SDLK_COLON] = Fw::KeyColon;
    m_keyMap[SDLK_SEMICOLON] = Fw::KeySemicolon;
    m_keyMap[SDLK_LESS] = Fw::KeyLess;
    m_keyMap[SDLK_EQUALS] = Fw::KeyEqual;
    m_keyMap[SDLK_GREATER] = Fw::KeyGreater;
    m_keyMap[SDLK_QUESTION] = Fw::KeyQuestion;
    m_keyMap[SDLK_AT] = Fw::KeyAtSign;

    m_keyMap[SDLK_a] = Fw::KeyA;
    m_keyMap[SDLK_b] = Fw::KeyB;
    m_keyMap[SDLK_c] = Fw::KeyC;
    m_keyMap[SDLK_d] = Fw::KeyD;
    m_keyMap[SDLK_e] = Fw::KeyE;
    m_keyMap[SDLK_f] = Fw::KeyF;
    m_keyMap[SDLK_g] = Fw::KeyG;
    m_keyMap[SDLK_h] = Fw::KeyH;
    m_keyMap[SDLK_i] = Fw::KeyI;
    m_keyMap[SDLK_j] = Fw::KeyJ;
    m_keyMap[SDLK_k] = Fw::KeyK;
    m_keyMap[SDLK_l] = Fw::KeyL;
    m_keyMap[SDLK_m] = Fw::KeyM;
    m_keyMap[SDLK_n] = Fw::KeyN;
    m_keyMap[SDLK_o] = Fw::KeyO;
    m_keyMap[SDLK_p] = Fw::KeyP;
    m_keyMap[SDLK_q] = Fw::KeyQ;
    m_keyMap[SDLK_r] = Fw::KeyR;
    m_keyMap[SDLK_s] = Fw::KeyS;
    m_keyMap[SDLK_t] = Fw::KeyT;
    m_keyMap[SDLK_u] = Fw::KeyU;
    m_keyMap[SDLK_v] = Fw::KeyV;
    m_keyMap[SDLK_w] = Fw::KeyW;
    m_keyMap[SDLK_x] = Fw::KeyX;
    m_keyMap[SDLK_y] = Fw::KeyY;
    m_keyMap[SDLK_z] = Fw::KeyZ;

    m_keyMap[SDLK_LGUI] = Fw::KeyLeftBracket;
    m_keyMap[SDLK_BACKSLASH] = Fw::KeyBackslash;
    m_keyMap[SDLK_RIGHTBRACKET] = Fw::KeyRightBracket;
    m_keyMap[SDLK_CARET] = Fw::KeyCaret;
    m_keyMap[SDLK_UNDERSCORE] = Fw::KeyUnderscore;
    m_keyMap[SDLK_BACKQUOTE] = Fw::KeyGrave;
    //m_keyMap['{'] = Fw::KeyLeftCurly;
    //m_keyMap['|'] = Fw::KeyBar;
    //m_keyMap['}'] = Fw::KeyRightCurly;
    //m_keyMap['~'] = Fw::KeyTilde;


    // keypad
    /*
    m_keyMap[SDLK_KP_PLUS] = Fw::KeyPlus;
    m_keyMap[SDLK_KP_MINUS] = Fw::KeyMinus;
    m_keyMap[SDLK_KP_DECIMAL] = Fw::KeyPeriod;
    m_keyMap[SDLK_KP_DIVIDE] = Fw::KeySlash;
    m_keyMap[SDLK_KP_MULTIPLY] = Fw::KeyAsterisk;

    m_keyMap[VK_OEM_1] = Fw::KeySemicolon;
    m_keyMap[VK_OEM_2] = Fw::KeySlash;
    m_keyMap[VK_OEM_3] = Fw::KeyGrave;
    m_keyMap[VK_OEM_4] = Fw::KeyLeftBracket;
    m_keyMap[VK_OEM_5] = Fw::KeyBackslash;
    m_keyMap[VK_OEM_6] = Fw::KeyRightBracket;
    m_keyMap[VK_OEM_7] = Fw::KeyApostrophe;
    m_keyMap[VK_OEM_MINUS] = Fw::KeyMinus;
    m_keyMap[VK_OEM_PLUS] = Fw::KeyEqual;
    m_keyMap[SDLK_KP_COMMA] = Fw::KeyComma;
    m_keyMap[VK_OEM_PERIOD] = Fw::KeyPeriod;

    m_keyMap[SDLK_F1] = Fw::KeyF1;
    m_keyMap[SDLK_F2] = Fw::KeyF2;
    m_keyMap[SDLK_F3] = Fw::KeyF3;
    m_keyMap[SDLK_F4] = Fw::KeyF4;
    m_keyMap[SDLK_F5] = Fw::KeyF5;
    m_keyMap[SDLK_F6] = Fw::KeyF6;
    m_keyMap[SDLK_F7] = Fw::KeyF7;
    m_keyMap[SDLK_F8] = Fw::KeyF8;
    m_keyMap[SDLK_F9] = Fw::KeyF9;
    m_keyMap[SDLK_F10] = Fw::KeyF10;
    m_keyMap[SDLK_F11] = Fw::KeyF11;
    m_keyMap[SDLK_F12] = Fw::KeyF12;
    */
}

void SDLWindow::init() {
    initializeSDL();
}

void SDLWindow::initializeSDL() {
    if(SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        g_logger.fatal( stdext::format("Unable to initialize SDL: %s", SDL_GetError()) );
        terminate();
    }

    setSDLAttributes();

    SDL_GetDisplayMode(0, 0, &m_mode);
    int width = m_mode.w;
    int height = m_mode.h;

    m_window = SDL_CreateWindow( NULL, 0, 0, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE );

    m_size = Size( width, height );

    if( m_window == 0 ) {
        g_logger.fatal("Failed to create window");
        terminate();
    }

    m_visible = true;

    SDL_GL_CreateContext(m_window);
    //SDL_StartTextInput();
}

void SDLWindow::setSDLAttributes() {
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 4);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 4);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 4);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_ES);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
}

void SDLWindow::terminate() {
    SDL_Quit();
    m_visible = false;
}

void SDLWindow::poll() {
    while(SDL_PollEvent(&m_event)) {
        processKeydownOrKeyrelease();

        if(hasRepeatedKey())
            continue;

        switch(m_event.type) {
            case SDL_TEXTINPUT:
                processTextInput();
                break;
            case SDL_FINGERDOWN:
            case SDL_FINGERUP:
                processFingerdownAndFingerup();
                break;
        }

        if(m_inputEvent.type != Fw::NoInputEvent && m_onInputEvent)
            m_onInputEvent(m_inputEvent);
    }

    fireKeysPress();
}

bool SDLWindow::hasRepeatedKey() {
    return m_event.key.repeat != 0;
}

void SDLWindow::processKeydownOrKeyrelease() {
    if(m_event.key.state == SDL_PRESSED || (m_event.key.state == SDL_RELEASED && !hasRepeatedKey())) {
        Fw::Key keyCode = Fw::KeyUnknown;
        SDL_Keycode keysym = m_event.key.keysym.sym;

        if(m_keyMap.find(keysym) != m_keyMap.end())
            keyCode = m_keyMap[keysym];

        if(m_event.type == SDL_PRESSED)
            processKeyDown(keyCode);
        else if(m_event.type == SDL_RELEASED)
            processKeyUp(keyCode);
    }
}

void SDLWindow::processTextInput() {
    std::string text = m_event.text.text;
    SDL_Keycode keysym = m_event.key.keysym.sym;

    if(text.length() == 0 || keysym == SDLK_BACKSPACE || keysym == SDLK_RETURN)
        return;

    if(m_onInputEvent) {
        m_inputEvent.reset(Fw::KeyTextInputEvent);
        m_inputEvent.keyText = text;
        m_onInputEvent(m_inputEvent);
    }
}

void SDLWindow::processFingerdownAndFingerup() {
    m_inputEvent.reset();
    m_inputEvent.type = (m_event.type == SDL_FINGERDOWN) ? Fw::MousePressInputEvent : Fw::MouseReleaseInputEvent;
    m_inputEvent.mouseButton = Fw::MouseLeftButton;
    m_mouseButtonStates[Fw::MouseLeftButton] = true;
    Point newMousePos(m_event.tfinger.x * m_mode.w, m_event.tfinger.y * m_mode.h);
    m_inputEvent.mouseMoved = newMousePos - m_inputEvent.mousePos;
    m_inputEvent.mousePos = newMousePos;
    if(m_onInputEvent)
        m_onInputEvent(m_inputEvent);
}

void SDLWindow::swapBuffers() {
    SDL_GL_SwapWindow(m_window);
}

void SDLWindow::setVerticalSync(bool enable) {
    // TODO
}

std::string SDLWindow::getClipboardText() {
    return SDL_GetClipboardText();
}

void SDLWindow::setClipboardText(const std::string& text) {
    SDL_SetClipboardText(text.c_str());
}

Size SDLWindow::getDisplaySize() {
    Size size(m_mode.w, m_mode.h);
    return size;
}

std::string SDLWindow::getPlatformType() {
    return "ANDROID_SDL2";
}

void SDLWindow::show() {
    // mobile devices doesn't need to show activity, it's open automatically
}

void SDLWindow::hide() {
    // SDL make this for us
}

void SDLWindow::maximize() {
    // mobile devices doesn't has window
}

void SDLWindow::move(const Point& pos) {
    // mobile devices doesn't has window
}

void SDLWindow::resize(const Size& size) {
    // android doesn't resize window
}

void SDLWindow::showMouse() {
    // mobile devices doesn't has mouse
}

void SDLWindow::hideMouse() {
    // mobile devices doesn't has mouse
}

int SDLWindow::internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot) {
    // mobile devices doesn't has mouse
    return 0;
}

void SDLWindow::setMouseCursor(int cursorId) {
    // mobile devices doesn't has mouse
}

void SDLWindow::restoreMouseCursor() {
    // mobile devices doesn't has mouse
}

void SDLWindow::setTitle(const std::string& title) {
    // mobile devices doesn't need to set title, the app make it
}

void SDLWindow::setMinimumSize(const Size& minimumSize) {
    // mobile devices doesn't has window
}

void SDLWindow::setFullscreen(bool fullscreen) {
    // mobile devices doesn't has window
}

void SDLWindow::setIcon(const std::string& iconFile) {
    // mobile devices doesn't has icon
}

#endif // ANDROID
