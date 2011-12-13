#include "particlesmanager.h"
#include <framework/core/resourcemanager.h>
#include <framework/otml/otml.h>

ParticlesManager g_particlesManager;

bool ParticlesManager::load(const std::string& filename)
{
    if(!g_resources.fileExists(filename))
        return false;

    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(filename);
        //for(const OTMLNodePtr& child : doc->children())
            //m_confsMap[child->tag()] = child->value();
        return true;
    } catch(Exception& e) {
        logError("could not load configurations: ", e.what());
        return false;
    }
}

void ParticlesManager::add(ParticlesSystem particleSystem)
{
    // check it has emitters
    m_particlesSystems.push_back(particleSystem);
}

void ParticlesManager::render()
{
    for(auto it = m_particlesSystems.begin(), end = m_particlesSystems.end(); it != end; ++it)
        (*it).render();
}

void ParticlesManager::update()
{
    for(auto it = m_particlesSystems.begin(), end = m_particlesSystems.end(); it != end; ++it)
        (*it).update();
}
