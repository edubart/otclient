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

#ifndef LUABINDER_H
#define LUABINDER_H

// this file is and must be included only from luainterface.h
#include "luainterface.h"
#include "luaexception.h"

#include <framework/stdext/traits.h>
#include <tuple>

/// This namespace contains some dirty metaprogamming that uses a lot of C++0x features
/// The purpose here is to create templates that can bind any function from C++
/// and expose in lua environment. This is done combining variadic templates,
/// lambdas, tuples and some type traits features from the new C++0x standard to create
/// templates that can detect functions's arguments and then generate lambdas. These lambdas
/// pops arguments from lua stack, call the bound C++ function and then
/// pushes the result to lua.
namespace luabinder
{
    /// Pack arguments from lua stack into a tuple recursively
    template<int N>
    struct pack_values_into_tuple {
        template<typename Tuple>
        static void call(Tuple& tuple, LuaInterface* lua) {
            typedef typename std::tuple_element<N-1, Tuple>::type ValueType;
            std::get<N-1>(tuple) = lua->polymorphicPop<ValueType>();
            pack_values_into_tuple<N-1>::call(tuple, lua);
        }
    };
    template<>
    struct pack_values_into_tuple<0> {
        template<typename Tuple>
        static void call(Tuple& tuple, LuaInterface* lua) { }
    };

    /// C++ function caller that can push results to lua
    template<typename Ret, typename F, typename... Args>
    typename std::enable_if<!std::is_void<Ret>::value, int>::type
    call_fun_and_push_result(const F& f, LuaInterface* lua, const Args&... args) {
        Ret ret = f(args...);
        int numRets = lua->polymorphicPush(ret);
        return numRets;
    }

    /// C++ void function caller
    template<typename Ret, typename F, typename... Args>
    typename std::enable_if<std::is_void<Ret>::value, int>::type
    call_fun_and_push_result(const F& f, LuaInterface* lua, const Args&... args) {
        f(args...);
        return 0;
    }

    /// Expand arguments from tuple for later calling the C++ function
    template<int N, typename Ret>
    struct expand_fun_arguments {
        template<typename Tuple, typename F, typename... Args>
        static int call(const Tuple& tuple, const F& f, LuaInterface* lua, const Args&... args) {
            return expand_fun_arguments<N-1,Ret>::call(tuple, f, lua, std::get<N-1>(tuple), args...);
        }
    };
    template<typename Ret>
    struct expand_fun_arguments<0,Ret> {
        template<typename Tuple, typename F, typename... Args>
        static int call(const Tuple& tuple, const F& f, LuaInterface* lua, const Args&... args) {
            return call_fun_and_push_result<Ret>(f, lua, args...);
        }
    };

    /// Bind different types of functions generating a lambda
    template<typename Ret, typename F, typename Tuple>
    LuaCppFunction bind_fun_specializer(const F& f) {
        enum { N = std::tuple_size<Tuple>::value };
        return [=](LuaInterface* lua) -> int {
            while(lua->stackSize() != N) {
                if(lua->stackSize() < N)
                    g_lua.pushNil();
                else
                    g_lua.pop();
            }
            Tuple tuple;
            pack_values_into_tuple<N>::call(tuple, lua);
            return expand_fun_arguments<N,Ret>::call(tuple, f, lua);
        };
    }

    /// Bind a customized function
    inline
    LuaCppFunction bind_fun(const std::function<int(LuaInterface*)>& f) {
        return f;
    }

    /// Bind a std::function
    template<typename Ret, typename... Args>
    LuaCppFunction bind_fun(const std::function<Ret(Args...)>& f) {
        typedef typename std::tuple<typename stdext::remove_const_ref<Args>::type...> Tuple;
        return bind_fun_specializer<typename stdext::remove_const_ref<Ret>::type,
                                    decltype(f),
                                    Tuple>(f);
    }

    /// Specialization for lambdas
    template<typename F>
    struct bind_lambda_fun;

