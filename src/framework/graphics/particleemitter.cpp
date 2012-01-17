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
#include "particleemitter.h"
#include "particlesystem.h"
#include <framework/core/clock.h>
#include <framework/graphics/texturemanager.h>
#include <framework/util/tools.h>

ParticleEmitter::ParticleEmitter(const ParticleSystemPtr& parent)
{
    m_parent = parent;

    m_pos = Point(0, 0);
    m_duration = -1;
    m_delay = 0;
    m_burstRate = 1; m_burstCount = 32;
    m_currentBurst = 0;
    m_elapsedTime = 0;
    m_finished = false;
    m_active = false;

    // particles default configuration. (make them reasonable for user detect missing properties on scripts)
    m_pMinPositionRadius = 0;
    m_pMaxPositionRadius = 3;
    m_pMinPositionAngle = 0;
    m_pMaxPositionAngle = 360;
    m_pStartSize = Size(32, 32);
    m_pFinalSize = Size(32, 32);
    m_pMinDuration = 0;
    m_pMaxDuration = 10;
    m_pIgnorePhysicsAfter = -1;
    m_pMinVelocity = 32;
    m_pMaxVelocity = 64;
    m_pMinVelocityAngle = 0;
    m_pMaxVelocityAngle = 360;
    m_pMinAcceleration = 32;
    m_pMaxAcceleration = 64;
    m_pMinAccelerationAngle = 0;
    m_pMaxAccelerationAngle = 360;
}

bool ParticleEmitter::load(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& childNode : node->children()) {
        // self related
        if(childNode->tag() == "position")
            m_pos = childNode->value<Point>();
        else if(childNode->tag() == "duration")
            m_duration = childNode->value<float>();
        else if(childNode->tag() == "delay")
            m_delay = childNode->value<float>();
        else if(childNode->tag() == "burstRate")
            m_burstRate = childNode->value<float>();
        else if(childNode->tag() == "burstCount")
            m_burstCount = childNode->value<int>();

        // particles generation related
        else if(childNode->tag() == "particle-position-radius") {
            m_pMinPositionRadius = childNode->value<float>();
            m_pMaxPositionRadius = childNode->value<float>();
        }
        else if(childNode->tag() == "particle-min-position-radius")
            m_pMinPositionRadius = childNode->value<float>();
        else if(childNode->tag() == "particle-max-position-radius")
            m_pMaxPositionRadius = childNode->value<float>();
        else if(childNode->tag() == "particle-position-angle") {
            m_pMinPositionAngle = childNode->value<float>() * DEG_TO_RAD;
            m_pMaxPositionAngle = childNode->value<float>() * DEG_TO_RAD;
        }
        else if(childNode->tag() == "particle-min-position-angle")
            m_pMinPositionAngle = childNode->value<float>() * DEG_TO_RAD;
        else if(childNode->tag() == "particle-max-position-angle")
            m_pMaxPositionAngle = childNode->value<float>() * DEG_TO_RAD;

        // velocity
        else if(childNode->tag() == "particle-velocity") {
            m_pMinVelocity = childNode->value<float>();
            m_pMaxVelocity = childNode->value<float>();
        }
        else if(childNode->tag() == "particle-min-velocity")
            m_pMinVelocity = childNode->value<float>();
        else if(childNode->tag() == "particle-max-velocity")
            m_pMaxVelocity = childNode->value<float>();
        else if(childNode->tag() == "particle-velocity-angle") {
            m_pMinVelocityAngle = childNode->value<float>() * DEG_TO_RAD;
            m_pMaxVelocityAngle = childNode->value<float>() * DEG_TO_RAD;
        }
        else if(childNode->tag() == "particle-min-velocity-angle")
            m_pMinVelocityAngle = childNode->value<float>() * DEG_TO_RAD;
        else if(childNode->tag() == "particle-max-velocity-angle")
            m_pMaxVelocityAngle = childNode->value<float>() * DEG_TO_RAD;
        else if(childNode->tag() == "particle-acceleration") {
            m_pMinAcceleration = childNode->value<float>();
            m_pMaxAcceleration = childNode->value<float>();
        }

        // acceleration
        else if(childNode->tag() == "particle-min-acceleration")
            m_pMinAcceleration = childNode->value<float>();
        else if(childNode->tag() == "particle-max-acceleration")
            m_pMaxAcceleration = childNode->value<float>();
        else if(childNode->tag() == "particle-acceleration-angle") {
            m_pMinAccelerationAngle = childNode->value<float>() * DEG_TO_RAD;
            m_pMaxAccelerationAngle = childNode->value<float>() * DEG_TO_RAD;
        }
        else if(childNode->tag() == "particle-min-acceleration-angle")
            m_pMinAccelerationAngle = childNode->value<float>() * DEG_TO_RAD;
        else if(childNode->tag() == "particle-max-acceleration-angle")
            m_pMaxAccelerationAngle = childNode->value<float>() * DEG_TO_RAD;

        // duration
        else if(childNode->tag() == "particle-duration") {
            m_pMinDuration = childNode->value<float>();
            m_pMaxDuration = childNode->value<float>();
        }
        else if(childNode->tag() == "particle-min-duration")
            m_pMinDuration = childNode->value<float>();
        else if(childNode->tag() == "particle-max-duration")
            m_pMaxDuration = childNode->value<float>();
        else if(childNode->tag() == "particle-ignore-physics-after")
            m_pIgnorePhysicsAfter = childNode->value<float>();

        // visual
        else if(childNode->tag() == "particle-size") {
            m_pStartSize = childNode->value<Size>();
            m_pFinalSize = childNode->value<Size>();
        }
        else if(childNode->tag() == "particle-start-size")
            m_pStartSize = childNode->value<Size>();
        else if(childNode->tag() == "particle-final-size")
            m_pFinalSize = childNode->value<Size>();

        else if(childNode->tag() == "particle-colors")
            m_pColors = Fw::split<Color>(childNode->value());
        else if(childNode->tag() == "particle-colors-stops")
            m_pColorsStops = Fw::split<float>(childNode->value());
        else if(childNode->tag() == "particle-texture")
            m_pTexture = g_textures.getTexture(childNode->value());
        else if(childNode->tag() == "particle-composition-mode") {
            if(childNode->value() == "normal")
                m_pCompositionMode = Painter::CompositionMode_Normal;
            else if(childNode->value() == "multiply")
                m_pCompositionMode = Painter::CompositionMode_Multiply;
            else if(childNode->value() == "addition")
                m_pCompositionMode = Painter::CompositionMode_Add;
        }
    }

    if(m_pColors.empty())
        m_pColors.push_back(Color(255, 255, 255, 128));
    if(m_pColorsStops.empty())
        m_pColorsStops.push_back(0);

    if(m_pColors.size() != m_pColorsStops.size()) {
        logError("particle colors must be equal to colorstops-1");
        return false;
    }

    return true;
}

