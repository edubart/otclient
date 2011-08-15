#ifndef EFFECT_H
#define EFFECT_H

#include <framework/global.h>
#include "thing.h"

class Effect : public Thing
{
public:
    Effect();

    void draw(int x, int y);

    const ThingAttributes& getAttributes();

    virtual EffectPtr asEffect() { return std::static_pointer_cast<Effect>(shared_from_this()); }
};

#endif