    template<typename Lambda, typename Ret, typename... Args>
    struct bind_lambda_fun<Ret(Lambda::*)(Args...) const> {
        static LuaCppFunction call(const Lambda& f) {
            typedef typename std::tuple<typename stdext::remove_const_ref<Args>::type...> Tuple;
            return bind_fun_specializer<typename stdext::remove_const_ref<Ret>::type,
                                        decltype(f),
                                        Tuple>(f);

        }
    };

    template<typename Lambda>
    typename std::enable_if<std::is_constructible<decltype(&Lambda::operator())>::value, LuaCppFunction>::type bind_fun(const Lambda& f) {
        typedef decltype(&Lambda::operator()) F;
        return bind_lambda_fun<F>::call(f);
    }

    /// Convert to C++ functions pointers to std::function then bind
    template<typename Ret, typename... Args>
    LuaCppFunction bind_fun(Ret (*f)(Args...)) {
        return bind_fun(std::function<Ret(Args...)>(f));
    }

    /// Create member function lambdas
    template<typename Ret, typename C, typename... Args>
    std::function<Ret(const stdext::shared_object_ptr<C>&, const Args&...)> make_mem_func(Ret (C::* f)(Args...)) {
        auto mf = std::mem_fn(f);
        return [=](const stdext::shared_object_ptr<C>& obj, const Args&... args) mutable -> Ret {
            if(!obj)
                throw LuaException("failed to call a member function because the passed object is nil");
            return mf(obj.get(), args...);
        };
    }
    template<typename C, typename... Args>
    std::function<void(const stdext::shared_object_ptr<C>&, const Args&...)> make_mem_func(void (C::* f)(Args...)) {
        auto mf = std::mem_fn(f);
        return [=](const stdext::shared_object_ptr<C>& obj, const Args&... args) mutable -> void {
            if(!obj)
                throw LuaException("failed to call a member function because the passed object is nil");
            mf(obj.get(), args...);
        };
    }

    /// Create member function lambdas for singleton classes
    template<typename Ret, typename C, typename... Args>
    std::function<Ret(const Args&...)> make_mem_func_singleton(Ret (C::* f)(Args...), C* instance) {
        auto mf = std::mem_fn(f);
        return [=](Args... args) mutable -> Ret { return mf(instance, args...); };
    }
    template<typename C, typename... Args>
    std::function<void(const Args&...)> make_mem_func_singleton(void (C::* f)(Args...), C* instance) {
        auto mf = std::mem_fn(f);
        return [=](Args... args) mutable -> void { mf(instance, args...); };
    }


    /// Bind member functions
    template<typename C, typename Ret, class FC, typename... Args>
    LuaCppFunction bind_mem_fun(Ret (FC::* f)(Args...)) {
        typedef typename std::tuple<stdext::shared_object_ptr<FC>, typename stdext::remove_const_ref<Args>::type...> Tuple;
        auto lambda = make_mem_func<Ret,FC>(f);
        return bind_fun_specializer<typename stdext::remove_const_ref<Ret>::type,
                                    decltype(lambda),
                                    Tuple>(lambda);
    }

    /// Bind singleton member functions
    template<typename C, typename Ret, class FC, typename... Args>
    LuaCppFunction bind_singleton_mem_fun(Ret (FC::*f)(Args...), C *instance) {
        typedef typename std::tuple<typename stdext::remove_const_ref<Args>::type...> Tuple;
        assert(instance);
        auto lambda = make_mem_func_singleton<Ret,FC>(f, static_cast<FC*>(instance));
        return bind_fun_specializer<typename stdext::remove_const_ref<Ret>::type,
                                    decltype(lambda),
                                    Tuple>(lambda);
    }

    /// Bind customized member functions
    template<typename C>
    LuaCppFunction bind_mem_fun(int (C::*f)(LuaInterface*)) {
        auto mf = std::mem_fn(f);
        return [=](LuaInterface* lua) mutable -> int {
            auto obj = lua->castValue<stdext::shared_object_ptr<C>>(1);
            lua->remove(1);
            return mf(obj, lua);
        };
    }
}

#endif
