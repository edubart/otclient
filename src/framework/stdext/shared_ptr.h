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

#ifndef STDEXT_SHARED_PTR_H
#define STDEXT_SHARED_PTR_H

#include "types.h"
#include "dumper.h"
#include <type_traits>
#include <functional>
#include <cassert>
#include <ostream>

#ifdef THREAD_SAFE
#include <atomic>
#endif

namespace stdext {

template<class T>
class shared_ptr;

template<class T>
class weak_ptr;

template<class T>
class shared_base
{
public:
    shared_base(T* p) : refs(1), weaks(0), px(p) { }
    ~shared_base() { }

    void add_ref() { ++refs; }
    void dec_ref() {
        if(--refs == 0) {
            delete px;
            px = nullptr;
            if(weaks == 0)
                delete this;
        }
    }

    void add_weak_ref() { ++weaks; }
    void dec_weak_ref() {
        if(--weaks == 0 && refs == 0)
            delete this;
    }

    void set(T *p) {
        T* tmp = px;
        px = p;
        if(tmp)
            delete tmp;
    }
    T *get() { return px; }

    refcount_t ref_count() { return refs; }
    refcount_t weak_count() { return weaks; }
    bool expired() { return refs == 0; }

private:
#ifdef THREAD_SAFE
    std::atomic<refcount_t> refs;
    std::atomic<refcount_t> weaks;
    std::atomic<T*> px;
#else
    refcount_t refs;
    refcount_t weaks;
    T* px;
#endif
};

template<class T>
class shared_ptr
{
public:
    typedef T element_type;

    shared_ptr() : base(nullptr) { }
    shared_ptr(T* p) { if(p != nullptr) base = new shared_base<T>(p); else base = nullptr; }
    shared_ptr(shared_ptr const& rhs): base(rhs.base) { if(base != nullptr) base->add_ref(); }
    template<class U>
    shared_ptr(shared_ptr<U> const& rhs, typename std::is_convertible<U,T>::type* = nullptr) : base(rhs.base) { if(base != nullptr) base->add_ref(); }
    ~shared_ptr() { if(base != nullptr) base->dec_ref(); }

    void reset() { shared_ptr().swap(*this); }
    void reset(T* rhs) { shared_ptr(rhs).swap(*this); }
    void swap(shared_ptr& rhs) { std::swap(base, rhs.base); }

    void set(T* p) { assert(p != nullptr); base->set(p); }
    T* get() const { return base ? base->get() : nullptr; }

    refcount_t use_count() const { return base ? base->ref_count() : 0; }
    refcount_t weak_count() const { return base ? base->weak_count() : 0; }
    bool is_unique() const { return use_count() == 1; }

    T& operator*() const { assert(base != nullptr); return *base->get(); }
    T* operator->() const { assert(base != nullptr); return base->get(); }

    template<class U>
    shared_ptr& operator=(shared_ptr<U> const& rhs) { shared_ptr(rhs).swap(*this); return *this; }
    shared_ptr& operator=(shared_ptr const& rhs) { shared_ptr(rhs).swap(*this); return *this; }
    shared_ptr& operator=(T* rhs) { shared_ptr(rhs).swap(*this); return *this; }

    // implicit conversion to bool
    typedef shared_base<T>* shared_ptr::*unspecified_bool_type;
    operator unspecified_bool_type() const { return base == nullptr ? nullptr : &shared_ptr::base; }
    bool operator!() const { return base == nullptr; }

    // std::move support
    shared_ptr(shared_ptr&& rhs): base(rhs.base) { rhs.base = nullptr; }
    shared_ptr& operator=(shared_ptr&& rhs) { shared_ptr(static_cast<shared_ptr&&>(rhs)).swap(*this); return *this; }

private:
    shared_ptr(shared_base<T> *base) {
        if(base && !base->expired()) {
            base->add_ref();
            this->base = base;
        } else
            base = nullptr;
    }
    shared_base<T> *base;

    friend class weak_ptr<T>;
};

template<class T>
class weak_ptr {
public:
    typedef T element_type;

    weak_ptr() : base(nullptr) { }
    weak_ptr(shared_ptr<T> const& rhs): base(rhs.base) { if(base != nullptr) base->add_weak_ref(); }
    template<class U>
    weak_ptr(shared_ptr<U> const& rhs, typename std::is_convertible<U,T>::type* = nullptr) : base(rhs.base) { if(base != nullptr) base->add_weak_ref(); }
    weak_ptr(weak_ptr<T> const& rhs): base(rhs.base) { if(base != nullptr) base->add_weak_ref(); }
    template<class U>
    weak_ptr(weak_ptr<U> const& rhs, typename std::is_convertible<U,T>::type* = nullptr) : base(rhs.base) { if(base != nullptr) base->add_weak_ref(); }
    ~weak_ptr() { if(base != nullptr) base->dec_weak_ref(); }

    void reset() { weak_ptr().swap(*this); }
    void swap(weak_ptr& rhs) { std::swap(base, rhs.base); }

