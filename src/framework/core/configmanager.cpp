/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

ConfigManager g_configs;

ConfigManager::ConfigManager()
{
    m_settings = ConfigPtr(new Config());
}

ConfigPtr ConfigManager::getSettings()
{
    return m_settings;
}

ConfigPtr ConfigManager::get(const std::string& file)
{
    for(const ConfigPtr config : m_configs) {
        if(config->getFileName() == file) {
            return config;
        }
    }
    g_logger.error(stdext::format("Unable to find configuration for '%s' ", file));
    return nullptr;
}

ConfigPtr ConfigManager::loadSettings(const std::string file)
{
    if(file.empty()) {
        g_logger.error("Must provide a configuration file to load.");
    }
    else {
        if(m_settings->load(file)) {
            return m_settings;
        }
    }
    return nullptr;
}

ConfigPtr ConfigManager::load(const std::string& file)
{
    if(file.empty()) {
        g_logger.error("Must provide a configuration file to load.");
    }
    else {
        ConfigPtr config = ConfigPtr(new Config());
        if(config->load(file)) {
            m_configs.push_back(config);
            return config;
        }
    }
    return nullptr;
}

bool ConfigManager::unload(const std::string& file)
{
    ConfigPtr config = get(file);
    if(config) {
        config->unload();
        m_configs.remove(config);
        return true;
    }
    return false;
}
