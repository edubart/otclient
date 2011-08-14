#include "modulemanager.h"
#include "resourcemanager.h"
#include <otml/otml.h>

ModuleManager g_modules;

void ModuleManager::discoverAndLoadModules()
{
    auto moduleDirs = g_resources.listDirectoryFiles("/");
    for(const std::string& moduleDir : moduleDirs) {
        auto moduleFiles = g_resources.listDirectoryFiles("/" + moduleDir);
        for(const std::string& file : moduleFiles) {
            if(boost::ends_with(file, ".otmod"))
                discoverModule("/" + moduleDir + "/" + file);
        }
    }

    // auto load modules
    for(const ModulePtr& module : m_modules) {
        if(!module->isLoaded() && module->autoLoad())
            module->load();
    }
}

bool ModuleManager::discoverModule(const std::string& file)
{
    ModulePtr module;
    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(file);
        OTMLNodePtr moduleNode = doc->at("Module");

        std::string name = moduleNode->readAt<std::string>("name");
        if(getModule(name))
            throw OTMLException(moduleNode, "a module with the same name is already discovered, did you duplicate module names?");

        module = ModulePtr(new Module(name));
        module->discover(moduleNode);
        m_modules.push_back(module);
    } catch(std::exception& e) {
        logError("ERROR: failed to load module from '", file, "':\n", e.what());
        return false;
    }
    return true;
}

void ModuleManager::unloadModules()
{
    for(const ModulePtr& module : m_modules)
        module->unload();
}

ModulePtr ModuleManager::getModule(const std::string& moduleName)
{
    for(const ModulePtr& module : m_modules)
        if(module->getName() == moduleName)
            return module;
    return nullptr;
}
