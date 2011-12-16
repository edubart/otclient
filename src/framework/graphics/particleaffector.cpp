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

void Gravity270Affector::update(const ParticlePtr& particle, double elapsedTime)
{
    // earth gravity is 9.8 m/s². -> in tibia, 32 pixels are equal to 1 meter -> 32 pixels/m -> 9.8 * 32 is gravity constant
    const float gravity = 9.8 * 32;

    PointF velocity = particle->getVelocity();
    velocity += PointF(0, -gravity * elapsedTime);

    particle->setVelocity(velocity);
}

void Gravity315Affector::update(const ParticlePtr& particle, double elapsedTime)
{
    // earth gravity is 9.8 m/s². -> in tibia, 32 pixels are equal to 1 meter -> 32 pixels/m -> 9.8 * 32 is gravity constant
    const float gravity = 9.8 * 32;

    PointF velocity = particle->getVelocity();
    velocity += PointF(gravity * elapsedTime * cos(7 * Fw::pi / 4), gravity * elapsedTime * sin(7 * Fw::pi / 4));

    particle->setVelocity(velocity);
}

bool GoToPointAffector::load(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "destination")
            m_destination = childNode->value<Point>();
        else if(childNode->tag() == "rotate-speed-percent")
            m_rotateSpeedPercent = childNode->value<float>();
    }
    return true;
}

void GoToPointAffector::update(const ParticlePtr& particle, double elapsedTime)
{
    // must change velocity angle, keeping modulus.



}
