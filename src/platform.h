#ifndef PLATFORM_H
#define PLATFORM_H

namespace Platform
{
    void init();
    void terminate();

    void poll();

    unsigned long getTicks();
    void sleep(unsigned long miliseconds);

    bool createWindow(int width, int height, int minWidth, int minHeight);
    void destroyWindow();
    void showWindow();
    void setWindowTitle(const char *title);
    bool isWindowFocused();
    bool isWindowVisible();

    void *getExtensionProcAddress(const char *ext);
    bool isExtensionSupported(const char *ext);

    const char *getTextFromClipboard();
    void copyToClipboard(const char *text);

    void hideMouseCursor();
    void showMouseCursor();

    void setVsync(bool enable = true);
    void swapBuffers();
};

#endif // PLATFORM_H
