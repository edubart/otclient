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

#ifndef CONFIG_H
#define CONFIG_H

#include "declarations.h"

#include <framework/luaengine/luaobject.h>
#include <framework/otml/declarations.h>

// @bindclass
class Config : public LuaObject
{
public:
    Config();

    bool load(const std::string& file);
    bool unload();
    bool save();
    void clear();

    void setValue(const std::string& key, const std::string& value);
    void setList(const std::string& key, const std::vector<std::string>& list);
    std::string getValue(const std::string& key);
    std::vector<std::string> getList(const std::string& key);

    void setNode(const std::string& key, const OTMLNodePtr& node);
    void mergeNode(const std::string& key, const OTMLNodePtr& node);
    OTMLNodePtr getNode(const std::string& key);

    bool exists(const std::string& key);
    void remove(const std::string& key);

    std::string getFileName();
    bool isLoaded();

    // @dontbind
    ConfigPtr asConfig() { return static_self_cast<Config>(); }

private:
    std::string m_fileName;
    OTMLDocumentPtr m_confsDoc;
};

#endif
