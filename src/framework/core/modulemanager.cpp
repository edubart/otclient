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

#include "modulemanager.h"
#include "resourcemanager.h"

#include <framework/otml/otml.h>
#include <framework/application.h>

ModuleManager g_modules;

void ModuleManager::discoverModules()
{
    // remove modules that are not loaded
    m_autoLoadModules.clear();

    auto moduleDirs = g_resources.listDirectoryFiles("/");
    for(const std::string& moduleDir : moduleDirs) {
        auto moduleFiles = g_resources.listDirectoryFiles("/" + moduleDir);
        for(const std::string& moduleFile : moduleFiles) {
            if(boost::ends_with(moduleFile, ".otmod")) {
                ModulePtr module = discoverModule("/" + moduleDir + "/" + moduleFile);
                if(module && module->isAutoLoad())
                    m_autoLoadModules.insert(make_pair(module->getAutoLoadPriority(), module));
            }
        }
    }
}

void ModuleManager::autoLoadModules(int maxPriority)
{
    for(auto& pair : m_autoLoadModules) {
        int priority = pair.first;
        if(priority > maxPriority)
            break;
        ModulePtr module = pair.second;
        if(!module->isLoaded() && !module->load())
            logFatal("A required module has failed to load, cannot continue to run.");
    }
}

void ModuleManager::discoverModulesPath()
{
    // search for modules directory
    std::string possibleModulesDirs[] = { "modules",
                                          g_resources.getBaseDir() + "modules",
                                          g_resources.getBaseDir() + "../modules",
                                          g_resources.getBaseDir() + "../share/" + g_app->getName() + "/modules",
                                           "" };
    bool found = false;
    for(const std::string& dir : possibleModulesDirs) {
        // try to add module directory
        if(g_resources.addToSearchPath(dir, false)) {
            logInfo("Using modules directory '", dir.c_str(), "'");
            found = true;
            break;
        }
    }

    if(!found)
        logFatal("Could not find modules directory");

    // search for addons directory
    std::string possibleAddonsDirs[] = { "addons",
                                         g_resources.getBaseDir() + "addons",
                                         g_resources.getBaseDir() + "../addons",
                                         g_resources.getBaseDir() + "../addons/" + g_app->getName() + "/modules",
                                         "" };
    for(const std::string& dir : possibleAddonsDirs) {
        // try to add module directory
        if(g_resources.addToSearchPath(dir, true)) {
            logInfo("Using addons directory '", dir.c_str(), "'");
            found = true;
            break;
        }
    }
}

ModulePtr ModuleManager::discoverModule(const std::string& moduleFile)
{
    ModulePtr module;
    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(moduleFile);
        OTMLNodePtr moduleNode = doc->at("Module");

        std::string name = moduleNode->valueAt("name");
        //if(getModule(name))
        //    Fw::throwException("module '", name, "' already exists, cannot have duplicate module names");

        bool push = false;
        module = getModule(name);
        if(!module) {
            module = ModulePtr(new Module(name));
            push = true;
        }
        module->discover(moduleNode);

        // not loaded modules are always in back
        if(push)
            m_modules.push_back(module);
    } catch(Exception& e) {
        logError("Unable to discover module from file '", moduleFile, "': ", e.what());
    }
    return module;
}

void ModuleManager::ensureModuleLoaded(const std::string& moduleName)
{
    ModulePtr module = g_modules.getModule(moduleName);
    if(!module || !module->load())
        logFatal("Unable to load '", moduleName, "' module");
}

void ModuleManager::unloadModules()
{
    auto modulesBackup = m_modules;
    for(const ModulePtr& module : modulesBackup)
        module->unload();
}

void ModuleManager::reloadModules()
{
    std::deque<ModulePtr> toLoadList;

    // unload in the reverse direction, try to unload upto 10 times (because of dependencies)
    for(int i=0;i<10;++i) {
        auto modulesBackup = m_modules;
        for(const ModulePtr& module : modulesBackup) {
            if(module->isLoaded() && module->canUnload()) {
                module->unload();
                toLoadList.push_front(module);
            }
        }
    }

    for(const ModulePtr& module : toLoadList)
        module->load();
}

ModulePtr ModuleManager::getModule(const std::string& moduleName)
{
    for(const ModulePtr& module : m_modules)
        if(module->getName() == moduleName)
            return module;
    return nullptr;
}

void ModuleManager::updateModuleLoadOrder(ModulePtr module)
{
    auto it = std::find(m_modules.begin(), m_modules.end(), module);
    if(it != m_modules.end())
        m_modules.erase(it);
    if(module->isLoaded())
        m_modules.push_front(module);
    else
        m_modules.push_back(module);
}
