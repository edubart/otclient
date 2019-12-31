/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef EFFECT_H
#define EFFECT_H

#include <framework/global.h>
#include <framework/core/timer.h>
#include "thing.h"

// @bindclass
class Effect : public Thing
{
    enum {
        EFFECT_TICKS_PER_FRAME = 75
    };

public:
    void drawEffect(const Point& dest, float scaleFactor, bool animate, int offsetX = 0, int offsetY = 0, LightView *lightView = nullptr);

    void setId(uint32 id);
    uint32 getId() { return m_id; }

    EffectPtr asEffect() { return static_self_cast<Effect>(); }
    bool isEffect() { return true; }

    const ThingTypePtr& getThingType();
    ThingType *rawGetThingType();

protected:
    void onAppear();

private:
    Timer m_animationTimer;
    uint16 m_id;
};

#endif
