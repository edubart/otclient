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
#include "fonts.h"
#include "platform.h"
#include "graphics.h"
#include "input.h"
#include "configs.h"
#include "gamestate.h"
#include "net/connections.h"

Engine g_engine;

void Engine::init()
{
    // initialize stuff
    g_graphics.init();
    g_fonts.init();

    // finally show the window
    onResize(Platform::getWindowWidth(), Platform::getWindowHeight());
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
    ulong ticks;
    static ulong lastFrameTicks;

    m_running = true;

    lastFrameTicks = Platform::getTicks();
    while(!m_stopping) {
        // fire platform events
        Platform::poll();
        
        //poll network events
        //debug("%d", g_connections.poll());
        
        // update
        ticks = Platform::getTicks();
        update(ticks, ticks - lastFrameTicks);
        lastFrameTicks = ticks;

        // render only when visible
        //if(Platform::isWindowVisible()) {
            render();

            // swap buffers
            Platform::swapBuffers();
            
            /*
            static ConnectionPtr connection = g_connections.createConnection();
            
            if(connection->getLastError()){
                error("%s", connection->getLastError().message().c_str());
            }
            else{
                if(!connection->isConnecting() && !connection->isConnected()){
                    connection->connect("www.google.com.br", 80);
                }
                
                if(!connection->isConnected()){
                    debug("still not connected.");
                }
            }
            */
        //}
    }

    lastFrameTicks = 0;
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
    g_graphics.endRender();
}

void Engine::update(int ticks, int elapsedTicks)
{
    if(m_currentState)
        m_currentState->update(ticks, elapsedTicks);
}

void Engine::onClose()
{
    if(m_currentState)
        m_currentState->onClose();
}

void Engine::onResize(int width, int height)
{
    g_graphics.resize(width, height);
}

void Engine::onInputEvent(InputEvent *event)
{
    if(m_currentState)
        m_currentState->onInputEvent(event);
}
