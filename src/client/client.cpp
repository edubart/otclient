/*
 * Copyright (c) 2010-2015 OTClient <https://github.com/edubart/otclient>
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

#include "client.h"
#include "game.h"
#include "map.h"
#include "shadermanager.h"
#include "spritemanager.h"
#include "minimap.h"
#include <framework/luaengine/luainterface.h>
#include <framework/core/application.h>
#include <framework/core/modulemanager.h>
#include <framework/core/resourcemanager.h>
#include <framework/core/configmanager.h>
#include <framework/graphics/graphics.h>

Client::Client(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);
    initAppFrameworkAndOTClient(args);
}

void Client::initAppFrameworkAndOTClient(std::vector<std::string>& args)
{
    setupAppNameAndVersion();

    g_app.init(args);
    g_map.init();
    g_minimap.init();
    g_game.init();
    g_shaders.init();
    g_things.init();

    registerLuaFunctions();
    findLuaInitScript();

    g_app.runAppMainLoop();
}

void Client::setupAppNameAndVersion() {
    g_app.setName("OTClient");
    g_app.setCompactName("otclient");
    g_app.setVersion(VERSION);
}

void Client::findLuaInitScript() {
    if(!g_resources.discoverWorkDir("init.lua"))
        g_logger.fatal("Unable to find work directory, the application cannot be initialized.");

    runLuaInitScript();
}

void Client::runLuaInitScript() {
    if(!g_lua.safeRunScript("init.lua"))
        g_logger.fatal("Unable to run script init.lua!");
}

void Client::terminateAndFreeMemory()
{
    g_app.unloadModules();

    g_creatures.terminate();
    g_game.terminate();
    g_map.terminate();
    g_minimap.terminate();
    g_things.terminate();
    g_sprites.terminate();
    g_shaders.terminate();
    g_app.terminate();
}
