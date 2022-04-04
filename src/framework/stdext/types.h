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

#ifndef STDEXT_TYPES_H
#define STDEXT_TYPES_H

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <any>

using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;

using uint64 = uint64_t;
using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8 = uint8_t;
using int64 = int64_t;
using int32 = int32_t;
using int16 = int16_t;
using int8 = int8_t;

using ticks_t = int64;
using refcount_t = uint_fast32_t;

using std::size_t;
using std::ptrdiff_t;

template<typename Key>
class dynamic_storage {
    public:
        template<typename T> void set(const Key& key, const T& value) {
            m_data[key] = value;
        }

        bool remove(const Key& k) {
            return m_data.erase(k) > 0;
        }

        template<typename T> T get(const Key& k) const {
            auto it = m_data.find(k);
            if (it == m_data.end()) {
                return T();
            }

            try {
                return std::any_cast<T>(it->second);
            } catch (std::exception&) {
                return T();
            }
        }

        bool has(const Key& k) const {
            auto it = m_data.find(k);
            return it != m_data.end();
        }

        size_t size() const {
            return m_data.count();
        }

        void clear() { m_data.clear(); }

    private:
        std::unordered_map<Key, std::any> m_data;
};

#endif
