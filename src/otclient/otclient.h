#ifndef OTCLIENT_H
#define OTCLIENT_H

#include <framework/ui/declarations.h>
#include <framework/platform/platformlistener.h>

class OTClient : public PlatformListener
{
public:
    /// Where everything begins...
    void init(std::vector<std::string> args);
    /// Main loop
    void run();
    /// Called when the client is terminating
    void terminate();

    /// Stop running
    void exit();
    /// Poll platform, dispatcher and network events
    void poll();
    /// Render each frame
    void render();

    /// Fired when user tryes to close the window
    void onClose();
    /// Fired when user resize the window
    void onResize(const Size& size);
    /// Fired on an user input event
    void onPlatformEvent(const PlatformEvent& event);

    bool isRunning() const { return m_running; }

    void setOnClose(const SimpleCallback& onCloseCallback) { m_onCloseCallback = onCloseCallback; }
    SimpleCallback getOnClose() const { return m_onCloseCallback; }

private:
    /// Set default configurations and load the user configurations
    void loadConfigurations();
    /// Use the loaded configurations to setup other classes
    void setupConfigurations();
    void saveConfigurations();

    void registerLuaFunctions();

    bool m_running;
    bool m_stopping;

    SimpleCallback m_onCloseCallback;
};

extern OTClient g_client;

#endif
