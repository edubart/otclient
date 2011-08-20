#include "configs.h"
#include "resourcemanager.h"

#include <framework/otml/otml.h>

Configs g_configs;

bool Configs::load(const std::string& file)
{
    m_fileName = file;

    if(!g_resources.fileExists(file))
        return false;

    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(file);
        for(const OTMLNodePtr& child : doc->children())
            m_confsMap[child->tag()] = child->value();
        return true;
    } catch(std::exception& e) {
        logError("could not load configurations: ", e.what());
        return false;
    }
}

bool Configs::save()
{
    OTMLDocumentPtr doc = OTMLDocument::create();
    for(auto it : m_confsMap) {
        OTMLNodePtr node = OTMLNode::create(it.first, it.second);
        doc->addChild(node);
    }
    return doc->save(m_fileName);
}
