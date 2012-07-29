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

#ifndef STDEXT_SHARED_OBJECT_H
#define STDEXT_SHARED_OBJECT_H

#include <boost/checked_delete.hpp>
#include <boost/intrusive_ptr.hpp>

namespace stdext {

template<typename T>
using shared_object_ptr = boost::intrusive_ptr<T>;

class shared_object
{
public:
    shared_object() : m_refs(0) { }
    virtual ~shared_object() { }
    void add_ref() { ++m_refs; assert(m_refs != 0xffffffff); }
    void dec_ref() {
        if(--m_refs == 0)
            boost::checked_delete(this);
    }
    bool is_unique_ref() { return m_refs == 1; }
    unsigned long ref_count() { return m_refs; }
    template<typename T>
    shared_object_ptr<T> self_cast() { return shared_object_ptr<T>(static_cast<T*>(this)); }
    template<typename T>
    shared_object_ptr<T> dynamic_self_cast() { return shared_object_ptr<T>(dynamic_cast<T*>(this)); }

private:
    unsigned int m_refs;
};

template<class T, typename... Args>
shared_object_ptr<T> make_shared_object(Args... args) { return shared_object_ptr<T>(new T(args...)); }

}

namespace std {
template<typename T>
struct hash<boost::intrusive_ptr<T>> : public __hash_base<size_t, boost::intrusive_ptr<T>> {
    size_t operator()(const boost::intrusive_ptr<T>& p) const noexcept { return std::hash<T*>()(p.get()); }
};
}

template<typename T>
struct remove_const_ref {
    typedef typename std::remove_const<typename std::remove_reference<T>::type>::type type;
};

template<typename T>
void intrusive_ptr_add_ref(T* p) { (static_cast<stdext::shared_object*>(p))->add_ref(); }

template<typename T>
void intrusive_ptr_release(T* p) { (static_cast<stdext::shared_object*>(p))->dec_ref(); }

#endif