void ParticleEmitter::update(double elapsedTime)
{
    // check if finished
    if(m_duration >= 0 && m_elapsedTime >= m_duration + m_delay) {
        m_finished = true;
        return;
    }

    if(!m_active && m_elapsedTime > m_delay)
        m_active = true;

    if(m_active) {
        int currentBurst = std::floor((m_elapsedTime - m_delay) / m_burstRate) + 1;
        for(int b = m_currentBurst; b < currentBurst; ++b) {

            // every burst created at same position.
            float pRadius = Fw::randomRange(m_pMinPositionRadius, m_pMaxPositionRadius);
            float pAngle = Fw::randomRange(m_pMinPositionAngle, m_pMaxPositionAngle);

            Point pPosition = m_pos + Point(pRadius * cos(pAngle), pRadius * sin(pAngle));

            for(int p = 0; p < m_burstCount; ++p) {

                float pDuration = Fw::randomRange(m_pMinDuration, m_pMaxDuration);

                // particles initial velocity
                float pVelocityAbs = Fw::randomRange(m_pMinVelocity, m_pMaxVelocity);
                float pVelocityAngle = Fw::randomRange(m_pMinVelocityAngle, m_pMaxVelocityAngle);
                PointF pVelocity(pVelocityAbs * cos(pVelocityAngle), pVelocityAbs * sin(pVelocityAngle));

                // particles initial acceleration
                float pAccelerationAbs = Fw::randomRange(m_pMinAcceleration, m_pMaxAcceleration);
                float pAccelerationAngle = Fw::randomRange(m_pMinAccelerationAngle, m_pMaxAccelerationAngle);
                PointF pAcceleration(pAccelerationAbs * cos(pAccelerationAngle), pAccelerationAbs * sin(pAccelerationAngle));

                ParticleSystemPtr particleSystem = m_parent.lock();
                particleSystem->addParticle(ParticlePtr(new Particle(pPosition, m_pStartSize, m_pFinalSize, pVelocity, pAcceleration, pDuration, m_pIgnorePhysicsAfter, m_pColors, m_pColorsStops, m_pCompositionMode, m_pTexture)));
            }
        }

        m_currentBurst = currentBurst;
    }

    m_elapsedTime += elapsedTime;
}
