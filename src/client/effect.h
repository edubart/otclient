/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#pragma once

#include "thing.h"
#include <framework/core/timer.h>
#include <framework/global.h>

 // @bindclass
class Effect : public Thing
{
public:
    void drawEffect(const Point& dest, float scaleFactor, LightView* lightView = nullptr);

    void setId(uint32 id) override;
    uint32 getId() override { return m_id; }

    EffectPtr asEffect() { return static_self_cast<Effect>(); }
    bool isEffect() override { return true; }

    const ThingTypePtr& getThingType() override;
    ThingType* rawGetThingType() override;

    void waitFor(const EffectPtr&);

protected:
    void onAppear() override;

private:
    Timer m_animationTimer;

    uint16 m_id;

    int m_duration;
    int m_timeToStartDrawing{ 0 };
};
