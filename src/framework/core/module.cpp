/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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
#include "resourcemanager.h"

#include <framework/otml/otml.h>
#include <framework/luaengine/luainterface.h>

Module::Module(const std::string& name)
{
    m_name = name;
    m_sandboxEnv = g_lua.newSandboxEnv();
}

bool Module::load()
{
    if(m_loaded)
        return true;

    try {
        // add to package.loaded
        g_lua.getGlobalField("package", "loaded");
        g_lua.getRef(m_sandboxEnv);
        g_lua.setField(m_name);
        g_lua.pop();

        for(const std::string& depName : m_dependencies) {
            if(depName == m_name)
                stdext::throw_exception("cannot depend on itself");

            ModulePtr dep = g_modules.getModule(depName);
            if(!dep)
                stdext::throw_exception(stdext::format("dependency '%s' was not found", depName));

            if(dep->hasDependency(m_name, true))
                stdext::throw_exception(stdext::format("dependency '%s' is recursively depending on itself", depName));

            if(!dep->isLoaded() && !dep->load())
                stdext::throw_exception(stdext::format("dependency '%s' has failed to load", depName));
        }

        if(m_sandboxed)
            g_lua.setGlobalEnvironment(m_sandboxEnv);

        for(const std::string& script : m_scripts) {
            g_lua.loadScript(script);
            g_lua.safeCall(0, 0);
        }

        const std::string& onLoadBuffer = std::get<0>(m_onLoadFunc);
        const std::string& onLoadSource = std::get<1>(m_onLoadFunc);
        if(!onLoadBuffer.empty()) {
            g_lua.loadBuffer(onLoadBuffer, onLoadSource);
            if(m_sandboxed) {
                g_lua.getRef(m_sandboxEnv);
                g_lua.setEnv();
            }
            g_lua.safeCall(0, 0);
        }

        if(m_sandboxed)
            g_lua.resetGlobalEnvironment();

        m_loaded = true;
        g_logger.debug(stdext::format("Loaded module '%s'", m_name));
    } catch(stdext::exception& e) {
        // remove from package.loaded
        g_lua.getGlobalField("package", "loaded");
        g_lua.pushNil();
        g_lua.setField(m_name);
        g_lua.pop();

        if(m_sandboxed)
            g_lua.resetGlobalEnvironment();
        g_logger.error(stdext::format("Unable to load module '%s': %s", m_name, e.what()));
        return false;
    }

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
        try {
            if(m_sandboxed)
                g_lua.setGlobalEnvironment(m_sandboxEnv);

            const std::string& onUnloadBuffer = std::get<0>(m_onUnloadFunc);
            const std::string& onUnloadSource = std::get<1>(m_onUnloadFunc);
            if(!onUnloadBuffer.empty()) {
                g_lua.loadBuffer(onUnloadBuffer, onUnloadSource);
                g_lua.safeCall(0, 0);
            }

            if(m_sandboxed)
                g_lua.resetGlobalEnvironment();
        } catch(stdext::exception& e) {
            if(m_sandboxed)
                g_lua.resetGlobalEnvironment();
            g_logger.error(stdext::format("Unable to unload module '%s': %s", m_name, e.what()));
        }

        // clear all env references
        g_lua.getRef(m_sandboxEnv);
        g_lua.clearTable();
        g_lua.pop();

        // remove from package.loaded
        g_lua.getGlobalField("package", "loaded");
        g_lua.pushNil();
        g_lua.setField(m_name);
        g_lua.pop();

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

bool Module::hasDependency(const std::string& name, bool recursive)
{
    if(std::find(m_dependencies.begin(), m_dependencies.end(), name) != m_dependencies.end())
        return true;

    if(recursive) {
        for(const std::string& depName : m_dependencies) {
            ModulePtr dep = g_modules.getModule(depName);
            if(dep && dep->hasDependency(name, true))
                return true;
        }
    }

    return false;
}

int Module::getSandbox(LuaInterface* lua)
{
    lua->getRef(m_sandboxEnv);
    return 1;
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
    m_sandboxed = moduleNode->valueAt<bool>("sandboxed", false);
    m_autoLoadPriority = moduleNode->valueAt<int>("autoload-priority", 9999);

    if(OTMLNodePtr node = moduleNode->get("dependencies")) {
        for(const OTMLNodePtr& tmp : node->children())
            m_dependencies.push_back(tmp->value());
    }

    if(OTMLNodePtr node = moduleNode->get("scripts")) {
        for(const OTMLNodePtr& tmp : node->children())
            m_scripts.push_back(stdext::resolve_path(tmp->value(), node->source()));
    }

    if(OTMLNodePtr node = moduleNode->get("load-later")) {
        for(const OTMLNodePtr& tmp : node->children())
            m_loadLaterModules.push_back(tmp->value());
    }

    if(OTMLNodePtr node = moduleNode->get("@onLoad"))
        m_onLoadFunc = std::make_tuple(node->value(), "@" + node->source() + ":[" + node->tag() + "]");

    if(OTMLNodePtr node = moduleNode->get("@onUnload"))
        m_onUnloadFunc = std::make_tuple(node->value(), "@" + node->source() + ":[" + node->tag() + "]");
}
