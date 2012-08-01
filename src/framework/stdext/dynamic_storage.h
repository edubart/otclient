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

#ifndef STDEXT_DYNAMICSTORAGE_H
#define STDEXT_DYNAMICSTORAGE_H

#include "types.h"
#include "any.h"
#include <unordered_map>

namespace stdext {

template<typename Key>
class dynamic_storage {
public:
    template<typename T> void set(const Key& k, const T& value) { m_map[k] = value; }
    template<typename T> T get(const Key& k) const {
        auto it = m_map.find(k);
        if(it != m_map.end())
            return any_cast<T>(it->second);
        return T();
    }
    bool has(const Key& k) const { return m_map.find(k) != m_map.end(); }

    std::size_t size() const { return m_map.size(); }
    void clear() { m_map.clear(); }

private:
    std::unordered_map<Key, any> m_map;
};

}

#endif
