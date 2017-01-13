/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#ifndef PARTICLETYPE_H
#define PARTICLETYPE_H

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

    // size
    Size pStartSize, pFinalSize;

    // initial position related to emitter position
    float pMinPositionRadius, pMaxPositionRadius;
    float pMinPositionAngle, pMaxPositionAngle;

    // initial velocity
    float pMinVelocity, pMaxVelocity;
    float pMinVelocityAngle, pMaxVelocityAngle;

    // initial acceleration
    float pMinAcceleration, pMaxAcceleration;
    float pMinAccelerationAngle, pMaxAccelerationAngle;

    // duration
    float pMinDuration, pMaxDuration, pIgnorePhysicsAfter;

    // visual ralated
    std::vector<Color> pColors;
    std::vector<float> pColorsStops;
    TexturePtr pTexture;
    ParticleTypePtr particleType;
    Painter::CompositionMode pCompositionMode;

    friend class ParticleEmitter;
};

#endif
