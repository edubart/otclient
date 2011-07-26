#ifndef LUABINDER_H
#define LUABINDER_H

#include "luavalue.h"
#include "luastate.h"

namespace luabinder
{
    // transform const T& -> T
    template<typename T>
    struct remove_const_ref {
        typedef typename std::remove_const<typename std::remove_reference<T>::type>::type type;
    };

    // pack an value into tuple recursively
    template<int N>
    struct pack_values_into_tuple {
        template<typename Tuple>
        static void call(Tuple& tuple, LuaState* lua) {
            typedef typename std::tuple_element<N-1, Tuple>::type ValueType;
            std::get<N-1>(tuple) = safe_luavalue_cast<ValueType>(lua->popValue());
            pack_values_into_tuple<N-1>::call(tuple, lua);
        }
    };
    template<>
    struct pack_values_into_tuple<0> {
        template<typename Tuple>
        static void call(Tuple &tuple, LuaState* lua) { }
    };

    // call function
    template<typename Ret, typename F, typename... Args>
    typename std::enable_if<!std::is_void<Ret>::value, int>::type
    call_fun_and_push_result(const F& f, LuaState* lua, Args... args) {
        Ret ret = f(args...);
        lua->pushValue(safe_to_luavalue(ret));
        return 1;
    }

    // call function with no return
    template<typename Ret, typename F, typename... Args>
    typename std::enable_if<std::is_void<Ret>::value, int>::type
    call_fun_and_push_result(const F& f, LuaState* lua, Args... args) {
        f(args...);
        return 0;
    }

    // expand function arguments for calling
    template<int N, typename Ret>
    struct expand_fun_arguments {
        template<typename Tuple, typename F, typename... Args>
        static int call(const Tuple& tuple, const F& f, LuaState* lua, Args... args) {
            return expand_fun_arguments<N-1,Ret>::call(tuple, f, lua, std::cref(std::get<N-1>(tuple)), args...);
        }
    };
    template<typename Ret>
    struct expand_fun_arguments<0,Ret> {
        template<typename Tuple, typename F, typename... Args>
        static int call(const Tuple& tuple, const F& f, LuaState* lua, Args... args) {
            return call_fun_and_push_result<Ret>(f, lua, args...);
        }
    };

    // bind different types of functions
    template<typename Ret, typename F, typename Tuple>
    LuaCppFunction bind_fun_specializer(const F& f) {
        enum { N = std::tuple_size<Tuple>::value };
        return [=](LuaState* lua) {
            if(lua->stackSize() != N)
                throw LuaBadNumberOfArgumentsException(N, lua->stackSize());
            Tuple tuple;
            pack_values_into_tuple<N>::call(tuple, lua);
            return expand_fun_arguments<N,Ret>::call(tuple, f, lua);
        };
    }

    // bind a std::function
    template<typename Ret, typename... Args>
    LuaCppFunction bind_fun(const std::function<Ret(Args...)>& f) {
        typedef typename std::tuple<typename remove_const_ref<Args>::type...> Tuple;
        return bind_fun_specializer<typename remove_const_ref<Ret>::type,
                                    decltype(f),
                                    Tuple>(f);
    }

    // bind a custumized function
    inline
    LuaCppFunction bind_fun(const std::function<int(LuaState*)>& f) {
        return f;
    }

    // convert to std::function then bind
    template<typename Ret, typename... Args>
    LuaCppFunction bind_fun(Ret (*f)(Args...)) {
        return bind_fun(std::function<Ret(Args...)>(f));
    }

    // a tuple_element that works with the next algorithm
    template<std::size_t __i, typename _Tp>
        struct tuple_element;
    template<std::size_t __i, typename _Head, typename... _Tail>
        struct tuple_element<__i, std::tuple<_Head, _Tail...> >
        : tuple_element<__i - 1, std::tuple<_Tail...> > { };
    template<typename _Head, typename... _Tail>
        struct tuple_element<0, std::tuple<_Head, _Tail...> > { typedef _Head type; };
    template<typename _Head>
        struct tuple_element<-1,std::tuple<_Head>> { typedef void type; };
    template<std::size_t __i>
        struct tuple_element<__i,std::tuple<>> { typedef void type; };

    // rebind functions already binded by std::bind that have placeholders
    template<typename Enable, int N, typename ArgsTuple, typename HoldersTuple, typename... Args>
    struct get_holded_tuple;

