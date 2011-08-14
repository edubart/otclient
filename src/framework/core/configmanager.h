#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <global.h>

struct ConfigValueProxy {
    ConfigValueProxy(const std::string& v) : value(v) { }
    operator std::string() const { return aux::unsafe_cast<std::string>(value); }
    operator float() const { return aux::unsafe_cast<float>(value); }
    operator int() const { return aux::unsafe_cast<int>(value); }
    operator bool() const { return aux::unsafe_cast<bool>(value); }
    std::string value;
};

class ConfigManager
{
public:
    bool load(const std::string& fileName);
    bool save();

    template<class T>
    void set(const std::string& key, const T& value) { m_confsMap[key] = aux::unsafe_cast<std::string>(value); }

    ConfigValueProxy get(const std::string& key) { return ConfigValueProxy(m_confsMap[key]); }

private:
    std::string m_fileName;
    std::map<std::string, std::string> m_confsMap;
};

extern ConfigManager g_configs;

#endif
