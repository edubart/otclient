/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

#include "sdlwindow.h"

SDLWindow::SDLWindow()
{
    m_visual = 0;
    m_display = 0;
}

void SDLWindow::init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        g_logger.fatal(stdext::format("SDL video initialization failed: %s", SDL_GetError()));

    m_visual = SDL_GetVideoInfo();
    if(!m_visual)
        g_logger.fatal(stdext::format("SDL video query failed: %s", SDL_GetError()));

    int width = 640;
    int height = 480;
    int bpp = m_visual->vfmt->BitsPerPixel;
    int flags = SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_display = SDL_SetVideoMode(width, height, bpp, flags);
    if(!m_display)
        g_logger.fatal(stdext::format("SDL video mode set failed: %s", SDL_GetError()));

    m_size = Size(width,height);
    m_visible = true;
}

void SDLWindow::terminate()
{
    SDL_Quit();
}

void SDLWindow::move(const Point& pos)
{
    
}

void SDLWindow::resize(const Size& size)
{
    
}

void SDLWindow::show()
{
    
}

void SDLWindow::hide()
{
    
}

void SDLWindow::maximize()
{
    
}

void SDLWindow::poll()
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_KEYDOWN:
            break;
        case SDL_MOUSEMOTION:
            break;
        case SDL_VIDEORESIZE:
            break;
        case SDL_QUIT:
            if(m_onClose)
                m_onClose();
        default:
            //printf("I don't know what this event is!\n");
            break;
        }
    }
}

void SDLWindow::swapBuffers()
{
    SDL_GL_SwapBuffers();
}

void SDLWindow::showMouse()
{
    
}

void SDLWindow::hideMouse()
{
    
}

void SDLWindow::setMouseCursor(int cursorId)
{
    
}
void SDLWindow::restoreMouseCursor()
{
    
}

void SDLWindow::setTitle(const std::string& title)
{
    SDL_WM_SetCaption(title.c_str(), title.c_str());
}

void SDLWindow::setMinimumSize(const Size& minimumSize)
{

}

void SDLWindow::setFullscreen(bool fullscreen)
{

}

void SDLWindow::setVerticalSync(bool enable)
{
    
}
void SDLWindow::setIcon(const std::string& file)
{
    
}
void SDLWindow::setClipboardText(const std::string& text)
{
    
}

Size SDLWindow::getDisplaySize()
{
    return m_size;
}

std::string SDLWindow::getClipboardText()
{
    return std::string();
}

std::string SDLWindow::getPlatformType()
{
    return "SDL";
}

int SDLWindow::internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot)
{
    return 0;
}
