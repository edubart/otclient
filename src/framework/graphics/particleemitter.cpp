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

Particle::Particle(const Rect& rect, float vx, float vy, float ax, float ay, float duration, const Color& color, TexturePtr texture)
{
    m_rect = rect;
    m_ix = rect.x(); m_iy = rect.y();
    m_vx = vx; m_vy = vy;
    m_ax = ax; m_ay = ay;
    m_color = color;
    m_texture = texture;
    m_duration = duration;
    m_startTicks = g_clock.ticks();
    m_finished = false;
}

Particle::~Particle()
{
    //dump << "deleted";
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
    ticks_t t = g_clock.ticks() - m_startTicks;

    // check if finished
    if(m_duration >= 0 && t > m_duration * 1000) {
        m_finished = true;
        return;
    }

    //update position
    m_rect.moveTo(m_ix + (m_vx * t / 1000.0) + (m_ax * t*t / (2.0 * 1000 * 1000)),
                  m_iy + (m_vy * t / 1000.0) + (m_ay * t*t / (2.0 * 1000 * 1000)));
}

ParticleEmitter::ParticleEmitter()
{
    m_position = Point(0, 0);
    m_duration = -1;
    m_burstRate = 1; m_burstCount = 32;
    m_currentBurst = 0;
    m_startTicks = g_clock.ticks();
    m_finished = false;

    // particles default configuration. (make them reasonable for user detect missing properties on scripts)
    m_pMinPositionRadius = 0; m_pMaxPositionRadius = 3;
    m_pMinPositionAngle = -Fw::pi; m_pMaxPositionAngle = Fw::pi;
    m_pMinSize = Size(32, 32); m_pMaxSize = Size(32, 32);
    m_pMinDuration = 0; m_pMaxDuration = 10;

    m_pMinVelocity = 32; m_pMaxVelocity = 64;
    m_pMinVelocityAngle = -Fw::pi; m_pMaxVelocityAngle = Fw::pi;

    m_pMinAcceleration = 32; m_pMaxAcceleration = 64;
    m_pMinAccelerationAngle = -Fw::pi; m_pMaxAccelerationAngle = Fw::pi;

    m_pColor = Color(255, 0, 0, 128);
    m_pTexture = nullptr;
}

