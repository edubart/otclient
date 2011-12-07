#include "particlessystem.h"
#include "graphics.h"
#include <framework/core/clock.h>

Particle::Particle(const Rect& rect, float vx, float vy, float ax, float ay, const Color& color, TexturePtr texture)
{
    m_rect = rect;
    m_vx = vx; m_vy = vy;
    m_ax = ax; m_ay = ay;
    m_color = color;
    m_texture = texture;
    m_finished = false;
}

void Particle::render()
{
    g_graphics.bindColor(m_color);

    if(!m_texture)
        g_graphics.drawFilledRect(m_rect);
    else
        g_graphics.drawTexturedRect(m_rect, m_texture);
}

Emitter::Emitter(const Point& position, int duration, int particlesPerSecond)
{
    m_position = position;
    m_duration = duration;
    m_particlesPerSecond = particlesPerSecond;
    m_createdParticles = 0;
    m_startTicks = g_clock.ticks();
    m_finished = false;
}

void Emitter::render()
{
    for(auto it = m_particles.begin(), end = m_particles.end(); it != end; ++it)
        (*it).render();
}

void Emitter::update()
{
    ticks_t elapsedTicks = g_clock.ticks() - m_startTicks;

    // check if finished
    if(m_duration > 0 && elapsedTicks > m_duration * 1000)
        m_finished = true;

    // create some particles
    int currentParticles = elapsedTicks / 1000.0 * m_particlesPerSecond;
    for(int i = m_createdParticles; i < currentParticles; ++i) {
        Particle particle = Particle(Rect(100, 100, 16, 16), 0, 0, 0, 0);
        m_particles.push_back(particle);
    }
}

void ParticlesSystem::add(Emitter emitter)
{
    m_emitters.push_back(emitter);
}

void ParticlesSystem::render()
{
    for(auto it = m_emitters.begin(), end = m_emitters.end(); it != end; ++it)
        (*it).render();
}

void ParticlesSystem::update()
{
    for(auto it = m_emitters.begin(), end = m_emitters.end(); it != end;) {
        Emitter emitter = *it;
        emitter.update();
        ++it;
    }
}
