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

#include "otclient.h"
#include <framework/core/modulemanager.h>
#include <framework/core/resourcemanager.h>
#include <framework/graphics/graphics.h>
#include "core/game.h"
#include "core/map.h"

OTClient::OTClient() : Application(Otc::AppCompactName)
{
    m_appVersion = Otc::AppVersion;
}

void OTClient::init(const std::vector<std::string>& args)
{
    std::string startupOptions;
    for(uint i=1;i<args.size();++i) {
        const std::string& arg = args[i];
        startupOptions += " ";
        startupOptions += arg;

        if(g_graphics.parseOption(arg))
            continue;

        if(arg == "-version" || arg == "--version" || arg == "-v") {
            Fw::print(Otc::AppName, " ", Otc::AppVersion, "\n"
                      "Buitt on: ", BUILD_DATE, "\n",
                      "Revision: ", BUILD_REVISION, "\n",
                      "Compiled by: ", BUILD_COMPILER, "\n",
                      "Build type: ", BUILD_TYPE, "\n");
            return;
        } else if(arg == "-help" || arg == "--help" || arg == "-h") {
            Fw::print("Usage: ", args[0], " [options]\n"
                      "Options:\n"
                      "  -help                   Display this information and exit\n"
                      "  -version                Display version and exit\n"
                      "\n"
                      "  -no-fbos                Disable usage of opengl framebuffer objects\n"
                      "  -no-mipmapping          Disable texture mipmaping\n"
                      "  -no-smoothing           Disable texture smoothing (bilinear filter)\n"
                      "  -no-hardware-buffering  Disable buffering vertex arrays in hardware\n"
                      "  -realtime-mipmapping    Improve framebuffer smoothing quality by\n"
                      "                          generating mipmaps in realtime when hardware\n"
                      "                          mipmap generation implementation is available\n");
            return;
        } else {
            Fw::println("Unrecognized option '", arg, "', please see -help for available options list");
            return;
        }
    }

    logInfo(Fw::formatString("%s %s (rev %s) built on %s",
                             Otc::AppName,
                             Otc::AppVersion,
                             BUILD_REVISION,
                             BUILD_DATE));
    if(startupOptions.length() > 0)
        logInfo("Startup options:", startupOptions);

    g_logger.setLogFile(Fw::formatString("%s.txt", Otc::AppCompactName));
    Application::init(args);

    g_modules.discoverModules();

    // core modules 0-99
    g_modules.autoLoadModules(99);
    g_modules.ensureModuleLoaded("core_lib");
    // client modules 100-499
    g_modules.autoLoadModules(499);
    g_modules.ensureModuleLoaded("client");
    // game modules 500-999
    g_modules.autoLoadModules(999);
    g_modules.ensureModuleLoaded("game");
    // addons 1000-9999
    g_modules.autoLoadModules(9999);

    // load otclientrc.lua
    if(g_resources.fileExists("/otclientrc.lua")) {
        try {
            g_lua.runScript("/otclientrc.lua");
        } catch(LuaException& e) {
            logError("failed to load otclientrc.lua: ", e.what());
        }
    }
}
