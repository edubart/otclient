/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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
#include <framework/core/modulemanager.h>
#include <framework/core/resourcemanager.h>
#include <framework/graphics/graphics.h>
#include "game.h"
#include "map.h"
#include "shadermanager.h"
#include "spritemanager.h"
#include "minimap.h"
#include <framework/core/configmanager.h>

Client g_client;

void Client::init(std::vector<std::string>& args)
{
    // register needed lua functions
    registerLuaFunctions();

    g_map.init();
    g_minimap.init();
    g_game.init();
    g_shaders.init();
    g_things.init();

    //TODO: restore options
/*
    if(g_graphics.parseOption(arg))
        continue;

    if(arg == "-version" || arg == "--version" || arg == "-v") {
        stdext::print(
            m_appName, " ", m_appVersion, "\n"
            "Buitt on: ", BUILD_DATE, "\n",
            "Commit: ", BUILD_COMMIT, "\n",
            "Compiled by: ", BUILD_COMPILER, "\n",
            "Build type: ", BUILD_TYPE, "\n");
        return;
    } else if(arg == "-help" || arg == "--help" || arg == "-h" || arg == "-?" || arg == "/?") {
        stdext::print(
            "Usage: ", args[0], " [options]\n"
            "Options:\n"
            "  -help                            Display this information and exit\n"
            "  -version                         Display version and exit\n"
            "  \n"
            "  -no-fbos                         Disable usage of opengl framebuffer objects\n"
            "  -no-mipmaps                      Disable texture mipmaping\n"
            "  -no-smooth                       Disable texture smoothing (bilinear filter)\n"
            "  -no-non-power-of-two-textures    Use only power of two textures\n"
            "  -no-clamp-to-edge                Don't use GL_CLAMP_TO_EDGE\n"
            "  -no-backbuffer-cache             Don't allow backbuffer caching\n"
            "  -hardware-buffers                Cache vertex arrays in hardware\n"
            "  -opengl1                         Use OpenGL 1.x painter\n"
            "  -opengl2                         Use OpenGL 2.0 painter\n");
        return;
    } else {
        stdext::println("Unrecognized option '", arg, "', please see -help for available options list");
        return;
    }
    */
}

void Client::terminate()
{
    g_creatures.terminate();
    g_game.terminate();
    g_map.terminate();
    g_minimap.terminate();
    g_things.terminate();
    g_sprites.terminate();
    g_shaders.terminate();
}
