#ifndef EFFECT_H
#define EFFECT_H

#include <framework/global.h>
#include "thing.h"

class Effect : public Thing
{
public:
    Effect();

    void draw(int x, int y);

    bool finished() { return m_finished; }

    const ThingAttributes& getAttributes();

    EffectPtr asEffect() { return std::static_pointer_cast<Effect>(shared_from_this()); }

private:
    int m_lastTicks;
    int m_animation;
    bool m_finished;
};

#endif
