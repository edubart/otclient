/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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
    DataBuffer(int res = 64) {
        m_capacity = res;
        m_buffer = new T[m_capacity];
        m_size = 0;
    }
    ~DataBuffer() { delete[] m_buffer; }

    inline void reset() { m_size = 0; }
    inline bool isEmpty() const { return m_size == 0; }

    inline int size() const { return m_size; }
    inline T *data() const { return m_buffer; }

    inline const T& at(int i) const { return m_buffer[i]; }
    inline const T& last() const { return m_buffer[m_size-1]; }
    inline const T& first() const { return m_buffer[0]; }
    inline const T& operator[](int i) const { return m_buffer[i]; }
    inline T& operator[](int i) { return m_buffer[i]; }

    inline void add(const T &t) {
        if(m_size >= m_capacity) {
            m_capacity *= 2;
            T *buffer = new T[m_capacity];
            for(int i=0;i<m_size;++i)
                buffer[i] = m_buffer[i];
            delete[] m_buffer;
            m_buffer = buffer;
        }
        m_buffer[m_size++] = t;
    }

    inline DataBuffer &operator<<(const T &t) { add(t); return *this; }

private:
    int m_size;
    int m_capacity;
    T *m_buffer;
};

#endif
