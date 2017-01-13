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

#ifndef STDEXT_DYNAMICSTORAGE_H
#define STDEXT_DYNAMICSTORAGE_H

#include "types.h"
#include "any.h"
#include <unordered_map>

namespace stdext {

template<typename Key>
class dynamic_storage {
public:
    template<typename T> void set(const Key& k, const T& value) {
        if(m_data.size() <= k)
            m_data.resize(k+1);
        m_data[k] = value;
    }
    bool remove(const Key& k) {
        if(m_data.size() < k)
            return false;
        if(m_data[k].empty())
            return false;
        m_data[k] = any();
        return true;
    }
    template<typename T> T get(const Key& k) const { return has(k) ? any_cast<T>(m_data[k]) : T(); }
    bool has(const Key& k) const { return k < m_data.size() && !m_data[k].empty(); }

    std::size_t size() const {
        std::size_t count = 0;
        for(std::size_t i=0;i<m_data.size();++i)
            if(!m_data[i].empty())
                count++;
        return count;
    }

    void clear() { m_data.clear(); }

private:
    std::vector<any> m_data;
};

}

#endif
