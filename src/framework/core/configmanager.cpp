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

#include "configmanager.h"
#include "resourcemanager.h"

#include <framework/otml/otml.h>

ConfigManager g_configs;

bool ConfigManager::load(const std::string& file)
{
    m_fileName = file;

    if(!g_resources.fileExists(file))
        return false;

    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(file);
        for(const OTMLNodePtr& child : doc->children())
            m_confsMap[child->tag()] = child->value();
        return true;
    } catch(Exception& e) {
        logError("could not load configurations: ", e.what());
        return false;
    }
}

bool ConfigManager::save()
{
    OTMLDocumentPtr doc = OTMLDocument::create();
    for(auto it : m_confsMap) {
        OTMLNodePtr node = OTMLNode::create(it.first, it.second);
        doc->addChild(node);
    }
    return doc->save(m_fileName);
}

bool ConfigManager::exists(const std::string& key)
{
    return m_confsMap.find(key) != m_confsMap.end();
}

void ConfigManager::set(const std::string& key, const std::string& value)
{
    m_confsMap[key] = value;
}

std::string ConfigManager::get(const std::string& key)
{
    return m_confsMap[key];
}

void ConfigManager::remove(const std::string& key)
{
    auto it = m_confsMap.find(key);
    if(it != m_confsMap.end())
        m_confsMap.erase(it);
}
