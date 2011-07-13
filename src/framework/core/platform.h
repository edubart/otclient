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
