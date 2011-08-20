#include "module.h"
#include "modulemanager.h"

#include <framework/otml/otml.h>
#include <framework/luascript/luainterface.h>

void Module::discover(const OTMLNodePtr& moduleNode)
{
    const static std::string none = "none";
    m_description = moduleNode->valueAt("description", none);
    m_author = moduleNode->valueAt("author", none);
    m_website = moduleNode->valueAt("website", none);
    m_version = moduleNode->valueAt("version", none);
    m_autoLoad = moduleNode->valueAt<bool>("autoLoad", false);

    if(OTMLNodePtr node = moduleNode->get("dependencies")) {
        for(const OTMLNodePtr& tmp : node->children())
            m_dependencies.push_back(tmp->value());
    }

    // set onLoad callback
    if(OTMLNodePtr node = moduleNode->get("onLoad")) {
        g_lua.loadFunction(node->value<std::string>(), "@" + node->source() + "[" + node->tag() + "]");
        g_lua.useValue();
        m_loadCallback = g_lua.polymorphicPop<BooleanCallback>();
    }

    // set onUnload callback
    if(OTMLNodePtr node = moduleNode->get("onUnload")) {
        g_lua.loadFunction(node->value<std::string>(), "@" + node->source() + "[" + node->tag() + "]");
        g_lua.useValue();
        m_unloadCallback = g_lua.polymorphicPop<SimpleCallback>();
    }
}

bool Module::load()
{
    try {
        for(const std::string& depName : m_dependencies) {
            ModulePtr dep = g_modules.getModule(depName);
            if(!dep)
                throw std::runtime_error(fw::mkstr("could not find module dependency '", depName ,"'"));

            if(!dep->isLoaded() && !dep->load())
                throw std::runtime_error(fw::mkstr("dependency '", depName, "' has failed to load"));
        }

        if(m_loadCallback) {
            m_loaded = m_loadCallback();
            if(!m_loaded)
                throw std::runtime_error("module onLoad event returned false");
        }

        logInfo("Loaded module '", m_name, "'");
        return true;
    } catch(std::exception& e) {
        logError("failed to load module '", m_name, "': ", e.what());
        return false;
    }
}

void Module::unload()
{
    if(m_loaded) {
        if(m_unloadCallback)
            m_unloadCallback();
        m_loaded = false;
    }
}
