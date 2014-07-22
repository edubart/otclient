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

#ifdef SDL2

#include "sdl2window.h"
#include <framework/graphics/image.h>
#include <framework/core/application.h>
#include <framework/core/resourcemanager.h>

#define HSB_BIT_SET(p, n) (p[(n)/8] |= (128 >>((n)%8)))

SDL2Window::SDL2Window(){
    m_minimumSize = Size(600,480);
    m_size = Size(600,480);
    
    m_defaultCursor = NULL;
    m_cursor = NULL;
    
    m_window = NULL;
    m_context = NULL;
    
    m_keyMap[SDLK_ESCAPE] = Fw::KeyEscape;
    m_keyMap[SDLK_TAB] = Fw::KeyTab;
    m_keyMap[SDLK_RETURN] = Fw::KeyEnter;
    m_keyMap[SDLK_BACKSPACE] = Fw::KeyBackspace;
    m_keyMap[SDLK_SPACE] = Fw::KeySpace;

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

    /*
    m_keyMap[':'] = Fw::KeyColon;
    m_keyMap[';'] = Fw::KeySemicolon;
    m_keyMap['<'] = Fw::KeyLess;
    m_keyMap['='] = Fw::KeyEqual;
    m_keyMap['>'] = Fw::KeyGreater;
    m_keyMap['?'] = Fw::KeyQuestion;
    m_keyMap['@'] = Fw::KeyAtSign;
    */
    
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

    m_keyMap[SDLK_SEMICOLON] = Fw::KeySemicolon;
    m_keyMap[SDLK_SLASH] = Fw::KeySlash;
    m_keyMap[SDLK_BACKQUOTE] = Fw::KeyGrave;
    m_keyMap[SDLK_LEFTBRACKET] = Fw::KeyLeftBracket;
    m_keyMap[SDLK_BACKSLASH] = Fw::KeyBackslash;
    m_keyMap[SDLK_RIGHTBRACKET] = Fw::KeyRightBracket;
    m_keyMap[SDLK_QUOTEDBL] = Fw::KeyApostrophe;
    m_keyMap[SDLK_MINUS] = Fw::KeyMinus;
    m_keyMap[SDLK_PLUS] = Fw::KeyEqual;
    m_keyMap[SDLK_COMMA] = Fw::KeyComma;
    m_keyMap[SDLK_PERIOD] = Fw::KeyPeriod;

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
}

void SDL2Window::init(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        g_logger.fatal("SDL2 failed to initialize video!");
    }
    
    // TODO: OpenGL ES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    
    SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);
    
    uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;
    
    // Create window
    m_window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_size.width(), m_size.height(), flags);
    
    if(m_window == NULL){
        g_logger.fatal("SDL2 failed to create window!");
    }
    
    // Create OpenGL context
    m_context = SDL_GL_CreateContext(m_window);
    
    if(m_context == NULL){
        g_logger.fatal("SDL2 failed to create OpenGL context!");
    }
    
    // Set default cursor
    m_defaultCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    
    m_created = true;
}

