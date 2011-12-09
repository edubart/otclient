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
    dump << "deleted";
}

void Particle::render()
{
    g_graphics.bindColor(m_color);

    if(!m_texture)
        g_graphics.drawFilledRect(m_rect);
    else {
        g_graphics.bindBlendFunc(Fw::BlendParticles);
        g_graphics.drawTexturedRect(m_rect, m_texture);
        g_graphics.bindBlendFunc(Fw::BlendDefault);
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

ParticleEmitter::ParticleEmitter(const Point& position, float duration, int particlesPerSecond)
{
    m_position = position;
    m_duration = duration;
    m_particlesPerSecond = particlesPerSecond;
    m_createdParticles = 0;
    m_startTicks = g_clock.ticks();
    m_finished = false;

    // particles default configuration. (make them reasonable detect missing properties on scripts)
    m_pPositionMinRadius = 0; m_pPositionMaxRadius = 3;
    m_pPositionMinAngle = 0; m_pPositionMaxAngle = 90;
    m_pMinSize = Size(32, 32); m_pMaxSize = Size(32, 32);
    m_pMinDuration = 0; m_pMaxDuration = 10;
}

void ParticleEmitter::render()
{
    // testing
    //g_graphics.bindColor(Color(255, 255, 255));
    //g_graphics.drawFilledRect(Rect(0, 0, 400, 400));

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
    TexturePtr tex = g_textures.getTexture("circle2.png");

    int currentParticles = 1 + elapsedTicks / 1000.0 * m_particlesPerSecond;
    for(int i = m_createdParticles; i < currentParticles; ++i) {

        // \/ not working properly
        float pRadius = Fw::randomRange(m_pPositionMinRadius, m_pPositionMaxRadius);
        float pAngle = Fw::randomRange(m_pPositionMinAngle, m_pPositionMaxAngle) * 3.141592 / 180.0;

        Point pPosition = Point(cos(pAngle * pRadius), sin(pAngle * pRadius));

        Size pSize = Size(Fw::randomRange(m_pMinSize.width(), m_pMaxSize.width()), Fw::randomRange(m_pMinSize.height(), m_pMaxSize.height()));
        float pDuration = Fw::randomRange(m_pMinDuration, m_pMaxDuration);

        // todo: add random data generation
        m_particles.push_back(ParticlePtr(new Particle(Rect(m_position + pPosition, pSize), 16, 8, 0, 0, pDuration, Color(255, 0, 0, 32), tex)));
    }
    m_createdParticles = currentParticles;
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
