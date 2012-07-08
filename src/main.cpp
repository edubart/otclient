/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "framework/application.h"
#include "framework/luascript/luainterface.h"
#include "framework/core/resourcemanager.h"
#include "otclient/otclient.h"

int main(int argc, const char* argv[])
{
    std::vector<std::string> args(argv, argv + argc);

    // setup application name and version
    g_app.setName("OTClient");
    g_app.setCompactName("otclient");
    g_app.setVersion("0.5.0_dev");

    // initialize application framework and otclient
    g_app.init("otclient", args);
    g_otclient.init(args);

    // find script init.lua and run it
    g_resources.discoverWorkDir(g_app.getCompactName(), "init.lua");
    if(!g_lua.safeRunScript(g_resources.getWorkDir() + "init.lua"))
        g_logger.fatal("Unable to run script init.lua!");

    // the run application main loop
    g_app.run();

    // unload modules
    g_app.deinit();

    // terminate everything and free memory
    g_otclient.terminate();
    g_app.terminate();
    return 0;
}
