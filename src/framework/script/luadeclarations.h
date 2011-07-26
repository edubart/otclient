#ifndef LUADECLARATIONS_H
#define LUADECLARATIONS_H

#include <global.h>

class LuaInterface;
class LuaState;
class LuaValue;
class LuaObject;

typedef std::function<int(LuaState*)> LuaCppFunction;

typedef std::shared_ptr<LuaCppFunction> LuaCppFunctionPtr;
typedef std::shared_ptr<LuaValue> LuaValuePtr;
typedef std::shared_ptr<LuaObject> LuaObjectPtr;

typedef std::vector<LuaValuePtr> LuaValueList;

#endif // LUADECLARATIONS_H
