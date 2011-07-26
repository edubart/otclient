#ifndef CONFIGS_H
#define CONFIGS_H

#include <global.h>

struct ConfigValueProxy {
    ConfigValueProxy(const std::string& v) : value(v) { }
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

    ConfigValueProxy get(const std::string& key) { return ConfigValueProxy(m_confsMap[key]); }

private:
    std::string m_fileName;
    std::map<std::string, std::string> m_confsMap;
};

extern Configs g_configs;

#endif // CONFIGS_H
