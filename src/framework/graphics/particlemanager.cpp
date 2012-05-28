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

#include "particlemanager.h"
#include <framework/core/resourcemanager.h>
#include <framework/otml/otml.h>

ParticleManager g_particleManager;

bool ParticleManager::load(const std::string& filename)
{
    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(filename);
        for(const OTMLNodePtr& node : doc->children()) {
            if(node->tag() == "ParticleSystem") {
                ParticleSystemPtr particleSystem = ParticleSystemPtr(new ParticleSystem);
                particleSystem->load(node);
                m_systems.push_back(particleSystem);
            }
        }
        return true;
    } catch(stdext::exception& e) {
        logError("could not load particles: %s", e.what());
        return false;
    }
}

void ParticleManager::render()
{
    for(auto it = m_systems.begin(), end = m_systems.end(); it != end; ++it)
        (*it)->render();
}

void ParticleManager::update()
{
    for(auto it = m_systems.begin(), end = m_systems.end(); it != end;) {
        const ParticleSystemPtr& particleSystem = *it;

        if(particleSystem->hasFinished()) {
            it = m_systems.erase(it);
            continue;
        }

        particleSystem->update();
        ++it;
    }
}
