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


#include <prerequisites.h>
#include <core/engine.h>
#include <core/platform.h>
#include <core/dispatcher.h>
#include <graphics/graphics.h>
#include <graphics/fonts.h>
#include <ui/uicontainer.h>
#include <net/connection.h>
#include <script/luascript.h>

Engine g_engine;

void Engine::init()
{
    // initialize stuff
    g_graphics.init();
    g_fonts.init("tibia-12px-rounded");
}

void Engine::terminate()
{
    // terminate stuff
    g_fonts.terminate();
    g_graphics.terminate();
}

void Engine::poll()
{
    // poll platform events
    Platform::poll();

    // poll diaptcher tasks
    g_dispatcher.poll();

    // poll network events
    Connection::poll();
}

void Engine::run()
{
    // check if root container has elements
    const UIContainerPtr& rootContainer = UIContainer::getRoot();
    if(rootContainer->getChildCount() == 0)
        logFatal("FATAL ERROR: no ui loaded at all, no reason to continue running");

    std::string fpsText;
    Size fpsTextSize;
    Font *defaultFont = g_fonts.getDefaultFont();

    m_lastFrameTicks = Platform::getTicks();
    int lastFpsTicks = m_lastFrameTicks;
    int frameCount = 0;
    int fps = 0;
    m_running = true;

    while(!m_stopping) {
        m_lastFrameTicks = Platform::getTicks();

        poll();

        // render only when visible
        if(Platform::isWindowVisible()) {
            // calculate and fps
            if(m_calculateFps) {
                frameCount++;
                if(m_lastFrameTicks - lastFpsTicks >= 1000) {
                    lastFpsTicks = m_lastFrameTicks;
                    fps = frameCount;
                    frameCount = 0;

                    // update fps text
                    fpsText = f("FPS: %d", fps);
                    fpsTextSize = defaultFont->calculateTextRectSize(fpsText);
                }
            }

            // render
            g_graphics.beginRender();

            rootContainer->render();

            // render fps
            if(m_calculateFps)
                defaultFont->renderText(fpsText, Point(g_graphics.getScreenSize().width() - fpsTextSize.width() - 10, 10));

            g_graphics.endRender();

            // swap buffers
            Platform::swapBuffers();
        }
    }

    m_stopping = false;
    m_running = false;

    g_lua.collectGarbage();

    // destroy root ui
    rootContainer->destroy();

    // poll remaning events
    g_engine.poll();
}

void Engine::stop()
{
    m_stopping = true;
}

void Engine::onClose()
{
    g_dispatcher.addTask(boost::bind(&LuaScript::callModuleField, &g_lua, "App", "onClose"));
}

void Engine::onResize(const Size& size)
{
    g_graphics.resize(size);
    UIContainer::getRoot()->setSize(size);
}

void Engine::onInputEvent(const InputEvent& event)
{
    UIContainer::getRoot()->onInputEvent(event);
}
