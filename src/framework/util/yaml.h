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


#ifndef YAML_H
#define YAML_H

#include <prerequisites.h>
#include <yaml-cpp/yaml.h>

inline void operator>>(const YAML::Node& node, Color& color)
{
    int r, g, b, a;
    node[0] >> r;
    node[1] >> g;
    node[2] >> b;
    node[3] >> a;
    color.setRGBA(r,g,b,a);
}

template <class T>
inline void operator>>(const YAML::Node& node, TPoint<T>& point)
{
    node[0] >> point.x;
    node[1] >> point.y;
}

template <class T>
inline void operator>>(const YAML::Node& node, TRect<T>& rect)
{
    T x, y, width, height;
    node[0] >> x;
    node[1] >> y;
    node[2] >> width;
    node[3] >> height;
    rect.setRect(x, y, width, height);
}

template <class T>
inline void operator>>(const YAML::Node& node, TSize<T>& size)
{
    T w, h;
    node[0] >> w;
    node[1] >> h;
    size.setSize(w, h);
}

template<class T>
inline T yamlRead(const YAML::Node& node)
{
    return node.to<T>();
}

template<class T>
inline T yamlRead(const YAML::Node& node, const char *name)
{
    T value;
    if(node.Type() == YAML::NodeType::Map)
        node[name] >> value;
    return value;
}

template<class T>
inline T yamlRead(const YAML::Node& node, const char *name, const T& defaultValue)
{
    T value = defaultValue;
    if(node.Type() == YAML::NodeType::Map && node.FindValue(name))
        node[name] >> value;
    return value;
}

inline bool yamlHasValue(const YAML::Node& node, const char *name)
{
    return (node.Type() == YAML::NodeType::Map && node.FindValue(name) != NULL);
}

inline std::string yamlErrorDesc(const YAML::Node& node, const std::string& error)
{
    return YAML::Exception(node.GetMark(), error.c_str()).what();
}

inline void yamlThrowError(const YAML::Node& node, const std::string& error)
{
    throw YAML::Exception(node.GetMark(), error.c_str());
}

template<class A, class B>
inline std::map<A,B> yamlReadMap(const YAML::Node& node, const char *name)
{
    std::map<A,B> map;
    if(node.Type() == YAML::NodeType::Map) {
        if(const YAML::Node* mapNode = node.FindValue(name)) {
            for(auto it = mapNode->begin(); it != mapNode->end(); ++it) {
                A a;
                B b;
                it.first() >> a;
                it.second() >> b;
                map[a] = b;
            }
        }
    }
    return map;
}

#endif // YAML_H
