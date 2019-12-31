/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#ifndef PLATFORMWINDOW_H
#define PLATFORMWINDOW_H

#include <framework/global.h>
#include <framework/core/inputevent.h>
#include <framework/core/timer.h>
#include <framework/graphics/declarations.h>

//@bindsingleton g_window
class PlatformWindow
{
    enum {
        KEY_PRESS_REPEAT_INTERVAL = 30,
    };

    typedef std::function<void(const Size&)> OnResizeCallback;
    typedef std::function<void(const InputEvent&)> OnInputEventCallback;

public:
    virtual void init() = 0;
    virtual void terminate() = 0;

    virtual void move(const Point& pos) = 0;
    virtual void resize(const Size& size) = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void maximize() = 0;
    virtual void poll() = 0;
    virtual void swapBuffers() = 0;
    virtual void showMouse() = 0;
    virtual void hideMouse() = 0;
    virtual void displayFatalError(const std::string& message) { }

    int loadMouseCursor(const std::string& file, const Point& hotSpot);
    virtual void setMouseCursor(int cursorId) = 0;
    virtual void restoreMouseCursor() = 0;

    virtual void setTitle(const std::string& title) = 0;
    virtual void setMinimumSize(const Size& minimumSize) = 0;
    virtual void setFullscreen(bool fullscreen) = 0;
    virtual void setVerticalSync(bool enable) = 0;
    virtual void setIcon(const std::string& iconFile) = 0;
    virtual void setClipboardText(const std::string& text) = 0;

    virtual Size getDisplaySize() = 0;
    virtual std::string getClipboardText() = 0;
    virtual std::string getPlatformType() = 0;

    int getDisplayWidth() { return getDisplaySize().width(); }
    int getDisplayHeight() { return getDisplaySize().height(); }

    Size getUnmaximizedSize() { return m_unmaximizedSize; }
    Size getSize() { return m_size; }
    Size getMinimumSize() { return m_minimumSize; }
    int getWidth() { return m_size.width(); }
    int getHeight() { return m_size.height(); }
    Point getUnmaximizedPos() { return m_unmaximizedPos; }
    Point getPosition() { return m_position; }
    int getX() { return m_position.x; }
    int getY() { return m_position.y; }
    Point getMousePosition() { return m_inputEvent.mousePos; }
    int getKeyboardModifiers() { return m_inputEvent.keyboardModifiers; }

    bool isKeyPressed(Fw::Key keyCode) { return m_keysState[keyCode]; }
    bool isMouseButtonPressed(Fw::MouseButton mouseButton) { return m_mouseButtonStates[mouseButton]; }
    bool isVisible() { return m_visible; }
    bool isMaximized() { return m_maximized; }
    bool isFullscreen() { return m_fullscreen; }
    bool hasFocus() { return m_focused; }

    void setOnClose(const std::function<void()>& onClose) { m_onClose = onClose; }
    void setOnResize(const OnResizeCallback& onResize) { m_onResize = onResize; }
    void setOnInputEvent(const OnInputEventCallback& onInputEvent) { m_onInputEvent = onInputEvent; }

protected:
    virtual int internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot) = 0;

    void updateUnmaximizedCoords();

    void processKeyDown(Fw::Key keyCode);
    void processKeyUp(Fw::Key keyCode);
    void releaseAllKeys();
    void fireKeysPress();

    std::map<int, Fw::Key> m_keyMap;
    std::map<Fw::Key, stdext::boolean<false>> m_keysState;
    std::map<Fw::Key, ticks_t> m_firstKeysPress;
    std::map<Fw::Key, ticks_t> m_lastKeysPress;
    Timer m_keyPressTimer;

    Size m_size;
    Size m_minimumSize;
    Point m_position;
    Size m_unmaximizedSize;
    Point m_unmaximizedPos;
    InputEvent m_inputEvent;
    stdext::boolean<false> m_mouseButtonStates[4];

    stdext::boolean<false> m_created;
    stdext::boolean<false> m_visible;
    stdext::boolean<false> m_focused;
    stdext::boolean<false> m_fullscreen;
    stdext::boolean<false> m_maximized;

    std::function<void()> m_onClose;
    OnResizeCallback m_onResize;
    OnInputEventCallback m_onInputEvent;
};

extern PlatformWindow& g_window;

#endif
