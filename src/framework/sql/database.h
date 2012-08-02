#ifndef DATABASE_H
#define DATABASE_H

#include "declarations.h"
#include <framework/global.h>
#include <framework/luaengine/luaobject.h>

class Database : public LuaObject
{
public:
    Database();
    ~Database();
};

#endif
