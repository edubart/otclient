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

#include "particle.h"
#include "particleemitter.h"
#include "particlesystem.h"
#include <framework/core/clock.h>
#include <framework/graphics/texturemanager.h>
#include "particlemanager.h"

ParticleEmitter::ParticleEmitter()
{
    m_position = Point(0, 0);
    m_duration = -1;
    m_delay = 0;
    m_burstRate = 1;
    m_burstCount = 32;
    m_currentBurst = 0;
    m_elapsedTime = 0;
    m_finished = false;
    m_active = false;
}

void ParticleEmitter::load(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& childNode : node->children()) {
        // self related
        if(childNode->tag() == "position")
            m_position = childNode->value<Point>();
        else if(childNode->tag() == "duration")
            m_duration = childNode->value<float>();
        else if(childNode->tag() == "delay")
            m_delay = childNode->value<float>();
        else if(childNode->tag() == "burst-rate")
            m_burstRate = childNode->value<float>();
        else if(childNode->tag() == "burst-count")
            m_burstCount = childNode->value<int>();
        else if(childNode->tag() == "particle-type")
            m_particleType = g_particles.getParticleType(childNode->value());
    }

    if(!m_particleType)
        stdext::throw_exception("emitter didn't provide a valid particle type");
}

void ParticleEmitter::update(float elapsedTime, const ParticleSystemPtr& system)
{
    m_elapsedTime += elapsedTime;

    // check if finished
    if(m_duration > 0 && m_elapsedTime >= m_duration + m_delay) {
        m_finished = true;
        return;
    }

    if(!m_active && m_elapsedTime > m_delay)
        m_active = true;

    if(!m_active)
        return;

    int nextBurst = std::floor((m_elapsedTime - m_delay) * m_burstRate) + 1;
    const ParticleType *type = m_particleType.get();
    for(int b = m_currentBurst; b < nextBurst; ++b) {
        // every burst created at same position.
        float pRadius = stdext::random_range(type->pMinPositionRadius, type->pMaxPositionRadius);
        float pAngle = stdext::random_range(type->pMinPositionAngle, type->pMaxPositionAngle);

        Point pPosition = m_position + Point(pRadius * std::cos(pAngle), pRadius * std::sin(pAngle));

        for(int p = 0; p < m_burstCount; ++p) {
            float pDuration = stdext::random_range(type->pMinDuration, type->pMaxDuration);

            // particles initial velocity
            float pVelocityAbs = stdext::random_range(type->pMinVelocity, type->pMaxVelocity);
            float pVelocityAngle = stdext::random_range(type->pMinVelocityAngle, type->pMaxVelocityAngle);
            PointF pVelocity(pVelocityAbs * std::cos(pVelocityAngle), pVelocityAbs * std::sin(pVelocityAngle));

            // particles initial acceleration
            float pAccelerationAbs = stdext::random_range(type->pMinAcceleration, type->pMaxAcceleration);
            float pAccelerationAngle = stdext::random_range(type->pMinAccelerationAngle, type->pMaxAccelerationAngle);
            PointF pAcceleration(pAccelerationAbs * std::cos(pAccelerationAngle), pAccelerationAbs * std::sin(pAccelerationAngle));

            ParticlePtr particle(new Particle(pPosition, type->pStartSize, type->pFinalSize,
                                                pVelocity, pAcceleration,
                                                pDuration, type->pIgnorePhysicsAfter,
                                                type->pColors, type->pColorsStops,
                                                type->pCompositionMode, type->pTexture));
            system->addParticle(particle);
        }
    }

    m_currentBurst = nextBurst;
}
