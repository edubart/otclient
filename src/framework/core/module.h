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

#ifndef MODULE_H
#define MODULE_H

#include "declarations.h"

#include <framework/otml/declarations.h>

class Module
{
public:
    Module(const std::string& name) : m_loaded(false), m_autoLoad(false), m_name(name) { }
    void discover(const OTMLNodePtr& moduleNode);

    bool load();
    void unload();

    bool isLoaded() const { return m_loaded; }

    std::string getDescription() const { return m_description; }
    std::string getName() const { return m_name; }
    std::string getAuthor() const { return m_author; }
    std::string getWebsite() const { return m_website; }
    std::string getVersion() const { return m_version; }
    bool autoLoad() const { return m_autoLoad; }

private:
    bool m_loaded;
    bool m_autoLoad;
    std::string m_name;
    std::string m_description;
    std::string m_author;
    std::string m_website;
    std::string m_version;
    BooleanCallback m_loadCallback;
    SimpleCallback m_unloadCallback;
    std::list<std::string> m_dependencies;
};

#endif
