#include "particlesystem.h"

bool ParticleSystem::load(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "Emitter") {
            ParticleEmitterPtr emitter = ParticleEmitterPtr(new ParticleEmitter);
            emitter->load(childNode);
            m_emitters.push_back(emitter);
        }
    }
    return true;
}

void ParticleSystem::render()
{
    for(auto it = m_emitters.begin(), end = m_emitters.end(); it != end; ++it)
        (*it)->render();
}

void ParticleSystem::update()
{
    for(auto it = m_emitters.begin(), end = m_emitters.end(); it != end;) {
        const ParticleEmitterPtr& emitter = *it;
        if(emitter->hasFinished()) {
            it = m_emitters.erase(it);
            continue;
        }
        emitter->update();
        ++it;
    }
}
