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
#include <core/configs.h>
#include <core/resources.h>
#include <core/platform.h>
#include <core/dispatcher.h>
#include <ui/uiskins.h>
#include <script/luascript.h>
#include <ui/uicontainer.h>

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
                g_engine.onClose();
            }
            break;
        }
    }
}

/// Default configurations
void setDefaultConfigs()
{
    // default size
    int defWidth = 550;
    int defHeight = 450;

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

#ifdef WIN32_NO_CONSOLE
#include <windows.h>
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument,int nCmdShow)
{
    std::vector<std::string> args;
    boost::split(args, lpszArgument, boost::is_any_of(" "));
#else
int main(int argc, const char *argv[])
{
    std::vector<std::string> args;
    for(int i=0;i<argc;++i)
        args.push_back(argv[i]);
#endif

    // install our signal handler
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    // init platform stuff
    Platform::init("OTClient");

    // init random numbers
    std::srand(std::time(NULL));

    // init resources
    g_resources.init(args[0].c_str());
    if(g_resources.setWriteDir(Platform::getAppUserDir()))
        g_resources.addToSearchPath(Platform::getAppUserDir());
    g_resources.addToSearchPath("data");

    // before loading configurations set the default ones
    setDefaultConfigs();

    // load configurations
    if(!g_configs.load("config.yml"))
        logInfo("Could not read configuration file, default configurations will be used.");

    logInfo("OTClient 0.2.0");

    // create the window
    Platform::createWindow(g_configs.getInteger("window x"), g_configs.getInteger("window y"),
                           g_configs.getInteger("window width"), g_configs.getInteger("window height"),
                           550, 450,
                           g_configs.getBoolean("window maximized"));
    Platform::setWindowTitle("OTClient");
    //Platform::setVsync();

    // init engine
    g_engine.init();
    g_engine.enableFpsCounter();

    // load ui skins
    g_uiSkins.load("skins/lightness.yml");

    // load script modules
    g_lua.loadAllModules();

    Platform::showWindow();

    // main loop, run everything
    g_engine.run();

    // terminate stuff
    g_engine.terminate();

    g_uiSkins.terminate();

    // save configurations before exiting
    saveConfigs();

    Platform::terminate();

    // unload resources
    g_resources.terminate();
    return 0;
}
