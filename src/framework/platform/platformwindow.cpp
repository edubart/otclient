/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#include "platformwindow.h"

#ifdef _WIN32
#include "win32window.h"
WIN32Window window;
#else
#include "x11window.h"
#include <framework/core/clock.h>
X11Window window;
#endif

#include <framework/core/clock.h>
#include <framework/graphics/image.h>

PlatformWindow& g_window = window;

int PlatformWindow::loadMouseCursor(const std::string& file, const Point& hotSpot)
{
    ImagePtr image = Image::load(file);

    if(!image) {
        g_logger.traceError(stdext::format("unable to load cursor image file %s", file));
        return -1;
    }

    if(image->getBpp() != 4) {
        g_logger.error("the cursor image must have 4 channels");
        return -1;
    }

    if(image->getWidth() != 32 || image->getHeight() != 32) {
        g_logger.error("the cursor image must have 32x32 dimension");
        return -1;
    }

    return internalLoadMouseCursor(image, hotSpot);
}

void PlatformWindow::updateUnmaximizedCoords()
{
    if(!isMaximized() && !isFullscreen()) {
        m_unmaximizedPos = m_position;
        m_unmaximizedSize = m_size;
    }
}

void PlatformWindow::processKeyDown(Fw::Key keyCode)
{
    if(keyCode == Fw::KeyUnknown)
        return;

    if(keyCode == Fw::KeyCtrl) {
        m_inputEvent.keyboardModifiers |= Fw::KeyboardCtrlModifier;
        return;
    } else if(keyCode == Fw::KeyAlt) {
        m_inputEvent.keyboardModifiers |= Fw::KeyboardAltModifier;
        return;
    } else if(keyCode == Fw::KeyShift) {
        m_inputEvent.keyboardModifiers |= Fw::KeyboardShiftModifier;
        return;
    }

    if(m_keysState[keyCode])
        return;

    m_keysState[keyCode] = true;
    m_lastKeysPress[keyCode] = -1;

    m_inputEvent.reset(Fw::KeyDownInputEvent);
    m_inputEvent.type = Fw::KeyDownInputEvent;
    m_inputEvent.keyCode = keyCode;

    if(m_onInputEvent) {
        m_onInputEvent(m_inputEvent);

        m_inputEvent.reset(Fw::KeyPressInputEvent);
        m_inputEvent.keyCode = keyCode;
        m_lastKeysPress[keyCode] = g_clock.millis();
        m_firstKeysPress[keyCode] = g_clock.millis();
        m_onInputEvent(m_inputEvent);
    }
}

void PlatformWindow::processKeyUp(Fw::Key keyCode)
{
    if(keyCode == Fw::KeyUnknown)
        return;

    if(keyCode == Fw::KeyCtrl) {
        m_inputEvent.keyboardModifiers &= ~Fw::KeyboardCtrlModifier;
        return;
    } else if(keyCode == Fw::KeyAlt) {
        m_inputEvent.keyboardModifiers &= ~Fw::KeyboardAltModifier;
        return;
    } else if(keyCode == Fw::KeyShift) {
        m_inputEvent.keyboardModifiers &= ~Fw::KeyboardShiftModifier;
        return;
    } else if(keyCode == Fw::KeyNumLock) {
        for(uchar k = Fw::KeyNumpad0; k <= Fw::KeyNumpad9; ++k) {
            if(m_keysState[(Fw::Key)k])
                processKeyUp((Fw::Key)k);
        }
    }

    if(!m_keysState[keyCode])
        return;

    m_keysState[keyCode] = false;

    if(m_onInputEvent) {
        m_inputEvent.reset(Fw::KeyUpInputEvent);
        m_inputEvent.keyCode = keyCode;
        m_onInputEvent(m_inputEvent);
    }
}

void PlatformWindow::releaseAllKeys()
{
    for(auto it : m_keysState) {
        Fw::Key keyCode = it.first;
        bool pressed = it.second;

        if(!pressed)
            continue;

        processKeyUp(keyCode);
    }

    m_inputEvent.keyboardModifiers = 0;

    for(int i=0;i<4;++i)
        m_mouseButtonStates[i] = false;
}

void PlatformWindow::fireKeysPress()
{
    // avoid massive checks
    if(m_keyPressTimer.ticksElapsed() < 10)
        return;
    m_keyPressTimer.restart();

    for(auto it : m_keysState) {
        Fw::Key keyCode = it.first;
        bool pressed = it.second;

        if(!pressed)
            continue;

        ticks_t lastPressTicks = m_lastKeysPress[keyCode];
        ticks_t firstKeyPress = m_firstKeysPress[keyCode];
        if(g_clock.millis() - lastPressTicks >= KEY_PRESS_REPEAT_INTERVAL) {
            if(m_onInputEvent) {
                m_inputEvent.reset(Fw::KeyPressInputEvent);
                m_inputEvent.keyCode = keyCode;
                m_inputEvent.autoRepeatTicks = g_clock.millis() - firstKeyPress;
                m_onInputEvent(m_inputEvent);
            }
            m_lastKeysPress[keyCode] = g_clock.millis();
        }
    }
}

