#ifndef SDL_PLATFORM_H
#define SDL_PLATFORM_H

#include "platformwindow.h"

#ifdef OPENGL_ES
#include <EGL/egl.h>
#endif

class SDLWindow : public PlatformWindow
{
public:
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
    int internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot);
};

#endif
