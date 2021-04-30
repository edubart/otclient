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

#include "particle.h"
#include "particleaffector.h"
#include <framework/core/clock.h>

ParticleAffector::ParticleAffector()
{
    m_active = false;
    m_finished = false;
    m_delay = 0;
    m_duration = 0;
    m_elapsedTime = 0;
}

void ParticleAffector::update(float elapsedTime)
{
    if(m_duration >= 0 && m_elapsedTime >= m_duration + m_delay) {
        m_finished = true;
        return;
    }

    if(!m_active && m_elapsedTime > m_delay)
        m_active = true;

    m_elapsedTime += elapsedTime;
}

void ParticleAffector::load(const OTMLNodePtr& node)
{
    float minDelay = 0, maxDelay = 0;
    float minDuration = -1, maxDuration = -1;

    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "delay") {
            minDelay = childNode->value<float>();
            maxDelay = childNode->value<float>();
        } else if(childNode->tag() == "min-delay")
            minDelay = childNode->value<float>();
        else if(childNode->tag() == "max-delay")
            maxDelay = childNode->value<float>();
        else if(childNode->tag() == "duration") {
            minDuration = childNode->value<float>();
            maxDuration = childNode->value<float>();
        } else if(childNode->tag() == "min-duration")
            minDuration = childNode->value<float>();
        else if(childNode->tag() == "max-duration")
            maxDuration = childNode->value<float>();
    }

    m_delay = stdext::random_range(minDelay, maxDelay);
    m_duration = stdext::random_range(minDuration, maxDuration);
}

void GravityAffector::load(const OTMLNodePtr& node)
{
    ParticleAffector::load(node);

    m_angle = 270 * DEG_TO_RAD;
    m_gravity = 9.8f;

    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "angle")
            m_angle = childNode->value<float>() * DEG_TO_RAD;
        else if(childNode->tag() == "gravity")
            m_gravity = childNode->value<float>();
    }
}

void GravityAffector::updateParticle(const ParticlePtr& particle, float elapsedTime)
{
    if(!m_active)
        return;

    PointF velocity = particle->getVelocity();
    velocity += PointF(m_gravity * elapsedTime * std::cos(m_angle), m_gravity * elapsedTime * std::sin(m_angle));
    particle->setVelocity(velocity);
}

void AttractionAffector::load(const OTMLNodePtr& node)
{
    ParticleAffector::load(node);

    m_acceleration = 32;
    m_reduction = 0;
    m_repelish = false;

    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "position")
            m_position = childNode->value<Point>();
        else if(childNode->tag() == "acceleration")
            m_acceleration = childNode->value<float>();
        else if(childNode->tag() == "velocity-reduction-percent")
            m_reduction = childNode->value<float>();
        else if(childNode->tag() == "repelish")
            m_repelish = childNode->value<bool>();
    }
}

void AttractionAffector::updateParticle(const ParticlePtr& particle, float elapsedTime)
{
    if(!m_active)
        return;

    PointF pPosition = particle->getPosition();
    PointF d = PointF(m_position.x - pPosition.x, pPosition.y - m_position.y);
    if(d.length() == 0)
        return;

    PointF direction = PointF(1, 1);
    if(m_repelish)
        direction = PointF(-1, -1);

    PointF pVelocity = particle->getVelocity() + (d / d.length() * m_acceleration * elapsedTime) * direction;
    particle->setVelocity(pVelocity - pVelocity * m_reduction/100.f * elapsedTime);
}
