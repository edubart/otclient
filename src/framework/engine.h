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


#ifndef ENGINE_H
#define ENGINE_H

#include "prerequisites.h"

struct InputEvent;

class GameState;

class Engine
{
public:
    Engine() : m_stopping(false),
               m_running(false),
               m_currentState(NULL) { }

    void init();
    void terminate();

    /// Main loop
    void run();

    /// Stops main loop
    void stop();

    /// Change current game state
    void changeState(GameState *newState);

    bool isRunning() const { return m_running; }
    bool isStopping() const { return m_stopping; }

    /// Fired by platform on window close
    void onClose();
    /// Fired by platform on window resize
    void onResize(int width, int height);
    /// Fired by platform on mouse/keyboard input
    void onInputEvent(InputEvent *event);

private:
    /// Called to render every frame
    void render();
    /// Called between renders
    void update(int ticks, int elapsedTicks);

    bool m_stopping;
    bool m_running;

    GameState *m_currentState;
};

extern Engine g_engine;

#endif // ENGINE_H

