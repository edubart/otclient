#include "configs.h"
#include "resourcemanager.h"

#include <framework/otml/otml.h>

Configs g_configs;

bool Configs::load(const std::string& fileName)
{
    m_fileName = fileName;

    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(fileName);
        for(const OTMLNodePtr& child : doc->children())
            m_confsMap[child->tag()] = child->value();
        return true;
    } catch(std::exception& e) {
        logError("ERROR: could not load configurations: ", e.what());
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
