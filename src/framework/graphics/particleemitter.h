/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include "declarations.h"
#include "painter.h"
#include <framework/global.h>
#include <framework/graphics/texture.h>
#include <framework/otml/otml.h>

class ParticleEmitter {
public:

    ParticleEmitter(const ParticleSystemPtr& parent);

    bool load(const OTMLNodePtr& node);

    void update(double elapsedTime);

    bool hasFinished() { return m_finished; }

private:
    ParticleSystemWeakPtr m_parent;

    // self related
    Point m_pos;
    float m_duration, m_delay;
    double m_elapsedTime;
    bool m_finished, m_active;
    float m_burstRate;
    int m_currentBurst, m_burstCount;

    // particles size
    Size m_pStartSize, m_pFinalSize;

    // particles initial position related to emitter position
    float m_pMinPositionRadius, m_pMaxPositionRadius;
    float m_pMinPositionAngle, m_pMaxPositionAngle;

    // particles initial velocity
    float m_pMinVelocity, m_pMaxVelocity;
    float m_pMinVelocityAngle, m_pMaxVelocityAngle;

    // particles initial acceleration
    float m_pMinAcceleration, m_pMaxAcceleration;
    float m_pMinAccelerationAngle, m_pMaxAccelerationAngle;

    // particles duration
    float m_pMinDuration, m_pMaxDuration, m_pIgnorePhysicsAfter;

    // visual ralated
    std::vector<Color> m_pColors;
    std::vector<float> m_pColorsStops;
    TexturePtr m_pTexture;
    Painter::CompositionMode m_pCompositionMode;
};

#endif
