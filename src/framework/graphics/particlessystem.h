#include <framework/global.h>
#include <framework/graphics/texture.h>

class Particle {
public:

    Particle(const Rect& rect, float vx, float vy, float ax, float ay, float duration, const Color& color = Color(255, 255, 255), TexturePtr texture = nullptr);
    ~Particle();

    void render();
    void update();

    bool hasFinished() { return m_finished; }

private:
    Rect m_rect;
    Color m_color;
    TexturePtr m_texture;

    int m_ix, m_iy;
    PointF m_s0, m_v, m_a;
    float m_vx, m_vy;
    float m_ax, m_ay;

    float m_duration;
    ticks_t m_startTicks;
    bool m_finished;
};
typedef std::shared_ptr<Particle> ParticlePtr;

class ParticleEmitter {
public:

    ParticleEmitter(const Point& position, float duration, float burstRate, float burstCount);

    void render();
    void update();

    bool hasFinished() { return m_finished; }

private:
    // self related
    Point m_position;
    int m_duration;
    ticks_t m_startTicks;
    bool m_finished;
    float m_burstRate, m_burstCount;
    int m_currentBurst;
    std::list<ParticlePtr> m_particles;

    // particles size
    Size m_pMinSize, m_pMaxSize;

    // particles initial position related to emitter position
    float m_pPositionMinRadius, m_pPositionMaxRadius;
    float m_pPositionMinAngle, m_pPositionMaxAngle;

    // particles initial velocity
    float m_pMinVelocity, m_pMaxVelocity;
    float m_pMinVelocityAngle, m_pMaxVelocityAngle;

    // particles initial acceleration
    float m_pMinAcceleration, m_pMaxAcceleration;
    float m_pMinAccelerationAngle, m_pMaxAccelerationAngle;

    // particles duration
    float m_pMinDuration, m_pMaxDuration;

    // visual ralated
    Color m_pColor;
    TexturePtr m_pTexture;
};
typedef std::shared_ptr<ParticleEmitter> ParticleEmitterPtr;

class Affector {
public:
    virtual void update() {}
};

class Gravity270Affector : public Affector {
public:
    void update() {
        // earth gravity is 9.8 m/s². -> in tibia, 32 pixels are equal to 1 meter -> 32 pixels/m -> 9.8 * 32 is gravity constant
    }
};

class ParticlesSystem {
public:
    void add(const ParticleEmitterPtr& emitter);

    void render();
    void update();

private:
    std::list<ParticleEmitterPtr> m_emitters;
    std::list<Affector> m_affectors;
};
