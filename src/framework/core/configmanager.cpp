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

ConfigManager::ConfigManager()
{
    m_confsDoc = OTMLDocument::create();
}

bool ConfigManager::load(const std::string& file)
{
    m_fileName = file;

    if(!g_resources.fileExists(file))
        return false;

    try {
        OTMLDocumentPtr confsDoc = OTMLDocument::parse(file);
        if(confsDoc)
            m_confsDoc = confsDoc;
        return true;
    } catch(Exception& e) {
        logError("could not load configurations: ", e.what());
        return false;
    }
}

bool ConfigManager::save()
{
    if(m_fileName.length() == 0)
        return false;
    return m_confsDoc->save(m_fileName);
}

void ConfigManager::set(const std::string& key, const std::string& value)
{
    if(key == "") {
        remove(key);
        return;
    }

    OTMLNodePtr child = OTMLNode::create(key, value);
    m_confsDoc->addChild(child);
}

void ConfigManager::setList(const std::string& key, const std::vector<std::string>& list)
{
    remove(key);

    if(list.size() == 0)
        return;

    OTMLNodePtr child = OTMLNode::create(key, true);
    for(const std::string& value : list)
        child->writeIn(value);
    m_confsDoc->addChild(child);
}

bool ConfigManager::exists(const std::string& key)
{
    return m_confsDoc->hasChildAt(key);
}

std::string ConfigManager::get(const std::string& key)
{
    OTMLNodePtr child = m_confsDoc->get(key);
    if(child)
        return child->value();
    else
        return "";
}

std::vector<std::string> ConfigManager::getList(const std::string& key)
{
    std::vector<std::string> list;
    OTMLNodePtr child = m_confsDoc->get(key);
    if(child) {
        for(const OTMLNodePtr& subchild : child->children())
            list.push_back(subchild->value());
    }
    return list;
}

void ConfigManager::remove(const std::string& key)
{
    OTMLNodePtr child = m_confsDoc->get(key);
    if(child)
        m_confsDoc->removeChild(child);
}
