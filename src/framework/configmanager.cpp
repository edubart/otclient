/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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

#include <yaml-cpp/yaml.h>

ConfigManager g_config;

ConfigManager::ConfigManager()
{

}

ConfigManager::~ConfigManager()
{

}

bool ConfigManager::load(const std::string& fileName)
{
    m_fileName = fileName;

    if(!g_resources.fileExists(fileName))
        return false;

    std::string fileContents = g_resources.loadTextFile(fileName);
    if(fileContents.size() == 0)
        return false;

    std::istringstream fin(fileContents);

    try {
        YAML::Parser parser(fin);

        YAML::Node doc;
        parser.GetNextDocument(doc);

        for(YAML::Iterator it = doc.begin(); it != doc.end(); ++it) {
            std::string key, value;
            it.first() >> key;
            it.second() >> value;
            m_confsMap[key] = value;
        }
    } catch (YAML::ParserException& e) {
        error("Malformed configuration file!");
        return false;
    }

    return true;
}

void ConfigManager::save()
{
    if(!m_fileName.empty()) {
        YAML::Emitter out;
        out << m_confsMap;
        g_resources.saveFile(m_fileName, (const unsigned char*)out.c_str(), out.size());
    }
}

void ConfigManager::setValue(const std::string &key, const std::string &value)
{
    m_confsMap[key] = value;
}

void ConfigManager::setValue(const std::string &key, const char *value)
{
    m_confsMap[key] = value;
}

void ConfigManager::setValue(const std::string &key, int value)
{
    setValue(key, convertType<std::string, int>(value));
}

void ConfigManager::setValue(const std::string &key, float value)
{
    setValue(key, convertType<std::string, float>(value));
}

void ConfigManager::setValue(const std::string &key, bool value)
{
    if(value)
        setValue(key,"true");
    else
        setValue(key,"false");
}

const std::string &ConfigManager::getString(const std::string &key)
{
    std::map<std::string, std::string>::iterator iter = m_confsMap.find(key);
    if(iter == m_confsMap.end()) {
        warning("Config value %s not found", key.c_str());
        static std::string emptystr;
        return emptystr;
    }
    return iter->second;
}

float ConfigManager::getFloat(const std::string &key)
{
    std::map<std::string, std::string>::iterator iter = m_confsMap.find(key);
    if(iter == m_confsMap.end()) {
        warning("Config value %s not found", key.c_str());
        return 0;
    }
    return convertType<float, std::string>(iter->second);
}

bool ConfigManager::getBoolean(const std::string &key)
{
    std::map<std::string, std::string>::iterator iter = m_confsMap.find(key);
    if(iter == m_confsMap.end()) {
        warning("Config value %s not found", key.c_str());
        return 0;
    }
    return (iter->second == "true");
}

int ConfigManager::getInteger(const std::string &key)
{
    std::map<std::string, std::string>::iterator iter = m_confsMap.find(key);
    if(iter == m_confsMap.end()) {
        warning("Config value %s not found", key.c_str());
        return 0;
    }
    return convertType<int, std::string>(iter->second);
}
