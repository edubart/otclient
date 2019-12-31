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

#ifndef PARTICLEAFFECTOR_H
#define PARTICLEAFFECTOR_H

#include "declarations.h"
#include <framework/otml/otml.h>

class ParticleAffector : public stdext::shared_object
{
public:
    ParticleAffector();

    void update(float elapsedTime);
    virtual void load(const OTMLNodePtr& node);
    virtual void updateParticle(const ParticlePtr&, float) {}

    bool hasFinished() { return m_finished; }

protected:
    bool m_finished, m_active;
    float m_delay, m_duration;
    float m_elapsedTime;
};

class GravityAffector : public ParticleAffector {
public:
    void load(const OTMLNodePtr& node);
    void updateParticle(const ParticlePtr& particle, float elapsedTime);

private:
    float m_angle, m_gravity;
};

class AttractionAffector : public ParticleAffector {
public:
    void load(const OTMLNodePtr& node);
    void updateParticle(const ParticlePtr& particle, float elapsedTime);

private:
    Point m_position;
    float m_acceleration, m_reduction;
    bool m_repelish;
};

#endif
