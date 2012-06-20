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

bool Module::load()
{
    if(m_loaded)
        return true;

    for(const std::string& depName : m_dependencies) {
        ModulePtr dep = g_modules.getModule(depName);
        if(!dep) {
            g_logger.error(stdext::format("Unable to load module '%s' because dependency '%s' was not found", m_name, depName));
            return false;
        }

        if(!dep->isLoaded() && !dep->load()) {
            g_logger.error(stdext::format("Unable to load module '%s' because dependency '%s' has failed to load", m_name, depName));
            return false;
        }
    }

    if(m_loadCallback)
        m_loadCallback();

    m_loaded = true;
    g_logger.debug(stdext::format("Loaded module '%s'", m_name));
    g_modules.updateModuleLoadOrder(asModule());

    for(const std::string& modName : m_loadLaterModules) {
        ModulePtr dep = g_modules.getModule(modName);
        if(!dep)
            g_logger.error(stdext::format("Unable to find module '%s' required by '%s'", modName, m_name));
        else if(!dep->isLoaded())
            dep->load();
    }

    return true;
}

void Module::unload()
{
    if(m_loaded) {
        if(m_unloadCallback)
            m_unloadCallback();
        m_loaded = false;
        //g_logger.info(stdext::format("Unloaded module '%s'", m_name));
        g_modules.updateModuleLoadOrder(asModule());
    }
}

bool Module::reload()
{
    unload();
    return load();
}

bool Module::isDependent()
{
    for(const ModulePtr& module : g_modules.getModules()) {
        if(module->isLoaded() && module->hasDependency(m_name))
            return true;
    }
    return false;
}

bool Module::hasDependency(const std::string& name)
{
    if(std::find(m_dependencies.begin(), m_dependencies.end(), name) != m_dependencies.end())
        return true;
    return false;
}

void Module::discover(const OTMLNodePtr& moduleNode)
{
    const static std::string none = "none";
    m_description = moduleNode->valueAt("description", none);
    m_author = moduleNode->valueAt("author", none);
    m_website = moduleNode->valueAt("website", none);
    m_version = moduleNode->valueAt("version", none);
    m_autoLoad = moduleNode->valueAt<bool>("autoload", false);
    m_reloadable = moduleNode->valueAt<bool>("reloadable", true);
    m_autoLoadPriority = moduleNode->valueAt<int>("autoload-priority", 9999);

    if(OTMLNodePtr node = moduleNode->get("dependencies")) {
        for(const OTMLNodePtr& tmp : node->children())
            m_dependencies.push_back(tmp->value());
    }

    // set onLoad callback
    if(OTMLNodePtr node = moduleNode->get("@onLoad")) {
        g_lua.loadFunction(node->value(), "@" + node->source() + "[" + node->tag() + "]");
        g_lua.useValue();
        m_loadCallback = g_lua.polymorphicPop<std::function<void()>>();
    }

    // set onUnload callback
    if(OTMLNodePtr node = moduleNode->get("@onUnload")) {
        g_lua.loadFunction(node->value(), "@" + node->source() + "[" + node->tag() + "]");
        g_lua.useValue();
        m_unloadCallback = g_lua.polymorphicPop<std::function<void()>>();
    }

    if(OTMLNodePtr node = moduleNode->get("load-later")) {
        for(const OTMLNodePtr& tmp : node->children())
            m_loadLaterModules.push_back(tmp->value());
    }
}
