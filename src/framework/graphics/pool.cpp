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

#include "pool.h"

void Pool::join()
{
    if(m_thread.joinable())
        m_thread.join();
}

void Pool::init(const bool openThread)
{
    if(openThread)
        m_thread = std::thread(m_action);
    else  m_action();
}

size_t PoolFramed::updateHash(const TexturePtr& texture, const DrawMethod& method)
{
    const auto& currentState = g_painter->getCurrentState();

    size_t hash = 0;

    if(texture)
        boost::hash_combine(hash, HASH_INT(texture->getUniqueId()));

    if(currentState.opacity < 1.f)
        boost::hash_combine(hash, HASH_INT(currentState.opacity));

    if(currentState.color != Color::white)
        boost::hash_combine(hash, HASH_INT(currentState.color.rgba()));

    if(currentState.compositionMode != Painter::CompositionMode_Normal)
        boost::hash_combine(hash, HASH_INT(currentState.compositionMode));

    if(currentState.shaderProgram)
        boost::hash_combine(hash, HASH_INT(currentState.shaderProgram->getProgramId()));

    if(currentState.clipRect.isValid()) {
        boost::hash_combine(hash, HASH_INT(currentState.clipRect.x()));
        boost::hash_combine(hash, HASH_INT(currentState.clipRect.y()));
    }

    if(method.rects.first.isValid()) {
        boost::hash_combine(hash, HASH_INT(method.rects.first.x()));
        boost::hash_combine(hash, HASH_INT(method.rects.first.y()));
    }
    if(method.rects.second.isValid()) {
        boost::hash_combine(hash, HASH_INT(method.rects.second.x()));
        boost::hash_combine(hash, HASH_INT(method.rects.second.y()));
    }

    const auto& a = std::get<0>(method.points),
        b = std::get<1>(method.points),
        c = std::get<2>(method.points);

    if(!a.isNull()) {
        boost::hash_combine(hash, HASH_INT(a.x));
        boost::hash_combine(hash, HASH_INT(a.y));
    }
    if(!b.isNull()) {
        boost::hash_combine(hash, HASH_INT(b.x));
        boost::hash_combine(hash, HASH_INT(b.y));
    }
    if(!c.isNull()) {
        boost::hash_combine(hash, HASH_INT(c.x));
        boost::hash_combine(hash, HASH_INT(c.y));
    }

    if(method.intValue != 0)
        boost::hash_combine(hash, HASH_INT(method.intValue));

    if(method.floatValue != 0)
        boost::hash_combine(hash, HASH_FLOAT(method.floatValue));

    boost::hash_combine(m_status.second, hash);

    return hash;
}
