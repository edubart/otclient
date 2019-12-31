/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "declarations.h"
#include "particle.h"
#include "particleemitter.h"
#include "particleaffector.h"

class ParticleSystem : public stdext::shared_object {
public:
    ParticleSystem();

    void load(const OTMLNodePtr& node);

    void addParticle(const ParticlePtr& particle);

    void render();
    void update();

    bool hasFinished() { return m_finished; }

private:
    bool m_finished;
    float m_lastUpdateTime;
    std::list<ParticlePtr> m_particles;
    std::list<ParticleEmitterPtr> m_emitters;
    std::list<ParticleAffectorPtr> m_affectors;
};

#endif
