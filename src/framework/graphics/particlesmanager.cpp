#include "particlesmanager.h"

ParticlesManager g_particlesManager;

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
