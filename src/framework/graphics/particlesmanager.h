#include "particlessystem.h"

class ParticlesManager {
public:
    void add(ParticlesSystem particleSystem);

    void render();
    void update();

private:
    std::list<ParticlesSystem> m_particlesSystems;
};

extern ParticlesManager g_particlesManager;
