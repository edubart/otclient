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

#ifndef MODULE_H
#define MODULE_H

#include "declarations.h"

#include <framework/otml/declarations.h>
#include <framework/luaengine/luaobject.h>

// @bindclass
class Module : public LuaObject
{
public:
    Module(const std::string& name);

    bool load();
    void unload();
    bool reload();

    bool canUnload() { return m_loaded && m_reloadable && !isDependent(); }
    bool canReload() { return m_reloadable && !isDependent(); }
    bool isLoaded() { return m_loaded; }
    bool isReloadable() { return m_reloadable; }
    bool isDependent();
    bool isSandboxed() { return m_sandboxed; }
    bool hasDependency(const std::string& name, bool recursive = false);
    int getSandbox(LuaInterface *lua);

    std::string getDescription() { return m_description; }
    std::string getName() { return m_name; }
    std::string getAuthor() { return m_author; }
    std::string getWebsite() { return m_website; }
    std::string getVersion() { return m_version; }
    bool isAutoLoad() { return m_autoLoad; }
    int getAutoLoadPriority() { return m_autoLoadPriority; }

    // @dontbind
    ModulePtr asModule() { return static_self_cast<Module>(); }

protected:
    void discover(const OTMLNodePtr& moduleNode);
    friend class ModuleManager;

private:
    stdext::boolean<false> m_loaded;
    stdext::boolean<false> m_autoLoad;
    stdext::boolean<false> m_reloadable;
    stdext::boolean<false> m_sandboxed;
    int m_autoLoadPriority;
    int m_sandboxEnv;
    std::tuple<std::string, std::string> m_onLoadFunc;
    std::tuple<std::string, std::string> m_onUnloadFunc;
    std::string m_name;
    std::string m_description;
    std::string m_author;
    std::string m_website;
    std::string m_version;
    std::function<void()> m_loadCallback;
    std::function<void()> m_unloadCallback;
    std::list<std::string> m_dependencies;
    std::list<std::string> m_scripts;
    std::list<std::string> m_loadLaterModules;
};

#endif
