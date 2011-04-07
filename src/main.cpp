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


#include "framework/engine.h"
#include "framework/configs.h"
#include "framework/resources.h"
#include "framework/platform.h"
#include "menustate.h"

/// Catches signals so we can exit nicely
void signal_handler(int sig)
{
    switch(sig) {
        case SIGTERM:
        case SIGINT:
        {
            static bool stopping = false;
            if(!stopping) {
                stopping = true;
                g_engine.stop();
            }
            break;
        }
    }
}

/// Default configurations
void setDefaultConfigs()
{
    // default size
    int defWidth = 640;
    int defHeight = 480;

    // init on screen center
    g_configs.setValue("window x", (Platform::getDisplayWidth() - defWidth)/2);
    g_configs.setValue("window y", (Platform::getDisplayHeight() - defHeight)/2);

    g_configs.setValue("window width", defWidth);
    g_configs.setValue("window height", defHeight);
    g_configs.setValue("window maximized", false);
}

void saveConfigs()
{
    g_configs.setValue("window x", Platform::getWindowX());
    g_configs.setValue("window y", Platform::getWindowY());
    g_configs.setValue("window width", Platform::getWindowWidth());
    g_configs.setValue("window height", Platform::getWindowHeight());
    g_configs.setValue("window maximized", Platform::isWindowMaximized());
    g_configs.save();
}

int main(int argc, const char *argv[])
{
    // install our signal handler
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    // init platform stuff
    Platform::init("OTClient");

    // init resources
    g_resources.init(argv[0]);
    if(g_resources.setWriteDir(Platform::getAppUserDir()))
        g_resources.addToSearchPath(Platform::getAppUserDir());
    g_resources.addToSearchPath("data");

    // before loading configurations set the default ones
    setDefaultConfigs();

    // load configurations
    if(!g_configs.load("config.yml"))
        logInfo("Could not read configuration file, default configurations will be used.");

    logInfo("OTClient 0.1.0");

    // create the window
    Platform::createWindow(g_configs.getInteger("window x"), g_configs.getInteger("window y"),
                           g_configs.getInteger("window width"), g_configs.getInteger("window height"),
                           640, 480,
                           g_configs.getBoolean("window maximized"));
    Platform::setWindowTitle("OTClient");
    //Platform::setVsync();

    // init engine
    g_engine.init();

    // state scope
    {
        std::shared_ptr<MenuState> menuState(new MenuState);
        g_engine.changeState(menuState.get());

        Platform::showWindow();
        //Platform::hideMouseCursor();
        g_engine.enableFpsCounter();

        // main loop, run everything
        g_engine.run();

        // terminate stuff
        g_engine.terminate();
    }

    // save configurations before exiting
    saveConfigs();

    //Platform::showMouseCursor();
    Platform::terminate();

    // unload resources
    g_resources.terminate();
    return 0;
}