void SDL2Window::terminate(){
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void SDL2Window::move(const Point& pos){
    m_position = pos;
    
    SDL_SetWindowPosition(m_window, m_position.x, m_position.y);
}

void SDL2Window::resize(const Size& size){
    m_size = size;
    
    SDL_SetWindowSize(m_window, m_position.x, m_position.y);
}

void SDL2Window::show(){
    //SDL_ShowWindow(m_window);
}

void SDL2Window::hide(){
    SDL_HideWindow(m_window);
}

void SDL2Window::maximize(){
    m_maximized = true;
    
    SDL_MaximizeWindow(m_window);
}

Fw::Key SDL2Window::retranslateVirtualKey(SDL_Keycode keycode){
    Fw::Key key = Fw::KeyUnknown;
    if(m_keyMap.find(keycode) != m_keyMap.end())
        key = m_keyMap[keycode];
        
    return key;
}

void SDL2Window::poll(){
    fireKeysPress();
    
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        SDL_Keymod keyMod = SDL_GetModState();
        
        m_inputEvent.keyboardModifiers = 0;
        if(keyMod & SDL_Keymod(KMOD_CTRL))
            m_inputEvent.keyboardModifiers |= Fw::KeyboardCtrlModifier;
        if(keyMod & SDL_Keymod(KMOD_SHIFT))
            m_inputEvent.keyboardModifiers |= Fw::KeyboardShiftModifier;
        if(keyMod & SDL_Keymod(KMOD_ALT))
            m_inputEvent.keyboardModifiers |= Fw::KeyboardAltModifier;
        
        switch(event.type){
            case SDL_WINDOWEVENT: {
                switch(event.window.event){
                    case SDL_WINDOWEVENT_SHOWN: {
                        m_visible = true;
                        break;
                    }
                    
                    case SDL_WINDOWEVENT_HIDDEN: {
                        m_visible = false;
                        break;
                    }
                    
                    case SDL_WINDOWEVENT_MOVED: {
                        m_position = Point(event.window.data1, event.window.data2);
                        break;
                    }
                    
                    case SDL_WINDOWEVENT_RESIZED: {
                        m_size = Size(event.window.data1, event.window.data2);
                        
                        if(m_onResize)
                            m_onResize(m_size);
                        break;
                    }
                    
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        int width, height;
                        SDL_GetWindowSize(m_window, &width, &height);
                        
                        m_size = Size(width, height);
                        if(m_onResize)
                            m_onResize(m_size);
                        break;
                    }
                    
                    case SDL_WINDOWEVENT_ENTER: {
                        m_focused = true;
                        break;
                    }
                    
                    case SDL_WINDOWEVENT_LEAVE: {
                        m_focused = false;
                        break;
                    }
                    
                    default: {
                        break;
                    }
                }
                
                break;
            }
            
            case SDLK_ESCAPE: {
                m_onClose();
                break;
            }
            
            case SDL_QUIT: {
                m_onClose();
                break;
            }
            
            case SDL_MOUSEMOTION: {
                m_inputEvent.reset(Fw::MouseMoveInputEvent);

                Point newMousePos(event.motion.x, event.motion.y);
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
            
            case SDL_MOUSEBUTTONUP: {
                switch(event.button.button){
                case SDL_BUTTON_LEFT: {
                        SDL_SetWindowGrab(m_window, SDL_FALSE); // Change to SDL_CaptureMouse(SDL_FALSE) with SDL 2.0.4
                        
                        m_inputEvent.reset(Fw::MouseReleaseInputEvent);
                        m_inputEvent.mouseButton = Fw::MouseLeftButton;
                        m_mouseButtonStates[Fw::MouseLeftButton] = false;
                        if(m_onInputEvent)
                            m_onInputEvent(m_inputEvent);
                        
                        break;
                    }
                    
                case SDL_BUTTON_MIDDLE: {
                        SDL_SetWindowGrab(m_window, SDL_FALSE); // Change to SDL_CaptureMouse(SDL_FALSE) with SDL 2.0.4
                        
                        m_inputEvent.reset(Fw::MouseReleaseInputEvent);
                        m_inputEvent.mouseButton = Fw::MouseMidButton;
                        m_mouseButtonStates[Fw::MouseMidButton] = false;
                        if(m_onInputEvent)
                            m_onInputEvent(m_inputEvent);
                        
                        break;
                    }
                    
                case SDL_BUTTON_RIGHT: {
                        SDL_SetWindowGrab(m_window, SDL_FALSE); // Change to SDL_CaptureMouse(SDL_FALSE) with SDL 2.0.4
                        
                        m_inputEvent.reset(Fw::MouseReleaseInputEvent);
                        m_inputEvent.mouseButton = Fw::MouseRightButton;
                        m_mouseButtonStates[Fw::MouseRightButton] = false;
                        if(m_onInputEvent)
                            m_onInputEvent(m_inputEvent);
                        
                        break;
                    }
                }

                
                break;
            }
            
            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button){
                    case SDL_BUTTON_LEFT: {
                        SDL_SetWindowGrab(m_window, SDL_TRUE); // Change to SDL_CaptureMouse(SDL_TRUE) with SDL 2.0.4
                        
                        m_inputEvent.reset(Fw::MousePressInputEvent);
                        m_inputEvent.mouseButton = Fw::MouseLeftButton;
                        m_mouseButtonStates[Fw::MouseLeftButton] = true;
                        if(m_onInputEvent)
                            m_onInputEvent(m_inputEvent);
                        
                        break;
                    }
                    
                    case SDL_BUTTON_MIDDLE: {
                        SDL_SetWindowGrab(m_window, SDL_TRUE); // Change to SDL_CaptureMouse(SDL_TRUE) with SDL 2.0.4
                        
                        m_inputEvent.reset(Fw::MousePressInputEvent);
                        m_inputEvent.mouseButton = Fw::MouseMidButton;
                        m_mouseButtonStates[Fw::MouseMidButton] = true;
                        if(m_onInputEvent)
                            m_onInputEvent(m_inputEvent);
                        
                        break;
                    }
                    
                    case SDL_BUTTON_RIGHT: {
                        SDL_SetWindowGrab(m_window, SDL_TRUE); // Change to SDL_CaptureMouse(SDL_TRUE) with SDL 2.0.4
                        
                        m_inputEvent.reset(Fw::MousePressInputEvent);
                        m_inputEvent.mouseButton = Fw::MouseRightButton;
                        m_mouseButtonStates[Fw::MouseRightButton] = true;
                        if(m_onInputEvent)
                            m_onInputEvent(m_inputEvent);
                        
                        break;
                    }
                }
                
                break;
            }
            
            case SDL_MOUSEWHEEL: {
                m_inputEvent.reset(Fw::MouseWheelInputEvent);
                m_inputEvent.mouseButton = Fw::MouseMidButton;
                m_inputEvent.wheelDirection = event.wheel.y > 0 ? Fw::MouseWheelUp : Fw::MouseWheelDown;
                if(m_onInputEvent)
                    m_onInputEvent(m_inputEvent);
                
                break;
            }
            
            case SDL_TEXTINPUT: {
                for(uint32 i = 0; i < strlen(event.text.text); i++){
                    m_inputEvent.reset(Fw::KeyTextInputEvent);
                    m_inputEvent.keyText = event.text.text[i];
                    if(m_onInputEvent)
                        m_onInputEvent(m_inputEvent);
                }
                
                break;
            }
            
            case SDL_KEYDOWN: {
                processKeyDown(retranslateVirtualKey(event.key.keysym.sym));
                break;
            }
            
            case SDL_KEYUP: {
                processKeyUp(retranslateVirtualKey(event.key.keysym.sym));
                break;
            }
            
            default: {      
                break;
            }
        }
    }
    
    updateUnmaximizedCoords();
}

