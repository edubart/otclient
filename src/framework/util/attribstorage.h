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

#ifndef ATTRIBSTORAGE_H
#define ATTRIBSTORAGE_H

#include "../stdext/types.h"
#include <tuple>
#include <boost/any.hpp>


#pragma pack(push,1) // disable memory alignment

// this class was designed to use less memory as possible
class AttribStorage {
public:
    AttribStorage() : m_attribs(nullptr), m_size(0) { }
    ~AttribStorage() { if(m_attribs) delete[] m_attribs; }

    template<typename T>
    void set(uint8 id, T value) {
        bool done = false;
        for(int i=0;i<m_size;++i) {
            if(std::get<0>(m_attribs[i]) == id) {
                std::get<1>(m_attribs[i]) = value;
                done = true;
                break;
            }
        }
        if(!done) {
            auto attribs = new std::tuple<uint8, boost::any>[m_size+1];
            if(m_size > 0) {
                for(int i=0;i<m_size;++i)
                    attribs[i] = m_attribs[i];
                delete[] m_attribs;
            }
            m_attribs = attribs;
            m_attribs[m_size++] = std::make_tuple(id, value);
        }
    }

    template<typename T>
    T get(uint8 id) const {
        for(int i=0;i<m_size;++i)
            if(std::get<0>(m_attribs[i]) == id)
                return boost::any_cast<T>(std::get<1>(m_attribs[i]));
        return T();
    }

    bool has(uint8 id) const {
        for(int i=0;i<m_size;++i)
            if(std::get<0>(m_attribs[i]) == id)
                return true;
        return false;
    }
private:
    std::tuple<uint8, boost::any>* m_attribs;
    uint8 m_size;
};
#pragma pack(pop)

#endif
