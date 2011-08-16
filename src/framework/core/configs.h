#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "declarations.h"

class Configs
{
public:
    bool load(const std::string& fileName);
    bool save();

    void set(const std::string& key, const std::string& value) { m_confsMap[key] = value; }
    std::string get(const std::string& key) { return m_confsMap[key]; }

private:
    std::string m_fileName;
    std::map<std::string, std::string> m_confsMap;
};

extern Configs g_configs;

#endif
