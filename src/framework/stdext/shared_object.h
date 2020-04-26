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

#ifndef STDEXT_SHARED_OBJECT_H
#define STDEXT_SHARED_OBJECT_H

#include "types.h"
#include <type_traits>
#include <functional>
#include <cassert>
#include <ostream>

#ifdef THREAD_SAFE
#include <atomic>
#endif

namespace stdext {

template<class T>
class shared_object_ptr;

class shared_object
{
public:
    shared_object() : refs(0) { }
    virtual ~shared_object() { }
    void add_ref() { ++refs; }
    void dec_ref() { if(--refs == 0) delete this; }
    refcount_t ref_count() { return refs; }

    template<typename T> stdext::shared_object_ptr<T> static_self_cast() { return stdext::shared_object_ptr<T>(static_cast<T*>(this)); }
    template<typename T> stdext::shared_object_ptr<T> dynamic_self_cast() { return stdext::shared_object_ptr<T>(dynamic_cast<T*>(this)); }
    template<typename T> stdext::shared_object_ptr<T> const_self_cast() { return stdext::shared_object_ptr<T>(const_cast<T*>(this)); }

private:
#ifdef THREAD_SAFE
    std::atomic<refcount_t> refs;
#else
    refcount_t refs;
#endif
};

template<class T>
class shared_object_ptr
{
public:
    typedef T element_type;

    shared_object_ptr(): px(nullptr) { }
    shared_object_ptr(T* p, bool add_ref = true) : px(p) {
        static_assert(std::is_base_of<shared_object, T>::value, "classes using shared_object_ptr must be a derived of stdext::shared_object");
        if(px != nullptr && add_ref)
            this->add_ref();
    }
    shared_object_ptr(shared_object_ptr const& rhs): px(rhs.px) { if(px != nullptr) add_ref(); }
    template<class U>
    shared_object_ptr(shared_object_ptr<U> const& rhs, typename std::enable_if<std::is_convertible<U*,T*>::value, U*>::type = nullptr) : px(rhs.get()) { if(px != nullptr) add_ref(); }
    ~shared_object_ptr() { if(px != nullptr) dec_ref(); }

    void reset() { shared_object_ptr().swap(*this); }
    void reset(T* rhs) { shared_object_ptr(rhs).swap(*this); }
    void swap(shared_object_ptr& rhs) { std::swap(px, rhs.px); }
    T* get() const { return px; }

    refcount_t use_count() const { return ((shared_object*)px)->ref_count(); }
    bool is_unique() const { return use_count() == 1; }

    template<class U> shared_object_ptr& operator=(shared_object_ptr<U> const& rhs) { shared_object_ptr(rhs).swap(*this); return *this; }

    T& operator*() const { assert(px != nullptr); return *px; }
    T* operator->() const { assert(px != nullptr); return px; }

    shared_object_ptr& operator=(shared_object_ptr const& rhs) { shared_object_ptr(rhs).swap(*this); return *this; }
    shared_object_ptr& operator=(T* rhs) { shared_object_ptr(rhs).swap(*this); return *this; }

    // implicit conversion to bool
    typedef T* shared_object_ptr::*unspecified_bool_type;
    operator unspecified_bool_type() const { return px == nullptr ? nullptr : &shared_object_ptr::px; }
    bool operator!() const { return px == nullptr; }

    // std::move support
    shared_object_ptr(shared_object_ptr&& rhs): px(rhs.px) { rhs.px = nullptr; }
    shared_object_ptr& operator=(shared_object_ptr&& rhs) { shared_object_ptr(static_cast<shared_object_ptr&&>(rhs)).swap(*this); return *this; }

private:
    void add_ref() { ((shared_object*)px)->add_ref(); }
    void dec_ref() { ((shared_object*)px)->dec_ref(); }

    T* px;
};

template<class T, class U> bool operator==(shared_object_ptr<T> const& a, shared_object_ptr<U> const& b) { return a.get() == b.get(); }
template<class T, class U> bool operator!=(shared_object_ptr<T> const& a, shared_object_ptr<U> const& b) { return a.get() != b.get(); }
template<class T, class U> bool operator==(shared_object_ptr<T> const& a, U* b) { return a.get() == b; }
template<class T, class U> bool operator!=(shared_object_ptr<T> const& a, U* b) { return a.get() != b; }
template<class T, class U> bool operator==(T * a, shared_object_ptr<U> const& b) { return a == b.get(); }
template<class T, class U> bool operator!=(T * a, shared_object_ptr<U> const& b) { return a != b.get(); }
template<class T> bool operator<(shared_object_ptr<T> const& a, shared_object_ptr<T> const& b) { return std::less<T*>()(a.get(), b.get()); }

template<class T> T* get_pointer(shared_object_ptr<T> const& p) { return p.get(); }
template<class T, class U> shared_object_ptr<T> static_pointer_cast(shared_object_ptr<U> const& p) { return static_cast<T*>(p.get()); }
template<class T, class U> shared_object_ptr<T> const_pointer_cast(shared_object_ptr<U> const& p) { return const_cast<T*>(p.get()); }
template<class T, class U> shared_object_ptr<T> dynamic_pointer_cast(shared_object_ptr<U> const& p) { return dynamic_cast<T*>(p.get()); }
template<class T, typename... Args> stdext::shared_object_ptr<T> make_shared_object(Args... args) { return stdext::shared_object_ptr<T>(new T(args...)); }

// operator<< support
template<class E, class T, class Y> std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T>& os, shared_object_ptr<Y> const& p) { os << p.get(); return os; }

}

namespace std {

// hash, for unordered_map support
template<typename T> struct hash<stdext::shared_object_ptr<T>> { size_t operator()(const stdext::shared_object_ptr<T>& p) const { return std::hash<T*>()(p.get()); } };

// swap support
template<class T> void swap(stdext::shared_object_ptr<T>& lhs, stdext::shared_object_ptr<T>& rhs) { lhs.swap(rhs); }

}

#endif
