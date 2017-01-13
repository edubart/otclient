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

#ifndef STDEXT_PACKEDVECTOR_H
#define STDEXT_PACKEDVECTOR_H

#include <algorithm>

namespace stdext {

// disable memory alignment
#pragma pack(push,1)

template<class T, class U = uint8_t>
class packed_vector
{
public:
    typedef U                                        size_type;
    typedef T*                                       iterator;
    typedef const T*                                 const_iterator;
    typedef std::reverse_iterator<iterator>          reverse_iterator;
    typedef std::reverse_iterator<const_iterator>    const_reverse_iterator;

    packed_vector() : m_size(0), m_data(nullptr) { }
    packed_vector(size_type size) : m_size(size), m_data(new T[size]) { }
    packed_vector(size_type size, const T& value) : m_size(size), m_data(new T[size]) { std::fill(begin(), end(), value); }
    template <class InputIterator>
    packed_vector(InputIterator first, InputIterator last) : m_size(last - first), m_data(new T[m_size]) { std::copy(first, last, m_data); }
    packed_vector(const packed_vector<T>& other) : m_size(other.m_size), m_data(new T[other.m_size]) { std::copy(other.begin(), other.end(), m_data); }
    ~packed_vector() { if(m_data) delete[] m_data; }

    packed_vector<T,U>& operator=(packed_vector<T,U> other) { other.swap(*this); return *this; }

    iterator begin() { return m_data; }
    const_iterator begin() const { return m_data; }
    const_iterator cbegin() const { return m_data; }
    iterator end() { return m_data + m_size; }
    const_iterator end() const { return m_data + m_size; }
    const_iterator cend() const { return m_data + m_size; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

    size_type size() const { return m_size; }
    bool empty() const { return m_size == 0; }

    T& operator[](size_type i) { return m_data[i]; }
    const T& operator[](size_type i) const { return m_data[i]; }
    T& at(size_type i) { return m_data[i]; }
    const T& at(size_type i) const { return m_data[i]; }

    T& front() { return m_data[0]; }
    const T& front() const { return m_data[0]; }
    T& back() { return m_data[m_size-1]; }
    const T& back() const { return m_data[m_size-1]; }

    T *data() { return m_data; }
    const T *data() const { return m_data; }

    void clear() {
        if(m_data) {
            delete[] m_data;
            m_data = nullptr;
        }
        m_size = 0;
    }

    void resize(size_type size) {
        clear();
        if(size > 0) {
            m_data = new T[size];
            m_size = size;
        }
    }

    void push_back(const T& x) {
        T *tmp = new T[m_size+1];
        std::copy(m_data, m_data + m_size, tmp);
        tmp[m_size] = x;
        delete[] m_data;
        m_data = tmp;
        m_size++;
    }

    void pop_back() {
        if(m_size == 1) {
            clear();
            return;
        }
        T *tmp = new T[m_size-1];
        std::copy(m_data, m_data + m_size - 1, tmp);
        delete[] m_data;
        m_data = tmp;
        m_size--;
    }

    iterator insert(const_iterator position, const T& x) {
        T *tmp = new T[m_size+1];
        size_type i = position - m_data;
        std::copy(m_data, m_data + i, tmp);
        tmp[i] = x;
        std::copy(m_data + i, m_data + m_size, tmp + i + 1);
        delete[] m_data;
        m_data = tmp;
        m_size++;
        return tmp + i;
    }

    iterator erase(const_iterator position) {
        T *tmp = new T[m_size-1];
        size_type i = position - m_data;
        std::copy(m_data, m_data + i, tmp);
        std::copy(m_data + i + 1, m_data + m_size, tmp + i);
        delete[] m_data;
        m_data = tmp;
        m_size--;
        return tmp + i;
    }

    void swap(packed_vector<T,U>& other) { std::swap(m_size, other.m_size); std::swap(m_data, other.m_data); }

    operator std::vector<T>() const { return std::vector<T>(begin(), end()); }

private:
    size_type m_size;
    T* m_data;
};

// restore memory alignment
#pragma pack(pop)

}

namespace std {
template<class T, class U> void swap(stdext::packed_vector<T,U>& lhs, stdext::packed_vector<T,U>& rhs) { lhs.swap(rhs); }
}

#endif
