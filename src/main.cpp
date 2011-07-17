#include <global.h>
#include <core/engine.h>
#include <core/configs.h>
#include <core/resources.h>
#include <core/platform.h>
#include <core/dispatcher.h>
#include <ui/uiskins.h>
#include <script/scriptcontext.h>
#include <ui/uicontainer.h>

#include <csignal>

void signal_handler(int sig)
{
    static bool stopping = false;
    switch(sig) {
        case SIGTERM:
        case SIGINT:
            if(!stopping) {
                stopping = true;
                g_engine.onClose();
            }
            break;
    }
}

void loadDefaultConfigs()
{
    // default size
    int defWidth = 550;
    int defHeight = 450;

    g_configs.set("window x", (Platform::getDisplayWidth() - defWidth)/2);
    g_configs.set("window y", (Platform::getDisplayHeight() - defHeight)/2);
    g_configs.set("window width", defWidth);
    g_configs.set("window height", defHeight);
    g_configs.set("window maximized", false);
    g_configs.set("skin", "tibia");
}

void saveConfigs()
{
    g_configs.set("window x", Platform::getWindowX());
    g_configs.set("window y", Platform::getWindowY());
    g_configs.set("window width", Platform::getWindowWidth());
    g_configs.set("window height", Platform::getWindowHeight());
    g_configs.set("window maximized", Platform::isWindowMaximized());
    g_configs.save();
}

#ifdef WIN32_NO_CONSOLE
#include <windows.h>
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    std::vector<std::string> args;
    boost::split(args, lpszArgument, boost::is_any_of(std::string(" ")));
#else
int main(int argc, const char *argv[])
{
    std::vector<std::string> args;
    for(int i=0;i<argc;++i)
        args.push_back(argv[i]);
#endif

    logInfo("OTClient 0.2.0");

    // install exit signal handler
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    // init platform stuff
    Platform::init("OTClient");

    // load resources paths
    g_resources.init(args[0].c_str());

    // load configurations
    loadDefaultConfigs();
    if(!g_configs.load("config.otml"))
        logInfo("Could not read configuration file, default configurations will be used.");

    // create the window
    Platform::createWindow(g_configs.get("window x"), g_configs.get("window y"),
                           g_configs.get("window width"), g_configs.get("window height"),
                           550, 450,
                           g_configs.get("window maximized"));
    Platform::setWindowTitle("OTClient");
    //Platform::setVsync();

    // init engine
    g_engine.init();
    g_engine.enableFpsCounter();

    // load ui skins
    g_uiSkins.load("tibiaskin");

    // load script modules
    g_lua.loadAllModules();

    Platform::showWindow();

    // main loop, run everything
    g_engine.run();

    // terminate stuff
    g_engine.terminate();
    g_uiSkins.terminate();
    saveConfigs();
    Platform::terminate();
    g_resources.terminate();
    return 0;
}
