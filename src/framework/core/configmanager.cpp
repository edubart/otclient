#include "configmanager.h"
#include "resourcemanager.h"

#include <otml/otml.h>

ConfigManager g_configs;

bool ConfigManager::load(const std::string& fileName)
{
    m_fileName = fileName;

    if(!g_resources.fileExists(fileName))
        return false;

    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(fileName);
        for(const OTMLNodePtr& child : doc->childNodes())
            m_confsMap[child->tag()] = child->value();
    } catch(std::exception& e) {
        logError("ERROR: could not load configurations: ", e.what());
        return false;
    }

    return true;
}

bool ConfigManager::save()
{
    if(!m_fileName.empty()) {
        OTMLDocumentPtr doc = OTMLDocument::create();
        doc->write(m_confsMap);
        return doc->save(m_fileName);
    }
    return false;
}