    template<int N, typename ArgsTuple, typename HoldersTuple, typename... Args>
    struct get_holded_tuple<typename std::enable_if<(N > 0 && std::is_placeholder<typename tuple_element<N-1, HoldersTuple>::type>::value > 0), void>::type, N, ArgsTuple, HoldersTuple, Args...> {
        typedef typename std::tuple_element<N-1, HoldersTuple>::type holder_type;
        typedef typename tuple_element<std::is_placeholder<holder_type>::value-1, ArgsTuple>::type _arg_type;
        typedef typename remove_const_ref<_arg_type>::type arg_type;
        typedef typename get_holded_tuple<void, N-1, ArgsTuple, HoldersTuple, arg_type, Args...>::type type;
    };
    template<int N, typename ArgsTuple, typename HoldersTuple, typename... Args>
    struct get_holded_tuple<typename std::enable_if<(N > 0 && std::is_placeholder<typename tuple_element<N-1, HoldersTuple>::type>::value == 0), void>::type, N, ArgsTuple, HoldersTuple, Args...> {
        typedef typename get_holded_tuple<void, N-1, ArgsTuple, HoldersTuple, Args...>::type type;
    };
    template<typename ArgsTuple, typename HoldersTuple, typename... Args>
    struct get_holded_tuple<void, 0, ArgsTuple, HoldersTuple, Args...> {
        typedef typename std::tuple<Args...> type;
    };

    template<typename Ret, typename... Args, typename... Holders>
    LuaCppFunction bind_fun(const std::_Bind<Ret (*(Holders...))(Args...)>& f) {
        typedef typename std::tuple<Args...> ArgsTuple;
        typedef typename std::tuple<Holders...> HoldersTuple;
        typedef typename get_holded_tuple<void, sizeof...(Holders), ArgsTuple, HoldersTuple>::type Tuple;
        return bind_fun_specializer<typename remove_const_ref<Ret>::type,
                                    decltype(f),
                                    Tuple>(f);
    }

    template<typename Obj, typename Ret, typename... Args, typename... Holders>
    LuaCppFunction bind_fun(const std::_Bind<std::_Mem_fn<Ret (Obj::*)(Args...)>(Obj*, Holders...)>& f) {
        typedef typename std::tuple<Args...> ArgsTuple;
        typedef typename std::tuple<Holders...> HoldersTuple;
        typedef typename get_holded_tuple<void, sizeof...(Holders), ArgsTuple, HoldersTuple>::type Tuple;
        return bind_fun_specializer<typename remove_const_ref<Ret>::type,
                                    decltype(f),
                                    Tuple>(f);
    }

    // custumized functions already binded by std::bind doesn't need to be bind again
    template<typename Obj>
    LuaCppFunction bind_fun(const std::_Bind<std::_Mem_fn<int (Obj::*)(LuaState*)>(Obj*, std::_Placeholder<1>)>& f) {
        return f;
    }
    inline
    LuaCppFunction bind_fun(const std::_Bind<int (*(std::_Placeholder<1>))(LuaState*)>& f) {
        return f;
    }

    // bind member function
    template<typename Ret, typename Obj, typename... Args>
    LuaCppFunction bind_mem_fun(Ret (Obj::*f)(Args...)) {
        auto mf = std::mem_fn(f);
        typedef typename std::tuple<Obj*, typename remove_const_ref<Args>::type...> Tuple;
        return bind_fun_specializer<typename remove_const_ref<Ret>::type,
                                    decltype(mf),
                                    Tuple>(mf);
    }
    template<typename Ret, typename Obj, typename... Args>
    LuaCppFunction bind_mem_fun(Ret (Obj::*f)(Args...) const) {
        auto mf = std::mem_fn(f);
        typedef typename std::tuple<Obj*, typename remove_const_ref<Args>::type...> Tuple;
        return bind_fun_specializer<typename remove_const_ref<Ret>::type,
                                    decltype(mf),
                                    Tuple>(mf);
    }

    // bind custumized member function
    template<typename Obj>
    LuaCppFunction bind_mem_fun(int (Obj::*f)(LuaState*)) {
        auto mf = std::mem_fn(f);
        return [=](LuaState* lua) {
            lua->insert(1);
            auto obj = safe_luavalue_cast<Obj*>(lua->popValue());
            return mf(obj, lua);
        };
    }
}

#endif // LUABINDER_H
