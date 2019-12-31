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

#ifndef STDEXT_PACKEDANY_H
#define STDEXT_PACKEDANY_H

#include <algorithm>
#include <cassert>
#include <type_traits>
#include <typeinfo>

namespace stdext {

// disable memory alignment
#pragma pack(push,1)

template<typename T>
struct can_pack_in_any : std::integral_constant<bool,
    (sizeof(T) <= sizeof(void*) && std::is_trivial<T>::value)> {};

// improved to use less memory
class packed_any {
public:
    struct placeholder {
        virtual ~placeholder()  { }
        virtual const std::type_info& type() const = 0;
        virtual placeholder* clone() const = 0;
    };

    template<typename T>
    struct holder : public placeholder {
        holder(const T& value) : held(value) { }
        const std::type_info& type() const { return typeid(T); }
        placeholder* clone() const { return new holder(held); }
        T held;
    private:
        holder& operator=(const holder &);
    };

    placeholder* content;
    bool scalar;

    packed_any() :
        content(nullptr), scalar(false) { }
    packed_any(const packed_any& other) :
        content(!other.scalar && other.content ? other.content->clone() : other.content),
        scalar(other.scalar) { }
    template<typename T>
    packed_any(const T& value, typename std::enable_if<(can_pack_in_any<T>::value)>::type* = nullptr) :
        content(reinterpret_cast<placeholder*>(static_cast<std::size_t>(value))), scalar(true) { }
    template<typename T>
    packed_any(const T& value, typename std::enable_if<!(can_pack_in_any<T>::value)>::type* = nullptr) :
        content(new holder<T>(value)), scalar(false) { }
    ~packed_any()
        { if(!scalar && content) delete content; }

    packed_any& swap(packed_any& rhs) { std::swap(content, rhs.content); std::swap(scalar, rhs.scalar); return *this; }

    template<typename T> packed_any& operator=(const T& rhs) { packed_any(rhs).swap(*this); return *this; }
    packed_any& operator=(packed_any rhs) { rhs.swap(*this); return *this; }

    bool empty() const { return !scalar && !content; }
    template<typename T> T cast() const;
    const std::type_info& type() const {
        if(!scalar)
            return content ? content->type() : typeid(void);
        else
            return typeid(std::size_t);
    }
};

template<typename T>
typename std::enable_if<can_pack_in_any<T>::value, T>::type
packed_any_cast(const packed_any& operand) {
    assert(operand.scalar);
    union {
        T v;
        packed_any::placeholder* content;
    };
    content = operand.content;
    return v;
}

template<typename T>
typename std::enable_if<!can_pack_in_any<T>::value, T>::type
packed_any_cast(const packed_any& operand) {
    assert(operand.type() == typeid(T));
    return static_cast<packed_any::holder<T>*>(operand.content)->held;
}

template<typename T> T packed_any::cast() const { return packed_any_cast<T>(*this); }

// restore memory alignment
#pragma pack(pop)

}

#endif
