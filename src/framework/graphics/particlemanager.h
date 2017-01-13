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

#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

#include "declarations.h"
#include "particleeffect.h"
#include "particletype.h"

class ParticleManager
{
public:
    bool importParticle(std::string file);
    ParticleEffectPtr createEffect(const std::string& name);
    void terminate();

    void poll();

    ParticleTypePtr getParticleType(std::string name) { return m_particleTypes[name]; }
    ParticleEffectTypePtr getParticleEffectType(std::string name) { return m_effectsTypes[name]; }

    const std::map<std::string, ParticleTypePtr>& getParticleTypes() { return m_particleTypes; }
    const std::map<std::string, ParticleEffectTypePtr>& getEffectsTypes() { return m_effectsTypes; }

private:
    std::list<ParticleEffectPtr> m_effects;
    std::map<std::string, ParticleEffectTypePtr> m_effectsTypes;
    std::map<std::string, ParticleTypePtr> m_particleTypes;
};

extern ParticleManager g_particles;

#endif
