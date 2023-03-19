#ifndef _IAS4AML_H
#define _IAS4AML_H

#ifdef ANGELSCRIPT_H
    #define AS_MODULE_TYPE asIScriptModule
#else // ANGELSCRIPT_H
    #warning "It looks like you forgot to include angelscript.h BEFORE ias4aml.h! Or you dont have it?"
    #define AS_MODULE_TYPE void
#endif // !ANGELSCRIPT_H

typedef void (*ScriptRegisteredCB)(AS_MODULE_TYPE*);

class IAS4AML
{
public:
    virtual void OnScriptRegisteredCallback(ScriptRegisteredCB) = 0;

    virtual void Scripts_GameStart() = 0;
    virtual void Scripts_GamePause() = 0;
    virtual void Scripts_GameResume() = 0;
    virtual void Scripts_GameStop() = 0;
    virtual void Scripts_Update() = 0;
};

// Those are some kind of prefabs
// It is not required to implement 'em
// but if you need - they`re always ready!

#endif // _IAS4AML_H
