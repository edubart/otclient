#ifndef EFFECT_H
#define EFFECT_H

#include <global.h>
#include "thing.h"

class Effect;
typedef std::shared_ptr<Effect> EffectPtr;

class Effect : public Thing
{
public:
    Effect();

    virtual ThingAttributes *getAttributes();
    void draw(int x, int y);

private:
};

#endif // EFFECT_H
