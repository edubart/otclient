/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#ifndef OTCLIENT_H
#define OTCLIENT_H

#include <framework/ui/declarations.h>
#include <framework/platform/platformlistener.h>

class OTClient : public PlatformListener
{
    enum {
        POLL_CYCLE_DELAY = 10
    };

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

    static void registerLuaFunctions();

private:
    /// Set default configurations and load the user configurations
    void loadConfigurations();
    /// Use the loaded configurations to setup other classes
    void setupConfigurations();
    void saveConfigurations();

    bool m_running;
    bool m_stopping;

    SimpleCallback m_onCloseCallback;
};

extern OTClient g_client;

#endif
