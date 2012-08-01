/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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
#include <framework/core/clock.h>

ParticleSystem::ParticleSystem()
{
    m_finished = false;
    m_lastUpdateTime = g_clock.seconds();
}

bool ParticleSystem::load(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "Emitter") {
            ParticleEmitterPtr emitter = ParticleEmitterPtr(new ParticleEmitter());
            if(!emitter->load(childNode))
                return false;
            m_emitters.push_back(emitter);
        }
        else if(childNode->tag().find("Affector") != std::string::npos) {
            ParticleAffectorPtr affector;

            if(childNode->tag() == "GravityAffector")
                affector = ParticleAffectorPtr(new GravityAffector);
            else if(childNode->tag() == "AttractionAffector")
                affector = ParticleAffectorPtr(new AttractionAffector);

            if(affector) {
                if(!affector->load(childNode))
                    return false;
                m_affectors.push_back(affector);
            }
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
    static const float delay = 0.0166; // 60 updates/s

    // check if finished
    if(m_particles.empty() && m_emitters.empty()) {
        m_finished = true;
        return;
    }

    // check time
    float elapsedTime = g_clock.seconds() - m_lastUpdateTime;
    if(elapsedTime < delay)
        return;
    m_lastUpdateTime = g_clock.seconds() - std::fmod(elapsedTime, delay);

    auto self = static_self_cast<ParticleSystem>();
    for(int i = 0; i < elapsedTime / delay; ++i) {

        // update emitters
        for(auto it = m_emitters.begin(), end = m_emitters.end(); it != end;) {
            const ParticleEmitterPtr& emitter = *it;
            if(emitter->hasFinished()) {
                it = m_emitters.erase(it);
                continue;
            }
            emitter->update(delay, self);
            ++it;
        }

        // update affectors
        for(auto it = m_affectors.begin(), end = m_affectors.end(); it != end;) {
            const ParticleAffectorPtr& affector = *it;
            if(affector->hasFinished()) {
                it = m_affectors.erase(it);
                continue;
            }
            affector->update(delay);
            ++it;
        }

        // update particles
        for(auto it = m_particles.begin(), end = m_particles.end(); it != end;) {
            const ParticlePtr& particle = *it;
            if(particle->hasFinished()) {
                it = m_particles.erase(it);
                continue;
            }

            // pass particles through affectors
            for(const ParticleAffectorPtr& particleAffector : m_affectors)
                particleAffector->updateParticle(particle, delay);

            particle->update(delay);
            ++it;
        }
    }
}