void SDL2Window::swapBuffers(){
    SDL_GL_SwapWindow(m_window);
}

void SDL2Window::showMouse(){
    SDL_ShowCursor(1);
}

void SDL2Window::hideMouse(){
    SDL_ShowCursor(0);
}

void SDL2Window::displayFatalError(const std::string& message){
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "FATAL ERROR", message.c_str(), m_window);
}

void SDL2Window::restoreMouseCursor(){
    SDL_SetCursor(m_defaultCursor);
    SDL_ShowCursor(1);
}


void SDL2Window::setTitle(const std::string& title){
    SDL_SetWindowTitle(m_window, title.c_str());
}

void SDL2Window::setMinimumSize(const Size& minimumSize){
    m_minimumSize = minimumSize;

    SDL_SetWindowMinimumSize(m_window, minimumSize.width(), minimumSize.height());
}

void SDL2Window::setFullscreen(bool fullscreen){
    if(m_fullscreen == fullscreen)
        return;
    
    SDL_SetWindowFullscreen(m_window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    m_fullscreen = fullscreen;
}

void SDL2Window::setVerticalSync(bool enable){
    SDL_GL_SetSwapInterval(enable ? 1 : 0);
}

void SDL2Window::setIcon(const std::string& iconFile){
    ImagePtr image = Image::load(iconFile);

    if(!image) {
        g_logger.traceError(stdext::format("unable to load icon file %s", iconFile));
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
    
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(&iconData[0], image->getWidth(), image->getHeight(), 32, image->getWidth() * 4, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    
    if(!surface) {
        std::string error = SDL_GetError();
        g_logger.traceError(stdext::format("unable to create SDL surface from file %s, error: %s", iconFile, error));
        return;
    }
    
    SDL_SetWindowIcon(m_window, surface);
    SDL_FreeSurface(surface);
}

void SDL2Window::setClipboardText(const std::string& text){
    SDL_SetClipboardText(text.c_str());
}


Size SDL2Window::getDisplaySize(){
    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0) {
        return Size(displayMode.w, displayMode.h);
    }
    
    return Size(640, 480);
}

std::string SDL2Window::getClipboardText(){
    char* str = SDL_GetClipboardText();
    std::string tmp(str);
    
    SDL_free(str);
    return tmp;
}

std::string SDL2Window::getPlatformType(){
    return "SDL2";
}

int SDL2Window::internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot){
    int width = image->getWidth();
    int height = image->getHeight();
    int numbits = width * height;
    int numbytes = (width * height)/8;

    std::vector<uchar> data(numbytes, 0);
    std::vector<uchar> mask(numbytes, 0);

    for(int i=0;i<numbits;++i) {
        uint32 rgba = stdext::readLE32(image->getPixelData() + i*4);
        if(rgba == 0xffffffff) { //white
            HSB_BIT_SET(mask, i);
        } else if(rgba != 0x00000000) { // 0xff000000 => black
            HSB_BIT_SET(data, i);
            HSB_BIT_SET(mask, i);
        }
    }
    
    SDL_Cursor* cursor = SDL_CreateCursor(&data[0], &mask[0], width, height, hotSpot.x, hotSpot.y);
    
    m_cursors.push_back(cursor);
    return m_cursors.size()-1;
}

void SDL2Window::setMouseCursor(int cursorId){
    if(cursorId >= (int)m_cursors.size() || cursorId < 0)
        return;

    m_cursor = m_cursors[cursorId];
    SDL_SetCursor(m_cursor);
    SDL_ShowCursor(1);
}

#endif