bool ParticleEmitter::load(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& childNode : node->children()) {

        // self related
        if(childNode->tag() == "position") {
            std::string value = childNode->value();
            std::vector<std::string> split;
            boost::split(split, value, boost::is_any_of(std::string(" ")));
            if(split.size() == 2)
                m_position = Point(Fw::safeCast<int>(split[0]), Fw::safeCast<int>(split[1]));
        }
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
            m_pMinPositionAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;
            m_pMaxPositionAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;
        }
        else if(childNode->tag() == "particle-min-position-angle")
            m_pMinPositionAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;
        else if(childNode->tag() == "particle-max-position-angle")
            m_pMaxPositionAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;

        else if(childNode->tag() == "particle-velocity") {
            m_pMinVelocity = childNode->value<float>();
            m_pMaxVelocity = childNode->value<float>();
        }
        else if(childNode->tag() == "particle-min-velocity")
            m_pMinVelocity = childNode->value<float>();
        else if(childNode->tag() == "particle-max-velocity")
            m_pMaxVelocity = childNode->value<float>();

        else if(childNode->tag() == "particle-velocity-angle") {
            m_pMinVelocityAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;
            m_pMaxVelocityAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;
        }
        else if(childNode->tag() == "particle-min-velocity-angle")
            m_pMinVelocityAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;
        else if(childNode->tag() == "particle-max-velocity-angle")
            m_pMaxVelocityAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;

        else if(childNode->tag() == "particle-acceleration") {
            m_pMinAcceleration = childNode->value<float>();
            m_pMaxAcceleration = childNode->value<float>();
        }
        else if(childNode->tag() == "particle-min-acceleration")
            m_pMinAcceleration = childNode->value<float>();
        else if(childNode->tag() == "particle-max-acceleration")
            m_pMaxAcceleration = childNode->value<float>();

        else if(childNode->tag() == "particle-acceleration-angle") {
            m_pMinAccelerationAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;
            m_pMaxAccelerationAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;
        }
        else if(childNode->tag() == "particle-min-acceleration-angle")
            m_pMinAccelerationAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;
        else if(childNode->tag() == "particle-max-acceleration-angle")
            m_pMaxAccelerationAngle = (childNode->value<float>() * Fw::pi / 180.0) - Fw::pi;

        else if(childNode->tag() == "particle-duration") {
            m_pMinDuration = childNode->value<float>();
            m_pMaxDuration = childNode->value<float>();
        }
        else if(childNode->tag() == "particle-min-duration")
            m_pMinDuration = childNode->value<float>();
        else if(childNode->tag() == "particle-max-duration")
            m_pMaxDuration = childNode->value<float>();

        else if(childNode->tag() == "particle-size") {
            std::string value = childNode->value();
            std::vector<std::string> split;
            boost::split(split, value, boost::is_any_of(std::string(" ")));
            if(split.size() == 2) {
                m_pMinSize = Size(Fw::safeCast<int>(split[0]), Fw::safeCast<int>(split[1]));
                m_pMaxSize = Size(Fw::safeCast<int>(split[0]), Fw::safeCast<int>(split[1]));
            }
        }
        else if(childNode->tag() == "particle-min-size") {
            std::string value = childNode->value();
            std::vector<std::string> split;
            boost::split(split, value, boost::is_any_of(std::string(" ")));
            if(split.size() == 2) {
                m_pMinSize = Size(Fw::safeCast<int>(split[0]), Fw::safeCast<int>(split[1]));
            }
        }
        else if(childNode->tag() == "particle-max-size") {
            std::string value = childNode->value();
            std::vector<std::string> split;
            boost::split(split, value, boost::is_any_of(std::string(" ")));
            if(split.size() == 2) {
                m_pMaxSize = Size(Fw::safeCast<int>(split[0]), Fw::safeCast<int>(split[1]));
            }
        }
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
    ticks_t elapsedTicks = g_clock.ticks() - m_startTicks;

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
    if(m_duration >= 0 && elapsedTicks > m_duration * 1000) {
        // stop emitting but only self remove when there are no particles left
        if(m_particles.size() == 0)
            m_finished = true;
        return;
    }

    int currentBurst = elapsedTicks / 1000.0 / m_burstRate + 1;
    for(int b = m_currentBurst; b < currentBurst; ++b) {

        // every burst created at same position.
        float pRadius = Fw::randomRange(m_pMinPositionRadius, m_pMaxPositionRadius);
        float pAngle = Fw::randomRange(m_pMinPositionAngle, m_pMaxPositionAngle);
        Point pPosition = Point(-pRadius * cos(pAngle), pRadius * sin(pAngle));

        for(int p = 0; p < m_burstCount; ++p) {

            Size pSize = Size(Fw::randomRange(m_pMinSize.width(), m_pMaxSize.width()), Fw::randomRange(m_pMinSize.height(), m_pMaxSize.height()));
            float pDuration = Fw::randomRange(m_pMinDuration, m_pMaxDuration);

            // particles initial velocity
            float pVelocity = Fw::randomRange(m_pMinVelocity, m_pMaxVelocity);
            float pVelocityAngle = Fw::randomRange(m_pMinVelocityAngle, m_pMaxVelocityAngle);

            // particles initial acceleration
            float pAcceleration = Fw::randomRange(m_pMinAcceleration, m_pMaxAcceleration);
            float pAccelerationAngle = Fw::randomRange(m_pMinAccelerationAngle, m_pMaxAccelerationAngle);

            m_particles.push_back(ParticlePtr(new Particle(Rect(m_position + pPosition, pSize),
                                                           -pVelocity * cos(pVelocityAngle), pVelocity * sin(pVelocityAngle),
                                                           -pAcceleration * cos(pAccelerationAngle), pAcceleration * sin(pAccelerationAngle),
                                                           pDuration, m_pColor, m_pTexture)));
        }
    }

    m_currentBurst = currentBurst;
}
