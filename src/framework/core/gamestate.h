/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <prerequisites.h>
#include <core/input.h>

struct InputEvent;

class GameState
{
public:
    GameState() { }
    virtual ~GameState() { }

    /// Fired when enter the state
    virtual void onEnter() = 0;
    /// Fired when leaves the state
    virtual void onLeave() = 0;

    /// Fired when user tries to close the window
    virtual void onClose() = 0;
    /// Fired for every user input event, this is called before processing UI input and if it returns false the input is not passed to the UI
    virtual bool onInputEvent(const InputEvent& event) = 0;
    /// Fired when main window is resized
    virtual void onResize(const Size& size) = 0;

    /// Fired before redering the UI
    virtual void render() = 0;
};

#endif // GAMESTATE_H
