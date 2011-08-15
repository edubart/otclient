#include "module.h"
#include "modulemanager.h"

#include <framework/otml/otml.h>
#include <framework/luascript/luainterface.h>

void Module::discover(const OTMLNodePtr& moduleNode)
{
    m_description = moduleNode->readAt<std::string>("description");
    m_author = moduleNode->readAt<std::string>("author");
    m_website = moduleNode->readAt<std::string>("website");
    m_version = moduleNode->readAt<std::string>("version");

    if(OTMLNodePtr node = moduleNode->get("dependencies")) {
        for(const OTMLNodePtr& tmp : node->childNodes())
            m_dependencies.push_back(tmp->value());
    }

    // set onLoad callback
    if(OTMLNodePtr node = moduleNode->get("onLoad")) {
        g_lua.loadFunction(node->read<std::string>(), "@" + node->source() + "[" + node->tag() + "]");
        g_lua.useValue();
        m_loadCallback = g_lua.polymorphicPop<BooleanCallback>();
    }

    // set onUnload callback
    if(OTMLNodePtr node = moduleNode->get("onUnload")) {
        g_lua.loadFunction(node->read<std::string>(), "@" + node->source() + "[" + node->tag() + "]");
        g_lua.useValue();
        m_unloadCallback = g_lua.polymorphicPop<SimpleCallback>();
    }

    // load if autoLoad is set
    m_autoLoad = moduleNode->readAt<bool>("autoLoad", false);
}

bool Module::load()
{
    for(const std::string& depName : m_dependencies) {
        ModulePtr dep = g_modules.getModule(depName);
        if(!dep) {
            logError("ERROR: failed to load module '",m_name,"': could not find module dependency '",depName,"'");
            return false;
        }

        if(!dep->isLoaded()) {
            if(!dep->load()) {
                logError("ERROR: failed to load module '",m_name,"': a dependency has failed to load");
                return false;
            }
        }
    }

    if(m_loadCallback) {
        m_loaded = m_loadCallback();
        if(!m_loaded) {
            logError("ERROR: failed to load module '",m_name, "': onLoad returned false");
            return false;
        }
    }

    logInfo("Loaded module '", m_name, "'");
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
