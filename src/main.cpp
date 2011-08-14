#include "otclient.h"

#include <csignal>
#include <core/eventdispatcher.h>

void signal_handler(int sig)
{
    static bool signaled = false;
    switch(sig) {
        case SIGTERM:
        case SIGINT:
            if(!signaled) {
                signaled = true;
                g_dispatcher.addEvent(std::bind(&OTClient::onClose, &g_client));
            }
            break;
    }
}

#ifdef WIN32_NO_CONSOLE
#include <windows.h>
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    std::vector<std::string> args;
    boost::split(args, lpszArgument, boost::is_any_of(std::string(" ")));
#else
int main(int argc, const char* argv[])
{
    std::vector<std::string> args;
    for(int i=0; i<argc; ++i)
        args.push_back(argv[i]);
#endif
    g_client.init(args);
    g_client.run();
    g_client.terminate();
    return 0;
}
