#ifndef PLATFORM_H
#define PLATFORM_H

#include <global.h>

class Platform
{
public:
    static void init(const char *appName);
    static void terminate();

    /// Poll platform input/window events
    static void poll();

    /// Get current time in milliseconds since init
    static int getTicks();
    /// Sleep in current thread
    static void sleep(ulong miliseconds);

    static bool createWindow(int x, int y, int width, int height, int minWidth, int minHeight, bool maximized);
    static void destroyWindow();
    static void showWindow();
    static void setWindowTitle(const char *title);
    static bool isWindowFocused();
    static bool isWindowVisible();
    static int getWindowX();
    static int getWindowY();
    static int getWindowWidth();
    static int getWindowHeight();
    static bool isWindowMaximized();

    static int getDisplayHeight();
    static int getDisplayWidth();

    /// Get GL extension function address
    static void *getExtensionProcAddress(const char *ext);
    /// Check if GL extension is supported
    static bool isExtensionSupported(const char *ext);

    static const char *getClipboardText();
    static void setClipboardText(const char *text);

    static void hideMouseCursor();
    static void showMouseCursor();

    /// Enable/disable vertical synchronization
    static void setVsync(bool enable = true);
    /// Swap GL buffers
    static void swapBuffers();

    /// Get the app user directory, the place to save files configurations files
    static std::string getAppUserDir();
};

#endif // PLATFORM_H
