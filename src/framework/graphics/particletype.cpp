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

#include "particletype.h"
#include <framework/graphics/texturemanager.h>

ParticleType::ParticleType()
{
    // particles default configuration. (make them reasonable for user detect missing properties on scripts)
    pMinPositionRadius = 0;
    pMaxPositionRadius = 3;
    pMinPositionAngle = 0;
    pMaxPositionAngle = 360;
    pStartSize = Size(32, 32);
    pFinalSize = Size(32, 32);
    pMinDuration = 0;
    pMaxDuration = 10;
    pIgnorePhysicsAfter = -1;
    pMinVelocity = 32;
    pMaxVelocity = 64;
    pMinVelocityAngle = 0;
    pMaxVelocityAngle = 360;
    pMinAcceleration = 32;
    pMaxAcceleration = 64;
    pMinAccelerationAngle = 0;
    pMaxAccelerationAngle = 360;
}

void ParticleType::load(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "name")
            pName = childNode->value();
        else if(childNode->tag() == "position-radius") {
            pMinPositionRadius = childNode->value<float>();
            pMaxPositionRadius = childNode->value<float>();
        }
        else if(childNode->tag() == "min-position-radius")
            pMinPositionRadius = childNode->value<float>();
        else if(childNode->tag() == "max-position-radius")
            pMaxPositionRadius = childNode->value<float>();
        else if(childNode->tag() == "position-angle") {
            pMinPositionAngle = childNode->value<float>() * DEG_TO_RAD;
            pMaxPositionAngle = childNode->value<float>() * DEG_TO_RAD;
        }
        else if(childNode->tag() == "min-position-angle")
            pMinPositionAngle = childNode->value<float>() * DEG_TO_RAD;
        else if(childNode->tag() == "max-position-angle")
            pMaxPositionAngle = childNode->value<float>() * DEG_TO_RAD;

        // velocity
        else if(childNode->tag() == "velocity") {
            pMinVelocity = childNode->value<float>();
            pMaxVelocity = childNode->value<float>();
        }
        else if(childNode->tag() == "min-velocity")
            pMinVelocity = childNode->value<float>();
        else if(childNode->tag() == "max-velocity")
            pMaxVelocity = childNode->value<float>();
        else if(childNode->tag() == "velocity-angle") {
            pMinVelocityAngle = childNode->value<float>() * DEG_TO_RAD;
            pMaxVelocityAngle = childNode->value<float>() * DEG_TO_RAD;
        }
        else if(childNode->tag() == "min-velocity-angle")
            pMinVelocityAngle = childNode->value<float>() * DEG_TO_RAD;
        else if(childNode->tag() == "max-velocity-angle")
            pMaxVelocityAngle = childNode->value<float>() * DEG_TO_RAD;
        else if(childNode->tag() == "acceleration") {
            pMinAcceleration = childNode->value<float>();
            pMaxAcceleration = childNode->value<float>();
        }

        // acceleration
        else if(childNode->tag() == "min-acceleration")
            pMinAcceleration = childNode->value<float>();
        else if(childNode->tag() == "max-acceleration")
            pMaxAcceleration = childNode->value<float>();
        else if(childNode->tag() == "acceleration-angle") {
            pMinAccelerationAngle = childNode->value<float>() * DEG_TO_RAD;
            pMaxAccelerationAngle = childNode->value<float>() * DEG_TO_RAD;
        }
        else if(childNode->tag() == "min-acceleration-angle")
            pMinAccelerationAngle = childNode->value<float>() * DEG_TO_RAD;
        else if(childNode->tag() == "max-acceleration-angle")
            pMaxAccelerationAngle = childNode->value<float>() * DEG_TO_RAD;

        // duration
        else if(childNode->tag() == "duration") {
            pMinDuration = childNode->value<float>();
            pMaxDuration = childNode->value<float>();
        }
        else if(childNode->tag() == "min-duration")
            pMinDuration = childNode->value<float>();
        else if(childNode->tag() == "max-duration")
            pMaxDuration = childNode->value<float>();
        else if(childNode->tag() == "ignore-physics-after")
            pIgnorePhysicsAfter = childNode->value<float>();

        // visual
        else if(childNode->tag() == "size") {
            pStartSize = childNode->value<Size>();
            pFinalSize = childNode->value<Size>();
        }
        else if(childNode->tag() == "start-size")
            pStartSize = childNode->value<Size>();
        else if(childNode->tag() == "final-size")
            pFinalSize = childNode->value<Size>();

        else if(childNode->tag() == "colors")
            pColors = stdext::split<Color>(childNode->value());
        else if(childNode->tag() == "colors-stops")
            pColorsStops = stdext::split<float>(childNode->value());
        else if(childNode->tag() == "texture")
            pTexture = g_textures.getTexture(childNode->value());
        else if(childNode->tag() == "composition-mode") {
            if(childNode->value() == "normal")
                pCompositionMode = Painter::CompositionMode_Normal;
            else if(childNode->value() == "multiply")
                pCompositionMode = Painter::CompositionMode_Multiply;
            else if(childNode->value() == "addition")
                pCompositionMode = Painter::CompositionMode_Add;
        }
    }

    if(pColors.empty())
        pColors.emplace_back(255, 255, 255, 128);
    if(pColorsStops.empty())
        pColorsStops.push_back(0);

    if(pColors.size() != pColorsStops.size())
        stdext::throw_exception("particle colors must be equal to colorstops-1");

    pTexture->setSmooth(true);
    pTexture->buildHardwareMipmaps();
}
