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
}

void SDLWindow::init() {
    window = 0;
    gl = 0;

    initializeSDL();
}

void SDLWindow::initializeSDL() {
    if(SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        g_logger.fatal( stdext::format("Unable to initialize SDL: %s", SDL_GetError()) );
        terminate();
    }

    setSDLAttributes();

    SDL_GetDisplayMode( 0, 0, &mode );
    int width = mode.w;
    int height = mode.h;

    window = SDL_CreateWindow( NULL, 0, 0, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE );

    if( window == 0 ) {
        g_logger.fatal("Failed to create window");
        terminate();
    }

    gl = SDL_GL_CreateContext(window);
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
}

void SDLWindow::hide() {
    // TODO?
}

void SDLWindow::poll() {
    // TODO
}

void SDLWindow::swapBuffers() {
    SDL_GL_SwapWindow(window);
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
    Size size(mode.w, mode.h);
    return size;
}

std::string SDLWindow::getPlatformType() {
    return "Android";
}

void SDLWindow::show() {
    // android doesn't need to show activity, it's open automatically
}

void SDLWindow::maximize() {
    // android doesn't has window
}

void SDLWindow::move(const Point& pos) {
    // android doesn't has window
}

void SDLWindow::resize(const Size& size) {
    // android doesn't resize window
}

void SDLWindow::showMouse() {
    // android doesn't has mouse
}

void SDLWindow::hideMouse() {
    // android doesn't has mouse
}

int SDLWindow::internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot) {
    // android doesn't has mouse
    return 0;
}

void SDLWindow::setMouseCursor(int cursorId) {
    // android doesn't has mouse
}

void SDLWindow::restoreMouseCursor() {
    // android doesn't has mouse
}

void SDLWindow::setTitle(const std::string& title) {
    // android doesn't need to set title, the app make it
}

void SDLWindow::setMinimumSize(const Size& minimumSize) {
    // android doesn't has window
}

void SDLWindow::setFullscreen(bool fullscreen) {
    // android doesn't has window
}

void SDLWindow::setIcon(const std::string& iconFile) {
    // android doesn't has window
}

#endif // ANDROID
