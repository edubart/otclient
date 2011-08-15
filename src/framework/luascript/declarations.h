#ifndef FRAMEWORK_LUA_DECLARATIONS_H
#define FRAMEWORK_LUA_DECLARATIONS_H

#include <framework/global.h>

class LuaInterface;
class LuaObject;

typedef std::function<int(LuaInterface*)> LuaCppFunction;
typedef std::unique_ptr<LuaCppFunction> LuaCppFunctionPtr;
typedef std::shared_ptr<LuaObject> LuaObjectPtr;

#endif
