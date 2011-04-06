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


#ifndef CONFIGS_H
#define CONFIGS_H

#include "prerequisites.h"

class Configs
{
public:
    Configs() { }

    /// Read configuration file and parse all settings to memory
    bool load(const std::string& fileName);

    /// Dump all settings to configuration file
    void save();

    void setValue(const std::string &key, const std::string &value);
    void setValue(const std::string &key, const char *value);
    void setValue(const std::string &key, float value);
    void setValue(const std::string &key, bool value);
    void setValue(const std::string &key, int value);

    const std::string &getString(const std::string &key) const;
    float getFloat(const std::string &key) const;
    bool getBoolean(const std::string &key) const;
    int getInteger(const std::string &key) const;

private:
    std::string m_fileName;
    std::map<std::string, std::string> m_confsMap;
};

extern Configs g_configs;

#endif // CONFIGS_H
