#ifndef MODULE_H
#define MODULE_H

#include "declarations.h"

#include <framework/otml/declarations.h>

class Module
{
public:
    Module(const std::string& name) : m_loaded(false), m_autoLoad(false), m_name(name) { }
    void discover(const OTMLNodePtr& moduleNode);

    bool load();
    void unload();

    bool isLoaded() const { return m_loaded; }

    std::string getDescription() const { return m_description; }
    std::string getName() const { return m_name; }
    std::string getAuthor() const { return m_author; }
    std::string getWebsite() const { return m_website; }
    std::string getVersion() const { return m_version; }
    bool autoLoad() const { return m_autoLoad; }

private:
    bool m_loaded;
    bool m_autoLoad;
    std::string m_name;
    std::string m_description;
    std::string m_author;
    std::string m_website;
    std::string m_version;
    BooleanCallback m_loadCallback;
    SimpleCallback m_unloadCallback;
    std::list<std::string> m_dependencies;
};

#endif
