#ifndef PLATFORM_H
#define PLATFORM_H

#include <framework/global.h>

class PlatformListener;

class Platform
{
public:
    void init(PlatformListener* platformListener, const char* appName);
    void terminate();

    /// Poll platform input/window events
    void poll();

    void updateTicks();

    /// Get current time in milliseconds since last application init
    int getTicks() { return m_lastTicks; }

    /// Sleep in current thread
    void sleep(ulong ms);

    bool createWindow(int x, int y, int width, int height, int minWidth, int minHeight, bool maximized);
    void destroyWindow();
    void showWindow();
    void hideWindow();
    void setWindowTitle(const char* title);
    bool isWindowFocused();
    bool isWindowVisible();
    int getWindowX();
    int getWindowY();
    int getWindowWidth();
    int getWindowHeight();
    bool isWindowMaximized();

    int getDisplayHeight();
    int getDisplayWidth();

    /// Get GL extension function address
    void* getExtensionProcAddress(const char* ext);
    /// Check if GLX/WGL extension is supported
    bool isExtensionSupported(const char* ext);

    /// Get text from Ctrl+c
    const char* getClipboardText();
    /// Set text for Ctrl+v
    void setClipboardText(const char* text);

    void hideMouseCursor();
    void showMouseCursor();

    /// Enable or disable vertical synchronization
    void setVerticalSync(bool enable);
    /// Swap GL buffers
    void swapBuffers();

    /// Get the app user directory, the place to save files configurations files
    std::string getAppUserDir();

private:
    PlatformListener* m_listener;
    int m_lastTicks;
};

extern Platform g_platform;

#endif
