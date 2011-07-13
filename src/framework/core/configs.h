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

#include <global.h>

struct ConfigValueProxy {
    operator std::string() const { return convert<std::string>(value); }
    operator float() const { return convert<float>(value); }
    operator int() const { return convert<int>(value); }
    operator bool() const { return convert<bool>(value); }
    std::string value;
};

class Configs
{
public:
    bool load(const std::string& fileName);
    void save();

    template<class T>
    void set(const std::string& key, const T& value) { m_confsMap[key] = convert<std::string>(value); }

    ConfigValueProxy get(const std::string& key) { return ConfigValueProxy{m_confsMap[key]}; }

private:
    std::string m_fileName;
    std::map<std::string, std::string> m_confsMap;
};

extern Configs g_configs;

#endif // CONFIGS_H
