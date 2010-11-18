#include "engine.h"
#include "const.h"
#include "logger.h"

#include <csignal>

// catches terminate signals to exit nicely
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

int main(int argc, const char *argv[])
{
    // install our signal handler
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    notice(APP_LONGNAME);

    // setup the engine and run
    g_engine.init();
    g_engine.run();
    g_engine.terminate();
    return 0;
}
