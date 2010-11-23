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
#include "platform.h"
#include "graphics.h"
#include "const.h"
#include "input.h"
#include "configmanager.h"
#include "logger.h"

Engine g_engine;

Engine::Engine() :
    m_stopping(false),
    m_running(false),
    m_lastFrameTicks(0)
{
}

Engine::~Engine()
{
}

void Engine::init()
{
    Platform::init();

    int width = g_config.getInteger("width");
    int height = g_config.getInteger("height");

    // create the window
    Platform::createWindow(width, height, 550, 450);
    Platform::setWindowTitle(APP_NAME);
    Platform::setVsync();

    // initialize graphics stuff
    g_graphics.init();

    // finally show the window
    onResize(width, height);
    Platform::showWindow();
    Platform::hideMouseCursor();
}

void Engine::terminate()
{
    // save configs
    g_config.setValue("width", Platform::getWindowWidth());
    g_config.setValue("height", Platform::getWindowHeight());

    Platform::showMouseCursor();
    Platform::terminate();
    g_graphics.terminate();
}

void Engine::run()
{
    unsigned long ticks;

    m_running = true;
    m_lastFrameTicks = Platform::getTicks();

    while(!m_stopping) {
        // fire platform events
        Platform::poll();

        // update
        ticks = Platform::getTicks();
        update(ticks - m_lastFrameTicks);
        m_lastFrameTicks = ticks;

        // render
        render();

        // swap buffers
        Platform::swapBuffers();
    }

    m_lastFrameTicks = 0;
    m_stopping = false;
    m_running = false;
}

void Engine::stop()
{
    m_stopping = true;
}

void Engine::render()
{
    g_graphics.beginRender();
    g_graphics.endRender();
}

void Engine::update(int elapsedTicks)
{

}

void Engine::onClose()
{
    stop();
}

void Engine::onResize(int width, int height)
{
    g_graphics.resize(width, height);
}

void Engine::onInputEvent(InputEvent *event)
{

}
