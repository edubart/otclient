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

#include "config.h"
#include "resourcemanager.h"
#include "configmanager.h"

#include <framework/otml/otml.h>

Config::Config()
{
    m_confsDoc = OTMLDocument::create();
    m_fileName = "";
}

bool Config::load(const std::string& file)
{
    m_fileName = file;

    if(!g_resources.fileExists(file))
        return false;

    try {
        OTMLDocumentPtr confsDoc = OTMLDocument::parse(file);
        if(confsDoc)
            m_confsDoc = confsDoc;
        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("Unable to parse configuration file '%s': ", e.what()));
        return false;
    }
}

bool Config::unload()
{
    if(isLoaded()) {
        m_confsDoc = nullptr;
        m_fileName = "";
        return true;
    }
    return false;
}

bool Config::save()
{
    if(m_fileName.length() == 0)
        return false;
    return m_confsDoc->save(m_fileName);
}

void Config::clear()
{
    m_confsDoc->clear();
}

void Config::setValue(const std::string& key, const std::string& value)
{
    if(key == "") {
        remove(key);
        return;
    }

    OTMLNodePtr child = OTMLNode::create(key, value);
    m_confsDoc->addChild(child);
}

void Config::setList(const std::string& key, const std::vector<std::string>& list)
{
    remove(key);

    if(list.size() == 0)
        return;

    OTMLNodePtr child = OTMLNode::create(key, true);
    for(const std::string& value : list)
        child->writeIn(value);
    m_confsDoc->addChild(child);
}

bool Config::exists(const std::string& key)
{
    return m_confsDoc->hasChildAt(key);
}

std::string Config::getValue(const std::string& key)
{
    OTMLNodePtr child = m_confsDoc->get(key);
    if(child)
        return child->value();
    else
        return "";
}

std::vector<std::string> Config::getList(const std::string& key)
{
    std::vector<std::string> list;
    OTMLNodePtr child = m_confsDoc->get(key);
    if(child) {
        for(const OTMLNodePtr& subchild : child->children())
            list.push_back(subchild->value());
    }
    return list;
}

void Config::remove(const std::string& key)
{
    OTMLNodePtr child = m_confsDoc->get(key);
    if(child)
        m_confsDoc->removeChild(child);
}

void Config::setNode(const std::string& key, const OTMLNodePtr& node)
{
    remove(key);
    mergeNode(key, node);
}

void Config::mergeNode(const std::string& key, const OTMLNodePtr& node)
{
    OTMLNodePtr clone = node->clone();
    node->setTag(key);
    node->setUnique(true);
    m_confsDoc->addChild(node);
}

OTMLNodePtr Config::getNode(const std::string& key)
{
    return m_confsDoc->get(key);
}

bool Config::isLoaded()
{
    return !m_fileName.empty() && m_confsDoc;
}

std::string Config::getFileName()
{
    return m_fileName;
}
