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

#ifndef STDEXT_PACKEDSTORAGE_H
#define STDEXT_PACKEDSTORAGE_H

#include "types.h"
#include "packed_any.h"

namespace stdext {

// disable memory alignment
#pragma pack(push,1)

// this class was designed to use less memory as possible
template<typename Key, typename SizeType = uint8>
class packed_storage {
    struct value_pair {
        Key id;
        packed_any value;
    };

public:
    packed_storage() : m_values(nullptr), m_size(0) { }
    ~packed_storage() { delete[] m_values; }

    template<typename T>
    void set(Key id, const T& value) {
        for(SizeType i=0;i<m_size;++i) {
            if(m_values[i].id == id) {
                m_values[i].value = value;
                return;
            }
        }
        auto tmp = new value_pair[m_size+1];
        if(m_size > 0) {
            std::copy(m_values, m_values + m_size, tmp);
            delete[] m_values;
        }
        m_values = tmp;
        m_values[m_size++] = { id, packed_any(value) };
    }

    bool remove(Key id) {
        auto begin = m_values;
        auto end = m_values + m_size;
        auto it = std::find_if(begin, end, [=](const value_pair& pair) -> bool { return pair.id == id; } );
        if(it == end)
            return false;
        int pos = it - begin;
        auto tmp = new value_pair[m_size-1];
        std::copy(begin, begin + pos, tmp);
        std::copy(begin + pos + 1, end, tmp + pos);
        delete[] m_values;
        m_values = tmp;
        m_size--;
        return true;
    }

    template<typename T>
    T get(Key id) const {
        for(SizeType i=0;i<m_size;++i)
            if(m_values[i].id == id)
                return packed_any_cast<T>(m_values[i].value);
        return T();
    }

    bool has(Key id) const {
        for(SizeType i=0;i<m_size;++i)
            if(m_values[i].id == id)
                return true;
        return false;
    }

    void clear() {
        if(m_values)
            delete [] m_values;
        m_values = nullptr;
        m_size = 0;
    }

    std::size_t size() { return m_size; }

private:
    value_pair *m_values;
    SizeType m_size;
};

// restore memory alignment
#pragma pack(pop)

}

#endif
