#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "module.h"

class ModuleManager
{
public:
    void discoverAndLoadModules();
    bool discoverModule(const std::string& file);
    void unloadModules();

    ModulePtr getModule(const std::string& moduleName);

private:
    std::vector<ModulePtr> m_modules;
};

extern ModuleManager g_modules;

#endif
