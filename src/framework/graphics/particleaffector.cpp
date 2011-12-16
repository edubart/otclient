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
#include "particleaffector.h"
#include <framework/core/clock.h>

#define DEG_TO_RAD (acos(-1)/180.0)

bool GravityAffector::load(const OTMLNodePtr& node)
{
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

void GravityAffector::update(const ParticlePtr& particle, double elapsedTime)
{
    PointF velocity = particle->getVelocity();
    velocity += PointF(m_gravity * elapsedTime * cos(m_angle), m_gravity * elapsedTime * sin(m_angle));
    particle->setVelocity(velocity);
}

bool AttractionAffector::load(const OTMLNodePtr& node)
{
    m_acceleration = 32;

    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "destination")
            m_destination = childNode->value<Point>();
        else if(childNode->tag() == "acceleration")
            m_acceleration = childNode->value<float>();
    }
    return true;
}

void AttractionAffector::update(const ParticlePtr& particle, double elapsedTime)
{
    // must change velocity angle, keeping modulus.

    PointF pPosition = particle->getPosition();

    PointF d = PointF(m_destination.x - pPosition.x, pPosition.y - m_destination.y);
    if(d.length() == 0)
        return;

    PointF pVelocity = particle->getVelocity();
    particle->setVelocity(pVelocity + (d / d.length()) * m_acceleration * elapsedTime);
}
