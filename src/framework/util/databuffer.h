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

#ifndef DATABUFFER_H
#define DATABUFFER_H

template<class T>
class DataBuffer
{
public:
    DataBuffer(unsigned int res = 64) : m_capacity(res), m_buffer(new T[m_capacity]) {}
    ~DataBuffer()
    {
        delete[] m_buffer;
    }

    void reset() { m_size = 0; }

    void clear()
    {
        m_size = 0;
        m_capacity = 0;
        delete[] m_buffer;
        m_buffer = nullptr;
    }

    bool empty() const { return m_size == 0; }
    unsigned int size() const { return m_size; }
    T* data() const { return m_buffer; }

    const T& at(unsigned int i) const { return m_buffer[i]; }
    const T& last() const { return m_buffer[m_size - 1]; }
    const T& first() const { return m_buffer[0]; }
    const T& operator[](unsigned int i) const { return m_buffer[i]; }
    T& operator[](unsigned int i) { return m_buffer[i]; }

    void reserve(unsigned int n)
    {
        if (n > m_capacity) {
            T* buffer = new T[n];
            for (unsigned int i = 0; i < m_size; ++i)
                buffer[i] = m_buffer[i];

            delete[] m_buffer;
            m_buffer = buffer;
            m_capacity = n;
        }
    }

    void resize(unsigned int n, T def = T())
    {
        if (n == m_size)
            return;
        reserve(n);
        for (unsigned int i = m_size; i < n; ++i)
            m_buffer[i] = def;
        m_size = n;
    }

    void grow(unsigned int n)
    {
        if (n <= m_size)
            return;
        if (n > m_capacity) {
            unsigned int newcapacity = m_capacity;
            do { newcapacity *= 2; } while (newcapacity < n);
            reserve(newcapacity);
        }
        m_size = n;
    }

    void add(const T& v)
    {
        grow(m_size + 1);
        m_buffer[m_size - 1] = v;
    }

    DataBuffer& operator<<(const T& t) { add(t); return *this; }

private:
    unsigned int m_size{ 0 };
    unsigned int m_capacity;
    T* m_buffer;
};

#endif
