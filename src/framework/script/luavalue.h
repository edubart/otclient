#ifndef LUAVALUE_H
#define LUAVALUE_H

#include "luavaluecasts.h"

/// Utility used to fill a LuaValueList by args...
inline void fillLuaValueList(LuaValueList& argsList) { }
template<class T, class... Args>
void fillLuaValueList(LuaValueList& argsList, const T& first, const Args&... rest) {
   argsList.push_back(safe_to_luavalue(first));
   fillLuaValueList(argsList, rest...);
}

/// LuaValue, all values from and to lua can be represented by this type
class LuaValue : public std::enable_shared_from_this<LuaValue>
{
public:
    LuaValue(int ref = -1);
    virtual ~LuaValue();

    /** If this value is a function or a list of functions, calls it.
    * Otherwise, if is not a nil value, throws an LuaException */
    LuaValueList call(const std::string& funcName, const LuaValueList& args = LuaValueList());
    template<class... T>
    LuaValueList call(const std::string& funcName, const T&... args) {
        LuaValueList argsList;
        push();
        fillLuaValueList(argsList, args...);
        return call(funcName, argsList);
    }

    int getTableSize();
    LuaValueList getTableValues();

    LuaValuePtr getField(const std::string& key);
    void setField(const std::string& key, const LuaValuePtr& value);
    template<typename T>
    void setField(const std::string& key, const T& value) {
        setField(key, safe_to_luavalue<T>(value));
    }

    LuaValuePtr getMetatable();
    void setMetatable(const LuaValuePtr& mt);

    void setTable(int n, const LuaValuePtr& value);

    void setEnvironment(LuaValuePtr env);

    std::string getTypeName();

    // check types
    bool isNil() { return m_ref < 0; }
    bool isBoolean();
    bool isString();
    bool isNumber();
    bool isTable();
    bool isFunction();
    bool isCFunction();
    bool isLuaFunction();
    bool isUserdata();

    // convert types
    bool toBoolean();
    int toInteger();
    double toNumber();
    std::string toString();
    void* toUserdata();

private:
    /// Pushes this value into lua stack
    void push();

    int m_ref;
    friend class LuaState;
};


#endif // LUAVALUE_H
