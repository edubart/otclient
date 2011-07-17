#ifndef SCRIPTOBJECT_H
#define SCRIPTOBJECT_H

#include <global.h>

class ScriptObject : public boost::enable_shared_from_this<ScriptObject>
{
public:
    ScriptObject() : m_scriptTableRef(-1) { }
    virtual ~ScriptObject() { releaseScriptObject(); }

    void releaseScriptObject();

    //TODO: global destroy
    virtual const char *getScriptObjectType() const { return NULL; }

    int getScriptTable();
    void callScriptTableField(const std::string& field, int numArgs = 0);

private:
    int m_scriptTableRef;
};

typedef boost::shared_ptr<ScriptObject> ScriptObjectPtr;

#endif // SCRIPTOBJECT_H
