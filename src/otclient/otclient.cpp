/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#include "otclient.h"

#include <framework/core/modulemanager.h>
#include <framework/core/configs.h>
#include <framework/core/resourcemanager.h>
#include <framework/core/eventdispatcher.h>
#include <framework/luascript/luainterface.h>
#include <framework/platform/platform.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/fontmanager.h>
#include <framework/ui/uimanager.h>
#include <framework/ui/uiwidget.h>
#include <framework/net/connection.h>

#include <otclient/net/protocolgame.h>
#include <otclient/core/game.h>
#include <otclient/core/map.h>

OTClient g_client;

void OTClient::init(std::vector<std::string> args)
{
    m_running = false;
    m_stopping = false;

    // print client information
    logInfo(Otc::AppName, " ", Otc::AppVersion);

    // initialize platform related stuff
    g_platform.init(this, Otc::AppName);

    // initialize script environment
    g_lua.init();

    // register otclient lua functions
    registerLuaFunctions();

    // initialize resources
    g_resources.init(args[0].c_str(), Otc::AppPathName);

    // load configurations
    loadConfigurations();

    // create the client window
    //TODO: bind these functions and move to otclient module
    int minWidth = 550;
    int minHeight = 450;
    int windowX = Fw::fromstring(g_configs.get("window x"), 0);
    int windowY = Fw::fromstring(g_configs.get("window y"), 0);
    int windowWidth = Fw::fromstring(g_configs.get("window width"), minWidth);
    int windowHeight = Fw::fromstring(g_configs.get("window height"), minHeight);
    bool maximized = Fw::fromstring(g_configs.get("window maximized"), false);
    g_platform.createWindow(windowX, windowY, windowWidth, windowHeight, minWidth, minHeight, maximized);
    g_platform.setWindowTitle(Otc::AppName);
    g_platform.setWindowIcon("otclient/otcicon.png");

    // initialize graphics
    g_graphics.init();

    // initialize event dispatcher
    g_dispatcher.init();

    // initialize the ui
    g_ui.init();

    g_game.init();

    // discover and load modules
    g_modules.discoverAndLoadModules();
    g_modules.getModule("otclient")->load();
    ModulePtr tibiaFilesModule = g_modules.getModule("tibiafiles");
    if(!tibiaFilesModule || !tibiaFilesModule->isLoaded()) {
        logFatal("Could not find or load 'tibiafiles' module, this module contains SPR and DAT files.\n"
                 "Please download it and place in modules directory, then try running again.");
    }

    // now that everything is initialized, setup configurations
    setupConfigurations();

    // now that everything is loaded, show the window
    g_platform.showWindow();
}

void OTClient::run()
{
    int frameTicks = g_platform.getTicks();
    int lastPollTicks = frameTicks;
    int frameCount = 0;

    m_stopping = false;
    m_running = true;

    if(g_ui.getRootWidget()->getChildCount() == 0) {
        logError("there is no root widgets to display, the app will close");
        m_stopping = true;
    }

    // run the first poll
    poll();

    while(!m_stopping) {
        //g_platform.sleep(150);
        g_platform.updateTicks();
        frameTicks = g_platform.getTicks();

        // poll events every POLL_CYCLE_DELAY
        // this delay exists to avoid massive polling thus increasing framerate
        if(frameTicks - lastPollTicks >= POLL_CYCLE_DELAY) {
            poll();
            lastPollTicks = frameTicks;
        }

        // only render when the windows is visible
        if(g_platform.isWindowVisible()) {
            // render begin
            g_graphics.beginRender();

            // render everything
            render();

            // render end
            g_graphics.endRender();

            // swap the old buffer with the new rendered
            g_platform.swapBuffers();
        } else {
            // sleeps until next poll to avoid massive cpu usage
            Fw::sleep(POLL_CYCLE_DELAY+1);
        }
    }

    m_stopping = false;
    m_running = false;
}

void OTClient::terminate()
{
    // hide the window because there is no render anymore
    g_platform.hideWindow();

    g_game.terminate();

    // run modules unload event
    g_modules.unloadModules();

    // terminate ui
    g_ui.terminate();

    // release remaining lua object references
    g_lua.collectGarbage();

    // poll remaining events
    poll();

    // terminate network
    Connection::terminate();

    // terminate dispatcher
    g_dispatcher.terminate();

    // terminate graphics
    g_graphics.terminate();

    // save configurations
    saveConfigurations();

    // release resources
    g_resources.terminate();

    // terminate script environment
    g_lua.terminate();

    // end platform related stuff
    g_platform.terminate();

}

void OTClient::exit()
{
    // stops the main loop
    m_stopping = true;
}

void OTClient::poll()
{
    // poll platform events
    g_platform.poll();

    // poll network events
    Connection::poll();

    // poll dispatcher events
    g_dispatcher.poll();
}

void OTClient::render()
{
    // everything is rendered by UI components
    g_ui.render();
}

void OTClient::loadConfigurations()
{
    // default window size
    int defWidth = 550;
    int defHeight = 450;

    // sets default window configuration
    g_configs.set("window x", Fw::tostring((g_platform.getDisplayWidth() - defWidth)/2));
    g_configs.set("window y", Fw::tostring((g_platform.getDisplayHeight() - defHeight)/2));
    g_configs.set("window width", Fw::tostring(defWidth));
    g_configs.set("window height", Fw::tostring(defHeight));
    g_configs.set("window maximized", Fw::tostring(false));
    g_configs.set("vsync", Fw::tostring(true));

    // loads user configuration
    if(!g_configs.load("config.otml"))
        logInfo("Using default configurations.");
}

void OTClient::setupConfigurations()
{
    // activate vertical synchronization?
    bool vsync = Fw::fromstring(g_configs.get("vsync"), true);
    g_platform.setVerticalSync(vsync);
}

void OTClient::saveConfigurations()
{
    g_configs.set("window x", Fw::tostring(g_platform.getWindowX()));
    g_configs.set("window y", Fw::tostring(g_platform.getWindowY()));
    g_configs.set("window width", Fw::tostring(g_platform.getWindowWidth()));
    g_configs.set("window height", Fw::tostring(g_platform.getWindowHeight()));
    g_configs.set("window maximized", Fw::tostring(g_platform.isWindowMaximized()));

    // saves user configuration
    if(!g_configs.save())
        logError("configurations are lost because it couldn't be saved");
}

void OTClient::onClose()
{
    //TODO: make and use g_lua.callGlobalField
    if(m_onCloseCallback)
       m_onCloseCallback();
    else
        exit();
}

void OTClient::onResize(const Size& size)
{
    static bool eventScheduled = false;
    static Size newSize;
    newSize = size;

    // avoid massive resize updates
    if(!eventScheduled) {
        g_dispatcher.addEvent([&] {
            g_graphics.resize(newSize);
            g_ui.resize(newSize);
            eventScheduled = false;
        });
        eventScheduled = true;
    }
}

void OTClient::onPlatformEvent(const PlatformEvent& event)
{
    bool fireUi = true;

    if(event.type == EventKeyDown && event.ctrl && !event.alt && !event.shift && event.keycode == Fw::KeyF8) {
        // TODO: move this events to lua
        UIWidgetPtr console = g_ui.getRootWidget()->getChildById("consolePanel");
        if(!console->isExplicitlyVisible()) {
            console->lock();
            console->show();
        } else {
            console->unlock();
            console->hide();
        }
        fireUi = false;
    }

    if(fireUi)
        g_ui.inputEvent(event);
}
