#ifndef EFFECT_H
#define EFFECT_H

#include <framework/global.h>
#include "thing.h"


class Effect : public Thing
{
public:
    Effect();

    virtual ThingAttributes *getAttributes();
    void draw(int x, int y);

private:
};

#endif // EFFECT_H
