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

#include "particleeffect.h"

ParticleEffectType::ParticleEffectType()
{

}

void ParticleEffectType::load(const OTMLNodePtr& node)
{
    m_node = node->clone();
    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "name")
            m_name = childNode->value();
        else if(childNode->tag() == "description")
            m_description = childNode->value();
    }
}

void ParticleEffect::load(const ParticleEffectTypePtr& effectType)
{
    if(!effectType)
        stdext::throw_exception("effect type not found");

    for(const OTMLNodePtr& childNode : effectType->getNode()->children()) {
        if(childNode->tag() == "System") {
            ParticleSystemPtr system = ParticleSystemPtr(new ParticleSystem);
            system->load(childNode);
            m_systems.push_back(system);
        }
    }
}

void ParticleEffect::render()
{
    for(auto &system: m_systems)
        system->render();
}

void ParticleEffect::update()
{
    for(auto it = m_systems.begin(); it != m_systems.end();) {
        const ParticleSystemPtr& system = *it;

        if(system->hasFinished()) {
            it = m_systems.erase(it);
        } else {
            system->update();
            ++it;
        }
    }
}
