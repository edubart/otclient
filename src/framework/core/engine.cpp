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


#include "engine.h"
#include "graphics/fonts.h"
#include "platform.h"
#include "graphics/graphics.h"
#include "configs.h"
#include "dispatcher.h"
#include "net/connections.h"
#include "ui/uicontainer.h"

Engine g_engine;

void Engine::init()
{
    // initialize stuff
    g_graphics.init();
    g_fonts.init();
}

void Engine::terminate()
{
    // force last state exit
    changeState(NULL);

    // terminate stuff
    g_fonts.terminate();
    g_graphics.terminate();
}

void Engine::run()
{
    int ticks = Platform::getTicks();
    int lastFpsTicks = ticks;
    int frameCount = 0;
    int fps = 0;
    m_running = true;

    while(!m_stopping) {
        // poll platform events
        Platform::poll();

        // poll network events
        g_connections.poll();

        ticks = Platform::getTicks();

        // poll diaptcher tasks
        g_dispatcher.poll(ticks);

        // render only when visible
        if(Platform::isWindowVisible()) {
            // calculate and fps
            if(m_calculateFps) {
                frameCount++;
                if(ticks - lastFpsTicks >= 1000) {
                    lastFpsTicks = ticks;
                    fps = frameCount;
                    frameCount = 0;
                }
            }

            render();

            // render fps
            if(m_calculateFps) {
                std::string fpsText = format("FPS: %d", fps);
                Size textSize = g_defaultFont->calculateTextRectSize(fpsText);
                g_defaultFont->renderText(fpsText, Point(g_graphics.getScreenSize().width() - textSize.width() - 10, 10));
            }

            // swap buffers
            Platform::swapBuffers();
        }
    }

    m_stopping = false;
    m_running = false;
}

void Engine::stop()
{
    m_stopping = true;
}

void Engine::changeState(GameState* newState)
{
    if(m_currentState)
        m_currentState->onLeave();
    m_currentState = newState;
    if(m_currentState)
        m_currentState->onEnter();
}

void Engine::render()
{
    g_graphics.beginRender();
    if(m_currentState)
        m_currentState->render();
    g_ui->render();
    g_graphics.endRender();
}

void Engine::onClose()
{
    if(m_currentState)
        m_currentState->onClose();
}

void Engine::onResize(const Size& size)
{
    g_graphics.resize(size);
    g_ui->setSize(size);

    if(m_currentState)
        m_currentState->onResize(size);
}

void Engine::onInputEvent(const InputEvent& event)
{
    // inputs goest to gui first
    if(!g_ui->onInputEvent(event)) {
        // if gui didnt capture the input then goes to the state
        if(m_currentState)
            m_currentState->onInputEvent(event);
    }
}
