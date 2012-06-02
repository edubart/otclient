/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "application.h"
#include <framework/core/clock.h>
#include <framework/core/resourcemanager.h>
#include <framework/core/modulemanager.h>
#include <framework/core/eventdispatcher.h>
#include <framework/core/configmanager.h>
#include <framework/net/connection.h>
#include <framework/platform/platformwindow.h>
#include <framework/ui/uimanager.h>
#include <framework/ui/uiwidget.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/particlemanager.h>
#include <framework/graphics/painter.h>
#include <framework/sound/soundmanager.h>
#include <framework/luascript/luainterface.h>
#include <framework/platform/crashhandler.h>

Application *g_app = nullptr;

void exitSignalHandler(int sig)
{
    static bool signaled = false;
    switch(sig) {
        case SIGTERM:
        case SIGINT:
            if(!signaled) {
                signaled = true;
                g_eventDispatcher.addEvent(std::bind(&Application::close, g_app));
            }
            break;
    }
}

Application::Application(const std::string& appName)
{
    g_app = this;
    m_appName = appName;
    m_foregroundFrameCounter.setMaxFps(60);
}

Application::~Application()
{
    g_app = nullptr;
}

void Application::init(const std::vector<std::string>& args)
{
    // capture exit signals
    signal(SIGTERM, exitSignalHandler);
    signal(SIGINT, exitSignalHandler);

#ifdef CRASH_HANDLER
    installCrashHandler();
#endif

    // initialize lua
    g_lua.init();
    registerLuaFunctions();

    // initialize resources
    g_resources.init(args[0].c_str());

    // setup configs write directory
    if(!g_resources.setupWriteDir(m_appName))
        g_logger.error("Could not setup write directory");

    // load configs
    if(!g_configs.load("/config.otml"))
        g_logger.info("Using default configurations.");

    // setup platform window
    g_ui.init();

    g_window.init();
    g_window.hide();
    g_window.setOnResize(std::bind(&Application::resize, this, std::placeholders::_1));
    g_window.setOnInputEvent(std::bind(&Application::inputEvent, this, std::placeholders::_1));
    g_window.setOnClose(std::bind(&Application::close, this));

    // initialize graphics
    g_graphics.init();

    // initialize sound
    g_sounds.init();

    // fire first resize
    resize(g_window.getSize());

    // display window when the application starts running
    //g_eventDispatcher.addEvent([]{ g_window.show(); });

    g_modules.discoverModulesPath();

    m_initialized = true;
}

void Application::terminate()
{
    if(!m_initialized)
        return;

    g_lua.callGlobalField("g_app", "onTerminate");

    // hide the window because there is no render anymore
    g_window.hide();

    // run modules unload events
    g_modules.unloadModules();

    // release remaining lua object references
    g_lua.collectGarbage();

    // poll remaining events
    poll();

    // terminate network
    Connection::terminate();

    // terminate graphics
    g_ui.terminate();
    g_window.terminate();

    // terminate sound
    g_sounds.terminate();

    // flush remaining dispatcher events
    g_eventDispatcher.flush();

    // save configurations
    g_configs.save();

    // release resources
    g_resources.terminate();

    // terminate script environment
    g_lua.terminate();

    m_foreground = nullptr;

    g_logger.info("Application ended successfully.");
}

void Application::run()
{
    if(!m_initialized)
        return;

    m_stopping = false;
    m_running = true;

    // run the first poll
    poll();

    g_lua.callGlobalField("g_app", "onRun");

    // first clock update
    g_clock.update();

    while(!m_stopping) {
        // poll all events before rendering
        poll();

        if(g_window.isVisible()) {
            // the otclient's screen consists of two panes
            // background pane - high updated and animated pane (where the game are stuff happens)
            // foreground pane - steady pane with few animated stuff (UI)
            bool redraw = false;
            bool updateForeground = false;

            if(m_backgroundFrameCounter.shouldProcessNextFrame()) {
                redraw = true;

                if(m_mustRepaint || m_foregroundFrameCounter.shouldProcessNextFrame()) {
                    m_mustRepaint = false;
                    updateForeground = true;
                }
            }

            if(redraw) {
                g_graphics.beginRender();

                Rect viewportRect(0, 0, g_graphics.getViewportSize());

                // draw the foreground into a texture
                if(updateForeground) {
                    m_foregroundFrameCounter.processNextFrame();

                    // draw foreground
                    g_ui.render(true);

                    // copy the foreground to a texture
                    m_foreground->copyFromScreen(viewportRect);
                }

                // draw background (animated stuff)
                m_backgroundFrameCounter.processNextFrame();
                g_ui.render(false);

                // draw the foreground (steady stuff)
                g_painter->setColor(Color::white);
                g_painter->setOpacity(1.0);
                g_painter->drawTexturedRect(viewportRect, m_foreground, viewportRect);

                g_graphics.endRender();

                // update screen pixels
                g_window.swapBuffers();
            }

            // only update the current time once per frame to gain performance
            g_clock.update();

            m_backgroundFrameCounter.update();
            m_foregroundFrameCounter.update();

            int sleepMicros = std::min(m_backgroundFrameCounter.getMaximumSleepMicros(), m_foregroundFrameCounter.getMaximumSleepMicros());
            if(sleepMicros >= AdaptativeFrameCounter::MINIMUM_MICROS_SLEEP) {
                stdext::microsleep(AdaptativeFrameCounter::MINIMUM_MICROS_SLEEP);
            }

        } else {
            // sleeps until next poll to avoid massive cpu usage
            stdext::millisleep(POLL_CYCLE_DELAY+1);
            g_clock.update();
        }
    }

    m_stopping = false;
    m_running = false;
}

void Application::exit()
{
    g_logger.info("Exiting application..");
    m_stopping = true;
}

void Application::poll()
{
    g_sounds.poll();

    // poll input events
    g_window.poll();
    //g_particleManager.update();

    Connection::poll();
    g_eventDispatcher.poll();
}

void Application::close()
{
    m_onInputEvent = true;
    if(!g_lua.callGlobalField<bool>("g_app", "onClose"))
        exit();
    m_onInputEvent = false;
}

void Application::resize(const Size& size)
{
    m_onInputEvent = true;
    g_graphics.resize(size);
    g_ui.resize(size);
    m_onInputEvent = false;

    m_foreground = TexturePtr(new Texture(size.width(), size.height()));
    m_foreground->setUpsideDown(true);
    m_mustRepaint = true;
}

void Application::inputEvent(const InputEvent& event)
{
    m_onInputEvent = true;
    g_ui.inputEvent(event);
    m_onInputEvent = false;
}
