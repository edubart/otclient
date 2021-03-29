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

#include "configmanager.h"

ConfigManager g_configs;

void ConfigManager::init()
{
    m_settings = ConfigPtr(new Config());
}

void ConfigManager::terminate()
{
    if(m_settings) {
        // ensure settings are saved
        m_settings->save();

        m_settings->unload();
        m_settings = nullptr;
    }

    for(ConfigPtr config : m_configs) {
        config->unload();
        config = nullptr;
    }

    m_configs.clear();
}

ConfigPtr ConfigManager::getSettings()
{
    return m_settings;
}

ConfigPtr ConfigManager::get(const std::string& file)
{
    for(const ConfigPtr& config : m_configs) {
        if(config->getFileName() == file) {
            return config;
        }
    }
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

ConfigPtr ConfigManager::create(const std::string& file)
{
    ConfigPtr config = load(file);
    if(!config) {
        config = ConfigPtr(new Config());

        config->load(file);
        config->save();

        m_configs.push_back(config);
    }
    return config;
}

ConfigPtr ConfigManager::load(const std::string& file)
{
    if(file.empty()) {
        g_logger.error("Must provide a configuration file to load.");
        return nullptr;
    }
    else {
        ConfigPtr config = get(file);
        if(!config) {
            config = ConfigPtr(new Config());

            if(config->load(file)) {
                m_configs.push_back(config);
            }
            else {
                // cannot load config
                config = nullptr;
            }
        }
        return config;
    }
}

bool ConfigManager::unload(const std::string& file)
{
    ConfigPtr config = get(file);
    if(config) {
        config->unload();
        remove(config);
        config = nullptr;
        return true;
    }
    return false;
}

void ConfigManager::remove(const ConfigPtr config)
{
    m_configs.remove(config);
}
