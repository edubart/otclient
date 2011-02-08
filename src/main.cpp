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
#include "framework/configmanager.h"
#include "framework/resourcemanager.h"
#include "framework/platform.h"
#include "menustate.h"

/// Catches signals so we can exit nicely
void signal_handler(int sig)
{
    switch(sig) {
        case SIGTERM:
        case SIGINT:
        case SIGQUIT:
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
    g_config.setValue("width", 640);
    g_config.setValue("height", 480);
}

int main(int argc, const char *argv[])
{
    // install our signal handler
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    // setup resources
    g_resources.init(argv[0]);
    if(g_resources.setWriteDir(Platform::getAppUserDir()))
        g_resources.addToSearchPath(Platform::getAppUserDir());
    g_resources.addToSearchPath("data");

    // before loading configurations set the default ones
    setDefaultConfigs();

    // load configurations
    if(!g_config.load("config.yml"))
        notice("Could not read configuration file, default configurations will be used.");

    notice(APP_LONGNAME);

    // setup the engine
    g_engine.init();

    // create initial state
    boost::scoped_ptr<MenuState> menuState(new MenuState);
    g_engine.changeState(menuState.get());

    // run
    g_engine.run();

    // terminate stuff
    g_engine.terminate();

    // save configurations before exiting
    g_config.save();

    // unload resources
    g_resources.terminate();
    return 0;
}
