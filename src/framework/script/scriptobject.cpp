#include "scriptobject.h"
#include "scriptcontext.h"

#include <core/dispatcher.h>

void ScriptObject::releaseScriptObject()
{
    if(m_scriptTableRef != -1) {
        g_lua.releaseRef(m_scriptTableRef);
        m_scriptTableRef = -1;
    }
}

int ScriptObject::getScriptTable()
{
    if(m_scriptTableRef == -1) {
        g_lua.newTable();
        m_scriptTableRef = g_lua.popRef();
    }
    return m_scriptTableRef;
}

void ScriptObject::callScriptTableField(const std::string& field, int numArgs)
{
    // set self
    g_lua.pushClassInstance(shared_from_this());
    g_lua.setGlobal("self");

    // push field
    g_lua.getScriptObjectField(shared_from_this(), field);

    // call it if its a function
    if(g_lua.isFunction()) {
        g_lua.insert(-numArgs-1);
        g_lua.callFunction(numArgs);
    // if its an array call each element
    } else if(g_lua.isTable()) {
        //TODO: call here with arguments
        g_lua.pushNil();
        while(g_lua.next()) {
            // call it if its a function
            if(g_lua.isFunction())
                g_lua.callFunction();
            g_lua.pop();
        }
    } else if(!g_lua.isNil()) {
        g_lua.reportError(make_string("field '", field, "' for '", getScriptObjectType(), "' is not a valid function or array of functions"));
    }

    // release self
    g_lua.pushNil();
    g_lua.setGlobal("self");
}

