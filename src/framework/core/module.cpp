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

#include "module.h"
#include "modulemanager.h"

#include <framework/otml/otml.h>
#include <framework/luascript/luainterface.h>

Module::Module(const std::string& name)
{
    m_name = name;
}

void Module::discover(const OTMLNodePtr& moduleNode)
{
    const static std::string none = "none";
    m_description = moduleNode->valueAt("description", none);
    m_author = moduleNode->valueAt("author", none);
    m_website = moduleNode->valueAt("website", none);
    m_version = moduleNode->valueAt("version", none);
    m_autoLoad = moduleNode->valueAt<bool>("autoLoad", false);
    m_autoLoadAntecedence = moduleNode->valueAt<int>("autoLoadAntecedence", 100);

    if(OTMLNodePtr node = moduleNode->get("dependencies")) {
        for(const OTMLNodePtr& tmp : node->children())
            m_dependencies.push_back(tmp->value());
    }

    // set onLoad callback
    if(OTMLNodePtr node = moduleNode->get("onLoad")) {
        g_lua.loadFunction(node->value(), "@" + node->source() + "[" + node->tag() + "]");
        g_lua.useValue();
        m_loadCallback = g_lua.polymorphicPop<SimpleCallback>();
    }

    // set onUnload callback
    if(OTMLNodePtr node = moduleNode->get("onUnload")) {
        g_lua.loadFunction(node->value(), "@" + node->source() + "[" + node->tag() + "]");
        g_lua.useValue();
        m_unloadCallback = g_lua.polymorphicPop<SimpleCallback>();
    }
}

bool Module::load()
{
    if(m_loaded)
        return true;

    for(const std::string& depName : m_dependencies) {
        ModulePtr dep = g_modules.getModule(depName);
        if(!dep) {
            logError("Unable to load module '", m_name, "' because dependency '", depName, "' was not found");
            return false;
        }

        if(!dep->isLoaded() && !dep->load()) {
            logError("Unable to load module '", m_name, "' because dependency '", depName, "' has failed to laod");
            return false;
        }
    }

    if(m_loadCallback)
        m_loadCallback();

    logInfo("Loaded module '", m_name, "'");
    m_loaded = true;
    return true;
}

void Module::unload()
{
    if(m_loaded) {
        if(m_unloadCallback)
            m_unloadCallback();
        m_loaded = false;
    }
}
