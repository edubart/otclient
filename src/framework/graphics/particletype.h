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

#include "declarations.h"
#include <framework/graphics/painter.h>
#include <framework/luaengine/luaobject.h>
#include <framework/otml/otml.h>

class ParticleType : public LuaObject
{
public:
    ParticleType();

    void load(const OTMLNodePtr& node);

    std::string getName() { return pName; }

protected:

    // name
    std::string pName;

    // visual ralated
    std::vector<Color> pColors;
    std::vector<float> pColorsStops;
    TexturePtr pTexture;
    ParticleTypePtr particleType;
    Painter::CompositionMode pCompositionMode;

    // particles default configuration. (make them reasonable for user detect missing properties on scripts)
    // size
    Size pStartSize{ 32 }, pFinalSize{ 32 };

    // initial position related to emitter position
    float pMinPositionRadius{ 0 }, pMaxPositionRadius{ 3 };
    float pMinPositionAngle{ 0 }, pMaxPositionAngle{ 360 };

    // initial velocity
    float pMinVelocity{ 32 }, pMaxVelocity{ 64 };
    float pMinVelocityAngle{ 0 }, pMaxVelocityAngle{ 360 };

    // initial acceleration
    float pMinAcceleration{ 32 }, pMaxAcceleration{ 64 };
    float pMinAccelerationAngle{ 0 }, pMaxAccelerationAngle{ 360 };

    // duration
    float pMinDuration{ 0 }, pMaxDuration{ 10 }, pIgnorePhysicsAfter{ -1 };

    friend class ParticleEmitter;
};
