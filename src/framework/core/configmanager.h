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

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "config.h"

// @bindsingleton g_configs
class ConfigManager
{
public:
    void init();
    void terminate();

    ConfigPtr getSettings();
    ConfigPtr get(const std::string& file);

    ConfigPtr create(const std::string& file);
    ConfigPtr loadSettings(const std::string file);
    ConfigPtr load(const std::string& file);

    bool unload(const std::string& file);
    void remove(const ConfigPtr config);

protected:
    ConfigPtr m_settings;

private:
    std::list<ConfigPtr> m_configs;
};

extern ConfigManager g_configs;

#endif
