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

void ParticleAffector::update(double elapsedTime)
{
    if(m_duration >= 0 && m_elapsedTime >= m_duration + m_delay) {
        m_finished = true;
        return;
    }

    if(!m_active && m_elapsedTime > m_delay)
        m_active = true;

    m_elapsedTime += elapsedTime;
}

bool ParticleAffector::load(const OTMLNodePtr& node)
{
    float minDelay = 0, maxDelay = 0;
    float minDuration = -1, maxDuration = -1;

    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "delay") {
            minDelay = childNode->value<float>();
            maxDelay = childNode->value<float>();
        }
        else if(childNode->tag() == "min-delay")
            minDelay = childNode->value<float>();
        else if(childNode->tag() == "max-delay")
            maxDelay = childNode->value<float>();

        if(childNode->tag() == "duration") {
            minDuration = childNode->value<float>();
            maxDuration = childNode->value<float>();
        }
        else if(childNode->tag() == "min-duration")
            minDuration = childNode->value<float>();
        else if(childNode->tag() == "max-duration")
            maxDuration = childNode->value<float>();
    }

    m_delay = Fw::randomRange(minDelay, maxDelay);
    m_duration = Fw::randomRange(minDuration, maxDuration);

    return true;
}

bool GravityAffector::load(const OTMLNodePtr& node)
{
    if(!ParticleAffector::load(node))
        return false;

    m_angle = 270 * DEG_TO_RAD;
    m_gravity = 9.8;

    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "angle")
            m_angle = childNode->value<float>() * DEG_TO_RAD;
        else if(childNode->tag() == "gravity")
            m_gravity = childNode->value<float>();
    }
    return true;
}

void GravityAffector::updateParticle(const ParticlePtr& particle, double elapsedTime)
{
    if(!m_active)
        return;

    PointF velocity = particle->getVelocity();
    velocity += PointF(m_gravity * elapsedTime * cos(m_angle), m_gravity * elapsedTime * sin(m_angle));
    particle->setVelocity(velocity);
}

bool AttractionAffector::load(const OTMLNodePtr& node)
{
    if(!ParticleAffector::load(node))
        return false;

    m_acceleration = 32;
    m_reduction = 0;
    m_repelish = false;

    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "position")
            m_pos = childNode->value<Point>();
        else if(childNode->tag() == "acceleration")
            m_acceleration = childNode->value<float>();
        else if(childNode->tag() == "velocity-reduction-percent")
            m_reduction = childNode->value<float>();
        else if(childNode->tag() == "repelish")
            m_repelish = childNode->value<bool>();
    }
    return true;
}

void AttractionAffector::updateParticle(const ParticlePtr& particle, double elapsedTime)
{
    if(!m_active)
        return;

    PointF pPosition = particle->getPos();
    PointF d = PointF(m_pos.x - pPosition.x, pPosition.y - m_pos.y);
    if(d.length() == 0)
        return;

    PointF direction = PointF(1, 1);
    if(m_repelish)
        direction = PointF(-1, -1);

    PointF pVelocity = particle->getVelocity() + (d / d.length() * m_acceleration * elapsedTime) * direction;
    particle->setVelocity(pVelocity - pVelocity * m_reduction/100.0 * elapsedTime);
}
