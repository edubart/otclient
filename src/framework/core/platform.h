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


#ifndef PLATFORM_H
#define PLATFORM_H

#include <prerequisites.h>

namespace Platform
{
    void init(const char *appName);
    void terminate();

    /// Poll platform input/window events
    void poll();

    /// Get current time in milliseconds since init
    int getTicks();
    /// Sleep in current thread
    void sleep(ulong miliseconds);

    bool createWindow(int x, int y, int width, int height, int minWidth, int minHeight, bool maximized);
    void destroyWindow();
    void showWindow();
    void setWindowTitle(const char *title);
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
    void *getExtensionProcAddress(const char *ext);
    /// Check if GL extension is supported
    bool isExtensionSupported(const char *ext);

    const char *getClipboardText();
    void setClipboardText(const char *text);

    void hideMouseCursor();
    void showMouseCursor();

    /// Enable/disable vertical synchronization
    void setVsync(bool enable = true);
    /// Swap GL buffers
    void swapBuffers();

    /// Get the app user directory, the place to save files configurations files
    std::string getAppUserDir();
}

#endif // PLATFORM_H
