#include "particlessystem.h"
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

ParticleEmitter::ParticleEmitter(const Point& position, float duration, float burstRate, float burstCount)
{
    m_position = position;
    m_duration = duration;
    m_burstRate = burstRate; m_burstCount = burstCount;
    m_currentBurst = 0;
    m_startTicks = g_clock.ticks();
    m_finished = false;

    // particles default configuration. (make them reasonable for user detect missing properties on scripts)
    m_pPositionMinRadius = 0; m_pPositionMaxRadius = 3;
    m_pPositionMinAngle = -Fw::pi; m_pPositionMaxAngle = Fw::pi;
    m_pMinSize = Size(32, 32); m_pMaxSize = Size(32, 32);
    m_pMinDuration = 0; m_pMaxDuration = 10;

    m_pMinVelocity = 32; m_pMaxVelocity = 64;
    m_pMinVelocityAngle = -Fw::pi; m_pMaxVelocityAngle = Fw::pi;

    m_pMinAcceleration = 32; m_pMaxAcceleration = 64;
    m_pMinAccelerationAngle = -Fw::pi; m_pMaxAccelerationAngle = Fw::pi;

    m_pColor = Color(255, 0, 0, 128);
    m_pTexture = nullptr;
}

void ParticleEmitter::render()
{
    for(auto it = m_particles.begin(), end = m_particles.end(); it != end; ++it)
        (*it)->render();
}

void ParticleEmitter::update()
{
    ticks_t elapsedTicks = g_clock.ticks() - m_startTicks;

    // check if finished
    if(m_duration >= 0 && elapsedTicks > m_duration * 1000) {
        m_finished = true;
        return;
    }

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

    // create some particles
    m_pTexture = g_textures.getTexture("circle2.png"); // remove this, 'll be parsed on loader

    int currentBurst = elapsedTicks / 1000.0 / m_burstRate + 1;
    for(int b = m_currentBurst; b < currentBurst; ++b) {

        // every burst created at same position.
        float pRadius = Fw::randomRange(m_pPositionMinRadius, m_pPositionMaxRadius);
        float pAngle = Fw::randomRange(m_pPositionMinAngle, m_pPositionMaxAngle);
        Point pPosition = Point(pRadius * cos(pAngle), pRadius * sin(pAngle));

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
                                                           pVelocity * cos(pVelocityAngle), pVelocity * sin(pVelocityAngle),
                                                           pAcceleration * cos(pAccelerationAngle), pAcceleration * sin(pAccelerationAngle),
                                                           pDuration, m_pColor, m_pTexture)));
        }
    }

    m_currentBurst = currentBurst;
}

void ParticlesSystem::add(const ParticleEmitterPtr& emitter)
{
    m_emitters.push_back(emitter);
}

void ParticlesSystem::render()
{
    for(auto it = m_emitters.begin(), end = m_emitters.end(); it != end; ++it)
        (*it)->render();
}

void ParticlesSystem::update()
{
    for(auto it = m_emitters.begin(), end = m_emitters.end(); it != end;) {
        const ParticleEmitterPtr& emitter = *it;
        if(emitter->hasFinished()) {
            it = m_emitters.erase(it);
            continue;
        }
        emitter->update();
        ++it;
    }
}
