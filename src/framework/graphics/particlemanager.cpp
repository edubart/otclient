#include "particlemanager.h"
#include <framework/core/resourcemanager.h>
#include <framework/otml/otml.h>

ParticleManager g_particleManager;

bool ParticleManager::load(const std::string& filename)
{
    if(!g_resources.fileExists(filename))
        return false;

    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(filename);
        for(const OTMLNodePtr& node : doc->children()) {
            if(node->tag() == "ParticleSystem") {
                ParticleSystemPtr particleSystem = ParticleSystemPtr(new ParticleSystem);
                particleSystem->load(node);
                m_particlesSystems.push_back(particleSystem);
            }
        }
        return true;
    } catch(Exception& e) {
        logError("could not load particles: ", e.what());
        return false;
    }
}

void ParticleManager::render()
{
    for(auto it = m_particlesSystems.begin(), end = m_particlesSystems.end(); it != end; ++it)
        (*it)->render();
}

void ParticleManager::update()
{
    for(auto it = m_particlesSystems.begin(), end = m_particlesSystems.end(); it != end; ++it)
        (*it)->update();
}
