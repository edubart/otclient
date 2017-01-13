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

#ifndef DATABUFFER_H
#define DATABUFFER_H

template<class T>
class DataBuffer
{
public:
    DataBuffer(uint res = 64) :
        m_size(0),
        m_capacity(res),
        m_buffer(new T[m_capacity]) { }
    ~DataBuffer() {
        if(m_buffer)
            delete[] m_buffer;
    }

    inline void reset() { m_size = 0; }
    inline void clear() {
        m_size = 0;
        m_capacity = 0;
        delete[] m_buffer;
        m_buffer = nullptr;
    }

    inline bool empty() const { return m_size == 0; }
    inline uint size() const { return m_size; }
    inline T *data() const { return m_buffer; }

    inline const T& at(uint i) const { return m_buffer[i]; }
    inline const T& last() const { return m_buffer[m_size-1]; }
    inline const T& first() const { return m_buffer[0]; }
    inline const T& operator[](uint i) const { return m_buffer[i]; }
    inline T& operator[](uint i) { return m_buffer[i]; }

    inline void reserve(uint n) {
        if(n > m_capacity) {
            T *buffer = new T[n];
            for(uint i=0;i<m_size;++i)
                buffer[i] = m_buffer[i];
            if(m_buffer)
                delete[] m_buffer;
            m_buffer = buffer;
            m_capacity = n;
        }
    }

    inline void resize(uint n, T def = T()) {
        if(n == m_size)
            return;
        reserve(n);
        for(uint i=m_size;i<n;++i)
            m_buffer[i] = def;
        m_size = n;
    }

    inline void grow(uint n) {
        if(n <= m_size)
            return;
        if(n > m_capacity) {
            uint newcapacity = m_capacity;
            do { newcapacity *= 2; } while(newcapacity < n);
            reserve(newcapacity);
        }
        m_size = n;
    }

    inline void add(const T& v) {
        grow(m_size + 1);
        m_buffer[m_size-1] = v;
    }

    inline DataBuffer &operator<<(const T &t) { add(t); return *this; }

private:
    uint m_size;
    uint m_capacity;
    T *m_buffer;
};

#endif
