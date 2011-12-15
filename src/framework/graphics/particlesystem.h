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

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "declarations.h"
#include "particleemitter.h"

class Affector {
public:
    virtual void update() {}
};

class Gravity270Affector : public Affector {
public:
    void update() {
        // earth gravity is 9.8 m/s². -> in tibia, 32 pixels are equal to 1 meter -> 32 pixels/m -> 9.8 * 32 is gravity constant
    }
};

class ParticleSystem : public std::enable_shared_from_this<ParticleSystem> {
public:
    ParticleSystem();
    ~ParticleSystem() { dump << "ParticleSystem deleted"; }

    bool load(const OTMLNodePtr& node);

    void addParticle(const ParticlePtr& particle);

    void render();
    void update();

    bool hasFinished() { return m_finished; }

private:
    bool m_finished;
    std::list<ParticlePtr> m_particles;
    std::list<ParticleEmitterPtr> m_emitters;
    std::list<Affector> m_affectors;
};

#endif
