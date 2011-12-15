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

#include "particleemitter.h"
#include "graphics.h"
#include <framework/core/clock.h>
#include <framework/graphics/texturemanager.h>
#include <framework/util/tools.h>

#define DEG_TO_RAD (acos(-1)/180.0)

Particle::Particle(const Point& pos, const Size& size, const PointF& velocity, const PointF& acceleration, float duration, const Color& color, TexturePtr texture)
{
    m_rect = Rect(pos, size);
    m_pos = PointF(pos.x, pos.y);
    m_size = size;
    m_velocity = velocity;
    m_acceleration = acceleration;
    m_color = color;
    m_texture = texture;
    m_duration = duration;
    m_startTime = g_clock.time();
    m_lastUpdateTime = g_clock.time();
    m_finished = false;
}

void Particle::render()
{
    g_painter.setColor(m_color);

    if(!m_texture)
        g_painter.drawFilledRect(m_rect);
    else {
        //g_painter.setCompositionMode(Painter::CompositionMode_AdditiveSource);
        g_painter.drawTexturedRect(m_rect, m_texture);
        //g_painter.setCompositionMode(Painter::CompositionMode_SourceOver);
    }
}

void Particle::update()
{
    float elapsedTime = g_clock.timeElapsed(m_lastUpdateTime);
    m_lastUpdateTime = g_clock.time();

    // check if finished
    if(m_duration > 0 && g_clock.timeElapsed(m_startTime) >= m_duration) {
        m_finished = true;
        return;
    }

    // update position
    PointF delta = m_velocity * elapsedTime;
    delta.y *= -1; // painter orientate Y axis in the inverse direction
    m_pos += delta;

    // update acceleration
    m_velocity += m_acceleration * elapsedTime;

    m_rect.moveTo((int)m_pos.x, (int)m_pos.y);
}

ParticleEmitter::ParticleEmitter()
{
    m_position = Point(0, 0);
    m_duration = -1;
    m_burstRate = 1; m_burstCount = 32;
    m_currentBurst = 0;
    m_startTime = g_clock.time();
    m_finished = false;

    // particles default configuration. (make them reasonable for user detect missing properties on scripts)
    m_pMinPositionRadius = 0;
    m_pMaxPositionRadius = 3;
    m_pMinPositionAngle = 0;
    m_pMaxPositionAngle = 360;
    m_pMinSize = Size(32, 32);
    m_pMaxSize = Size(32, 32);
    m_pMinDuration = 0;
    m_pMaxDuration = 10;
    m_pMinVelocity = 32;
    m_pMaxVelocity = 64;
    m_pMinVelocityAngle = 0;
    m_pMaxVelocityAngle = 360;
    m_pMinAcceleration = 32;
    m_pMaxAcceleration = 64;
    m_pMinAccelerationAngle = 0;
    m_pMaxAccelerationAngle = 360;
    m_pColor = Color(255, 255, 255, 128);
}

bool ParticleEmitter::load(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& childNode : node->children()) {
        // self related
        if(childNode->tag() == "position")
            m_position = childNode->value<Point>();
        else if(childNode->tag() == "duration")
            m_duration = childNode->value<float>();
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
        else if(childNode->tag() == "particle-duration") {
            m_pMinDuration = childNode->value<float>();
            m_pMaxDuration = childNode->value<float>();
        }
        else if(childNode->tag() == "particle-min-duration")
            m_pMinDuration = childNode->value<float>();
        else if(childNode->tag() == "particle-max-duration")
            m_pMaxDuration = childNode->value<float>();
        else if(childNode->tag() == "particle-size") {
            m_pMinSize = childNode->value<Size>();
            m_pMaxSize = childNode->value<Size>();
        }
        else if(childNode->tag() == "particle-min-size")
            m_pMinSize = childNode->value<Size>();
        else if(childNode->tag() == "particle-max-size")
            m_pMaxSize = childNode->value<Size>();
        else if(childNode->tag() == "particle-color")
            m_pColor = childNode->value<Color>();
        else if(childNode->tag() == "particle-texture")
            m_pTexture = g_textures.getTexture(childNode->value());
    }
    return true;
}

void ParticleEmitter::render()
{
    for(auto it = m_particles.begin(), end = m_particles.end(); it != end; ++it)
        (*it)->render();
}

void ParticleEmitter::update()
{
    float elapsedTime = g_clock.timeElapsed(m_startTime);

    // update particles
    for(auto it = m_particles.begin(), end = m_particles.end(); it != end;) {
        const ParticlePtr& particle = *it;
        if(particle->hasFinished()) {
            it = m_particles.erase(it);
            continue;
        }
        particle->update();
        ++it;
    }

    // check if finished
    if(m_duration > 0 && elapsedTime > m_duration) {
        // stop emitting but only self remove when there are no particles left
        if(m_particles.size() == 0)
            m_finished = true;
        return;
    }

    int currentBurst = (elapsedTime / m_burstRate) + 1;
    for(int b = m_currentBurst; b < currentBurst; ++b) {

        // every burst created at same position.
        float pRadius = Fw::randomRange(m_pMinPositionRadius, m_pMaxPositionRadius);
        float pAngle = Fw::randomRange(m_pMinPositionAngle, m_pMaxPositionAngle);
        Point pPosition = m_position + Point(pRadius * cos(pAngle), pRadius * sin(pAngle));

        for(int p = 0; p < m_burstCount; ++p) {

            Size pSize = Size(Fw::randomRange(m_pMinSize.width(), m_pMaxSize.width()), Fw::randomRange(m_pMinSize.height(), m_pMaxSize.height()));
            float pDuration = Fw::randomRange(m_pMinDuration, m_pMaxDuration);

            // particles initial velocity
            float pVelocityAbs = Fw::randomRange(m_pMinVelocity, m_pMaxVelocity);
            float pVelocityAngle = Fw::randomRange(m_pMinVelocityAngle, m_pMaxVelocityAngle);
            PointF pVelocity(pVelocityAbs * cos(pVelocityAngle), pVelocityAbs * sin(pVelocityAngle));

            // particles initial acceleration
            float pAccelerationAbs = Fw::randomRange(m_pMinAcceleration, m_pMaxAcceleration);
            float pAccelerationAngle = Fw::randomRange(m_pMinAccelerationAngle, m_pMaxAccelerationAngle);
            PointF pAcceleration(pAccelerationAbs * cos(pAccelerationAngle), pAccelerationAbs * sin(pAccelerationAngle));

            m_particles.push_back(ParticlePtr(new Particle(pPosition, pSize, pVelocity, pAcceleration, pDuration, m_pColor, m_pTexture)));
        }
    }

    m_currentBurst = currentBurst;
}
