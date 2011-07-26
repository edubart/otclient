#ifndef LUAOBJECT_H
#define LUAOBJECT_H

#include "luavalue.h"

/// LuaObject, all bound classes must be derived from it
class LuaObject : public std::enable_shared_from_this<LuaObject>
{
public:
    LuaObject();
    virtual ~LuaObject();

    /// Call a field of this lua object
    template<typename... T>
    LuaValueList callField(const std::string& field, const T&... args) {
        // note that we must retrive the field from or lua value (to use the __index metamethod)
        // so we cannot use our getField here
        auto fieldFunc = toLuaValue()->getField(field);
        auto funcName = make_string(getLuaTypeName(), ":", field);
        return fieldFunc->call(funcName, shared_from_this(), args...);
    }

    /// Set a field of this lua object
    template<typename T>
    void setField(const std::string& key, const T& value) {
        m_luaTable->setField(key, value);
    }

    /// Get a field of this lua object
    LuaValuePtr getField(const std::string& key) {
        return m_luaTable->getField(key);
    }

    /// Returns current use count by the shared_ptr
    int getUseCount() { return shared_from_this().use_count() - 1; }

    /// Class name used in lua, must be overridden by derived classes
    virtual const char* getLuaTypeName() const = 0;

    /** Convert to a lua value for pushing this object into lua stack.
     * Each call to this will create a new pointer, thus increasing the use count.*/
    LuaValuePtr toLuaValue();

    LuaObjectPtr asLuaObject() { return shared_from_this(); }

private:
    LuaValuePtr m_luaTable;
};

#endif // LUAOBJECT_H
