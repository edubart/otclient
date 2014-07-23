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

#ifndef SDL2WINDOW_H
#define SDL2WINDOW_H

#include "platformwindow.h"

#include <SDL2/SDL.h>

class SDL2Window : public PlatformWindow
{
    int internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot);
    void internalRestoreGLContext();
public:
    SDL2Window();
    void init();
    void terminate();

    void move(const Point& pos);
    void resize(const Size& size);
    void show();
    void hide();
    void maximize();
    void poll();
    void swapBuffers();
    void showMouse();
    void hideMouse();
    void displayFatalError(const std::string& message);

    void setMouseCursor(int cursorId);
    void restoreMouseCursor();

    void setTitle(const std::string& title);
    void setMinimumSize(const Size& minimumSize);
    void setFullscreen(bool fullscreen);
    void setVerticalSync(bool enable);
    void setIcon(const std::string& iconFile);
    void setClipboardText(const std::string& text);

    Size getDisplaySize();
    std::string getClipboardText();
    std::string getPlatformType();
protected:
    Fw::Key retranslateVirtualKey(SDL_Keycode keycode);
private:
    SDL_Cursor* m_defaultCursor;
    SDL_Cursor* m_cursor;
    std::vector<SDL_Cursor*> m_cursors;
    
    SDL_Window* m_window;
    SDL_GLContext m_context;
    
    stdext::boolean<true> m_hidden;
};

#endif
