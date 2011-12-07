#include <framework/global.h>
#include <framework/graphics/texture.h>

struct Particle {
public:

    Particle(const Rect& rect, float vx, float vy, float ax, float ay, const Color& color = Color(255, 255, 255), TexturePtr texture = nullptr);
    void render();

private:
    Rect m_rect;
    float m_vx, m_vy;
    float m_ax, m_ay;

    Color m_color;
    TexturePtr m_texture;
    bool m_finished;
};

class Emitter {
public:

    Emitter(const Point& position, int duration, int particlesPerSecond);

    void render();
    void update();

private:
    // self related
    Point m_position;
    int m_duration;
    ticks_t m_startTicks;
    bool m_finished;
    int m_particlesPerSecond, m_createdParticles;
    std::list<Particle> m_particles;

    // particles initial position related to emitter position
    float positionMinRadius, positionMaxRadius;
    float positionMinAngle, positionMaxAngle;

    // particles initial velocity
    float minVelocity, maxVelocity;
    float minVelocityAngle, maxVelocityAngle;

    // particles initial acceleration
    float minAcceleration, maxAcceleration;
    float minAccelerationAngle, maxAccelerationAngle;

    // color ralated
    Color color;


    // texture related
};

class Affector {
public:
    virtual void update() {};
};

class Gravity270Affector : public Affector {
public:
    void update() {

    }
};

class ParticlesSystem {
public:
    void add(Emitter emitter);

    void render();
    void update();

private:
    std::list<Emitter> m_emitters;
    std::list<Affector> m_affectors;
};
