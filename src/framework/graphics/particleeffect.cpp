/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

bool ParticleEffectType::load(const OTMLNodePtr& node)
{
    m_node = node;
    for(const OTMLNodePtr& childNode : node->children()) {
        if(childNode->tag() == "name") {
            setName(childNode->value());
        }
        else if(childNode->tag() == "description") {
            setDescription(childNode->value());
        }
    }
    return !m_name.empty();
}

bool ParticleEffect::load(const ParticleEffectTypePtr& effectType)
{
    if(!effectType)
        return false;

    for(const OTMLNodePtr& childNode : effectType->getNode()->children()) {
        if(childNode->tag() == "System") {
            ParticleSystemPtr system = ParticleSystemPtr(new ParticleSystem);
            if(system->load(childNode)) {
                m_systems.push_back(system);
            }
        }
    }
    return true;
}

void ParticleEffect::render()
{
    for(auto it = m_systems.begin(), end = m_systems.end(); it != end; ++it)
        (*it)->render();
}

void ParticleEffect::update()
{
    for(auto it = m_systems.begin(), end = m_systems.end(); it != end;) {
        const ParticleSystemPtr& system = *it;

        if(system->hasFinished()) {
            it = m_systems.erase(it);
            continue;
        }

        system->update();
        ++it;
    }
}
