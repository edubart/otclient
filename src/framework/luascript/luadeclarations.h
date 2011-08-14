#ifndef LUADECLARATIONS_H
#define LUADECLARATIONS_H

#include <global.h>

class LuaInterface;
class LuaObject;
typedef std::function<int(LuaInterface*)> LuaCppFunction;
typedef std::unique_ptr<LuaCppFunction> LuaCppFunctionPtr;
typedef std::shared_ptr<LuaObject> LuaObjectPtr;

#endif
