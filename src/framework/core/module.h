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

#ifndef MODULE_H
#define MODULE_H

#include "declarations.h"

#include <framework/otml/declarations.h>
#include <framework/luascript/luaobject.h>

class Module : public LuaObject
{
public:
    Module(const std::string& name);

    bool load();
    void unload();

    bool isLoaded() { return m_loaded; }

    std::string getDescription() { return m_description; }
    std::string getName() { return m_name; }
    std::string getAuthor() { return m_author; }
    std::string getWebsite() { return m_website; }
    std::string getVersion() { return m_version; }
    bool isAutoLoad() { return m_autoLoad; }
    int getAutoLoadAntecedence() { return m_autoLoadAntecedence; }

protected:
    void discover(const OTMLNodePtr& moduleNode);
    friend class ModuleManager;

private:
    Boolean<false> m_loaded;
    Boolean<false> m_autoLoad;
    int m_autoLoadAntecedence;
    std::string m_name;
    std::string m_description;
    std::string m_author;
    std::string m_website;
    std::string m_version;
    SimpleCallback m_loadCallback;
    SimpleCallback m_unloadCallback;
    std::list<std::string> m_dependencies;
};

#endif
