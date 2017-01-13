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

#ifndef STDEXT_ANY_H
#define STDEXT_ANY_H

#include <algorithm>
#include <cassert>
#include <typeinfo>

namespace stdext {

class any {
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

    any() : content(nullptr) { }
    any(const any& other) : content(other.content ? other.content->clone() : nullptr) { }
    template<typename T> any(const T& value) : content(new holder<T>(value)) { }
    ~any() { if(content) delete content; }

    any& swap(any& rhs) { std::swap(content, rhs.content); return *this; }

    template<typename T> any& operator=(const T& rhs) { any(rhs).swap(*this); return *this; }
    any& operator=(any rhs) { rhs.swap(*this); return *this; }

    bool empty() const { return !content; }
    template<typename T> const T& cast() const;
    const std::type_info & type() const { return content ? content->type() : typeid(void); }
};

template<typename T>
const T& any_cast(const any& operand) {
    assert(operand.type() == typeid(T));
    return static_cast<any::holder<T>*>(operand.content)->held;
}

template<typename T> const T& any::cast() const { return any_cast<T>(*this); }

}

#endif
