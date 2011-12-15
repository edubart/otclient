/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "particle.h"
#include "particlesystem.h"

ParticleSystem::ParticleSystem()
{
    m_finished = false;
}

bool ParticleSystem::load(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "Emitter") {
            ParticleEmitterPtr emitter = ParticleEmitterPtr(new ParticleEmitter(shared_from_this()));
            emitter->load(childNode);
            m_emitters.push_back(emitter);
        }
    }
    return true;
}

void ParticleSystem::addParticle(const ParticlePtr& particle)
{
    m_particles.push_back(particle);
}

void ParticleSystem::render()
{
    for(auto it = m_particles.begin(), end = m_particles.end(); it != end; ++it)
        (*it)->render();
}

void ParticleSystem::update()
{
    // check if finished
    if(m_particles.empty() && m_emitters.empty()) {
        m_finished = true;
        return;
    }

    // update emitters
    for(auto it = m_emitters.begin(), end = m_emitters.end(); it != end;) {
        const ParticleEmitterPtr& emitter = *it;
        if(emitter->hasFinished()) {
            it = m_emitters.erase(it);
            continue;
        }
        emitter->update();
        ++it;
    }

    // update particles
    for(auto it = m_particles.begin(), end = m_particles.end(); it != end;) {
        const ParticlePtr& particle = *it;
        if(particle->hasFinished()) {
            it = m_particles.erase(it);
            continue;
        }
        particle->update();
        ++it;
    }
}
