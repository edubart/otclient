/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#include "particlemanager.h"
#include <framework/core/resourcemanager.h>
#include <framework/otml/otml.h>

ParticleManager g_particles;

bool ParticleManager::importParticle(std::string file)
{
    try {
        file = g_resources.guessFilePath(file, "otps");

        OTMLDocumentPtr doc = OTMLDocument::parse(file);
        for(const OTMLNodePtr& node : doc->children()) {
            if(node->tag() == "Effect") {
                ParticleEffectTypePtr particleEffectType = ParticleEffectTypePtr(new ParticleEffectType);
                particleEffectType->load(node);
                m_effectsTypes[particleEffectType->getName()] = particleEffectType;
            }
            else if(node->tag() == "Particle") {
                ParticleTypePtr particleType = ParticleTypePtr(new ParticleType);
                particleType->load(node);
                m_particleTypes[particleType->getName()] = particleType;
            }
        }
        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("could not load particles file %s: %s", file, e.what()));
        return false;
    }
}

ParticleEffectPtr ParticleManager::createEffect(const std::string& name)
{
    try {
        ParticleEffectPtr particleEffect = ParticleEffectPtr(new ParticleEffect);
        particleEffect->load(m_effectsTypes[name]);
        m_effects.push_back(particleEffect);
        return particleEffect;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("failed to create effect '%s': %s", name, e.what()));
        return nullptr;
    }
}

void ParticleManager::terminate()
{
    m_effects.clear();
    m_effectsTypes.clear();
    m_particleTypes.clear();
}

void ParticleManager::poll()
{
    for(auto it = m_effects.begin(); it != m_effects.end();) {
        const ParticleEffectPtr& particleEffect = *it;

        if(particleEffect->hasFinished()) {
            it = m_effects.erase(it);
        } else {
            particleEffect->update();
            ++it;
        }
    }
}
