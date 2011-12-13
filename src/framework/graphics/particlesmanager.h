#include "particlessystem.h"

class ParticlesManager {
public:
    bool load(const std::string& filename);

    void add(ParticlesSystem particleSystem);

    void render();
    void update();

private:
    std::list<ParticlesSystem> m_particlesSystems;
};

extern ParticlesManager g_particlesManager;