    T* get() const { return base ? base->get() : nullptr; }
    refcount_t use_count() const { return base ? base->ref_count() : 0; }
    refcount_t weak_count() const { return base ? base->weak_count() : 0; }
    bool expired() const { return base ? base->expired() : false; }
    shared_ptr<T> lock() { return shared_ptr<T>(base); }

    weak_ptr& operator=(weak_ptr const& rhs) { weak_ptr(rhs).swap(*this); return *this; }
    template<class U>
    weak_ptr& operator=(shared_ptr<U> const& rhs) { weak_ptr(rhs).swap(*this); return *this; }
    template<class U>
    weak_ptr& operator=(weak_ptr<T> const& rhs) { weak_ptr(rhs).swap(*this); return *this; }

    // implicit conversion to bool
    typedef shared_base<T>* weak_ptr::*unspecified_bool_type;
    operator unspecified_bool_type() const { return base == nullptr ? nullptr : &weak_ptr::base; }
    bool operator!() const { return !expired(); }

    // std::move support
    weak_ptr(weak_ptr&& rhs): base(rhs.base) { rhs.base = nullptr; }
    weak_ptr& operator=(weak_ptr&& rhs) { weak_ptr(static_cast<weak_ptr&&>(rhs)).swap(*this); return *this; }

private:
    shared_base<T> *base;
};

template<class T, class U> bool operator==(shared_ptr<T> const& a, shared_ptr<U> const& b) { return a.get() == b.get(); }
template<class T, class U> bool operator==(weak_ptr<T> const& a, weak_ptr<U> const& b) { return a.get() == b.get(); }
template<class T, class U> bool operator==(shared_ptr<T> const& a, weak_ptr<U> const& b) { return a.get() == b.get(); }
template<class T, class U> bool operator==(weak_ptr<T> const& a, shared_ptr<U> const& b) { return a.get() == b.get(); }
template<class T, class U> bool operator!=(shared_ptr<T> const& a, shared_ptr<U> const& b) { return a.get() != b.get(); }
template<class T, class U> bool operator!=(weak_ptr<T> const& a, weak_ptr<U> const& b) { return a.get() != b.get(); }
template<class T, class U> bool operator!=(shared_ptr<T> const& a, weak_ptr<U> const& b) { return a.get() != b.get(); }
template<class T, class U> bool operator!=(weak_ptr<T> const& a, shared_ptr<U> const& b) { return a.get() != b.get(); }
template<class T, class U> bool operator==(shared_ptr<T> const& a, U* b) { return a.get() == b; }
template<class T, class U> bool operator==(weak_ptr<T> const& a, U* b) { return a.get() == b; }
template<class T, class U> bool operator!=(shared_ptr<T> const& a, U* b) { return a.get() != b; }
template<class T, class U> bool operator!=(weak_ptr<T> const& a, U* b) { return a.get() != b; }
template<class T, class U> bool operator==(T * a, shared_ptr<U> const& b) { return a == b.get(); }
template<class T, class U> bool operator==(T * a, weak_ptr<U> const& b) { return a == b.get(); }
template<class T, class U> bool operator!=(T * a, shared_ptr<U> const& b) { return a != b.get(); }
template<class T, class U> bool operator!=(T * a, weak_ptr<U> const& b) { return a != b.get(); }
template<class T> bool operator<(shared_ptr<T> const& a, shared_ptr<T> const& b) { return std::less<T*>()(a.get(), b.get()); }
template<class T> bool operator<(weak_ptr<T> const& a, weak_ptr<T> const& b) { return std::less<T*>()(a.get(), b.get()); }

template<class T> T* get_pointer(shared_ptr<T> const& p) { return p.get(); }
template<class T> T* get_pointer(weak_ptr<T> const& p) { return p.get(); }
template<class T, class U> shared_ptr<T> static_pointer_cast(shared_ptr<U> const& p) { return static_cast<T*>(p.get()); }
template<class T, class U> shared_ptr<T> const_pointer_cast(shared_ptr<U> const& p) { return const_cast<T*>(p.get()); }
template<class T, class U> shared_ptr<T> dynamic_pointer_cast(shared_ptr<U> const& p) { return dynamic_cast<T*>(p.get()); }
template<class T, typename... Args> stdext::shared_ptr<T> make_shared(Args... args) { return stdext::shared_ptr<T>(new T(args...)); }

// operator<< support
template<class E, class T, class Y> std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T>& os, shared_ptr<Y> const& p) { os << p.get(); return os; }
template<class E, class T, class Y> std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T>& os, weak_ptr<Y> const& p) { os << p.get(); return os; }

}

namespace std {

// hash, for unordered_map support
template<typename T> struct hash<stdext::shared_ptr<T>> { size_t operator()(const stdext::shared_ptr<T>& p) const { return std::hash<T*>()(p.get()); } };
template<typename T> struct hash<stdext::weak_ptr<T>> { size_t operator()(const stdext::weak_ptr<T>& p) const { return std::hash<T*>()(p.get()); } };

// swap support
template<class T> void swap(stdext::shared_ptr<T>& lhs, stdext::shared_ptr<T>& rhs) { lhs.swap(rhs); }
template<class T> void swap(stdext::weak_ptr<T>& lhs, stdext::weak_ptr<T>& rhs) { lhs.swap(rhs); }

}

#endif
