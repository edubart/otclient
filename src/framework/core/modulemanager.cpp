/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

        std::string name = moduleNode->valueAt("name");
        if(getModule(name))
            throw OTMLException(moduleNode, "a module with the same name is already discovered, did you duplicate module names?");

        module = ModulePtr(new Module(name));
        module->discover(moduleNode);
        m_modules.push_back(module);
    } catch(std::exception& e) {
        logError("failed to load module from '", file, "': ", e.what());
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
