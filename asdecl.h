#ifndef __ASDECL_H
#define __ASDECL_H

// Just a decl
#define AS_DECL(__cls) class Ref_##__cls
// Declare a "VALUE" in AS (not used by AS4GTA)

#define AS_SIMPLE_DECL(__cls) void Construct_##__cls(void* ptr){new(ptr) __cls();}void Destruct_##__cls(__cls *ptr){ptr->~__cls();}
// Declares a "REF" (~pointer) that is deletable

#ifdef AS_DECL_FULL
    #define AS_REF_DECL_CONSTRUCT(__cls, __scr_clsname) const char* Ref_##__cls ::sScrClsName=#__scr_clsname ;Ref_##__cls ::Ref_##__cls(){nRef=1;pValue=new __cls();bDelete=true;}Ref_##__cls ::Ref_##__cls(__cls *v){nRef=1;pValue=(void*)v;bDelete=false;}Ref_##__cls ::Ref_##__cls(__cls &v){nRef=1;pValue=(void*)&v;bDelete=false;}Ref_##__cls *Ref_##__cls ::Factory(){return new Ref_##__cls();}void Ref_##__cls ::AddRef(){++nRef;}void Ref_##__cls ::Release(){if(!(--nRef)){if(bDelete)delete (__cls*)pValue;delete this;}}Ref_##__cls &Ref_##__cls ::operator=(const Ref_##__cls &o){if(bDelete)delete (__cls*)pValue;pValue=o.pValue;logger->Info("assign");}bool Ref_##__cls ::GetProp_isnull(){return pValue==nullptr;}
    #define AS_REF_DECL_CONSTRUCT_FROM(__cls, __scr_clsname, __inherit_cls) const char* Ref_##__cls ::sScrClsName=#__scr_clsname ;Ref_##__cls ::Ref_##__cls(){nRef=1;pValue=new __cls();bDelete=true;}Ref_##__cls ::Ref_##__cls(__cls *v){nRef=1;pValue=(void*)v;bDelete=false;}Ref_##__cls ::Ref_##__cls(__cls &v){nRef=1;pValue=(void*)&v;bDelete=false;}Ref_##__cls *Ref_##__cls ::Factory(){return new Ref_##__cls();}Ref_##__cls &Ref_##__cls ::operator=(const Ref_##__cls &o){if(bDelete)delete (__cls*)pValue;pValue=o.pValue;}
#else
    #define AS_REF_DECL_CONSTRUCT(__cls, __scr_clsname) class Ref_##__cls{public:void* pValue;int nRef;bool bDelete;static const char* sScrClsName;Ref_##__cls();Ref_##__cls(__cls *v);Ref_##__cls(__cls &v);static Ref_##__cls *Factory();void AddRef();void Release();Ref_##__cls &operator=(const Ref_##__cls &o);bool GetProp_isnull();
    #define AS_REF_DECL_CONSTRUCT_FROM(__cls, __scr_clsname, __inherit_cls) class Ref_##__cls : public Ref_##__inherit_cls {public:static const char* sScrClsName;Ref_##__cls();Ref_##__cls(__cls *v);Ref_##__cls(__cls &v);static Ref_##__cls *Factory();Ref_##__cls &operator=(const Ref_##__cls &o);
#endif // !AS_DECL_FULL

// Same as above but undeletable pointer
#ifdef AS_DECL_FULL
    #define AS_REF_DECL(__cls, __scr_clsname) const char* Ref_##__cls ::sScrClsName=#__scr_clsname ;Ref_##__cls ::Ref_##__cls(){nRef=1;pValue=nullptr;}Ref_##__cls ::Ref_##__cls(__cls *v){nRef=1;pValue=(void*)v;}Ref_##__cls *Ref_##__cls ::Factory(){return new Ref_##__cls();}void Ref_##__cls ::AddRef(){++nRef;}void Ref_##__cls ::Release(){if(!(--nRef))delete this;}Ref_##__cls &Ref_##__cls ::operator=(const Ref_##__cls &o){pValue=o.pValue;}bool Ref_##__cls ::GetProp_isnull(){return pValue==nullptr;}
    #define AS_REF_DECL_FROM(__cls, __scr_clsname, __inherit_cls) const char* Ref_##__cls ::sScrClsName=#__scr_clsname ;Ref_##__cls ::Ref_##__cls(){nRef=1;pValue=nullptr;}Ref_##__cls ::Ref_##__cls(__cls *v){nRef=1;pValue=(void*)v;}Ref_##__cls *Ref_##__cls ::Factory(){return new Ref_##__cls();}Ref_##__cls &Ref_##__cls ::operator=(const Ref_##__cls &o){pValue=o.pValue;}
#else
    #define AS_REF_DECL(__cls, __scr_clsname) class Ref_##__cls{public:void* pValue;int nRef;static const char* sScrClsName;Ref_##__cls();Ref_##__cls(__cls *v);static Ref_##__cls *Factory();void AddRef();void Release();Ref_##__cls &operator=(const Ref_##__cls &o);bool GetProp_isnull();
    #define AS_REF_DECL_FROM(__cls, __scr_clsname, __inherit_cls) class Ref_##__cls : public Ref_##__inherit_cls {public:static const char* sScrClsName;Ref_##__cls();Ref_##__cls(__cls *v);static Ref_##__cls *Factory();Ref_##__cls &operator=(const Ref_##__cls &o);
#endif

// Stop decl...
#ifdef AS_DECL_FULL
    #define AS_DECL_END()
#else
    #define AS_DECL_END() };
#endif



// A simple property declaration
#ifdef AS_DECL_FULL
    #define AS_PROP_DECL(__cls, __type, __propname, __clsvar) __type &Ref_##__cls ::GetProp_##__propname(){ return ((__cls*)(pValue))->__clsvar; } void Ref_##__cls ::SetProp_##__propname(__type v){ ((__cls*)(pValue))->__clsvar = v; }
    #define AS_PROP_DECL_NOREFBIND(__cls, __type, __propname, __clsvar) __type Ref_##__cls ::GetProp_##__propname(){ return ((__cls*)(pValue))->__clsvar; } void Ref_##__cls ::SetProp_##__propname(__type v){ ((__cls*)(pValue))->__clsvar = v; }
#else
    #define AS_PROP_DECL(__cls, __type, __propname, __clsvar) __type &GetProp_##__propname(); void SetProp_##__propname(__type v);
    #define AS_PROP_DECL_NOREFBIND(__cls, __type, __propname, __clsvar) __type GetProp_##__propname(); void SetProp_##__propname(__type v);
#endif // !AS_DECL_FULL

// A property but returns a reference variable
#ifdef AS_DECL_FULL
    #define AS_PROP_DECLREF(__cls, __refcls, __propname, __clsvar) Ref_##__refcls *Ref_##__cls ::GetProp_##__propname(){ return new Ref_##__refcls(&((__cls*)(pValue))->__clsvar); } void Ref_##__cls ::SetProp_##__propname(Ref_##__refcls *v){ ((__cls*)(pValue))->__clsvar = *(__refcls*)(v->pValue); }
    #define AS_PROP_DECLREFPTR(__cls, __refcls, __propname, __clsvar) Ref_##__refcls *Ref_##__cls ::GetProp_##__propname(){ return new Ref_##__refcls(((__cls*)(pValue))->__clsvar); } void Ref_##__cls ::SetProp_##__propname(Ref_##__refcls *v){ ((__cls*)(pValue))->__clsvar = (__refcls*)v->pValue; }
#else
    #define AS_PROP_DECLREF(__cls, __refcls, __propname, __clsvar) Ref_##__refcls *GetProp_##__propname();void SetProp_##__propname(Ref_##__refcls *v);
    #define AS_PROP_DECLREFPTR(__cls, __refcls, __propname, __clsvar) AS_PROP_DECLREF(__cls, __refcls, __propname, __clsvar)
#endif

// A simple property but only "get" (read-only)
#define AS_PROP_DECL2(__cls, __type, __propname, __clsvar) __type &GetProp_##__propname(){ return pValue->__clsvar; }

// Just a "set" declaration without a function body
#define AS_PROP_DECL_SETNAKED(__cls, __type, __propname) void SetProp_##__propname(__type v)



#define AS_SIMPLE_REG(__cls) \
    engine->RegisterObjectType(Ref_##__cls ::sScrClsName, sizeof(__cls), asOBJ_VALUE | asGetTypeTraits<__cls>()); \
    engine->RegisterObjectBehaviour(Ref_##__cls ::sScrClsName, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Construct_##__cls), asCALL_CDECL_OBJLAST); \
    engine->RegisterObjectBehaviour(Ref_##__cls ::sScrClsName, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(Destruct_##__cls), asCALL_CDECL_OBJLAST); \
    engine->RegisterObjectMethod(Ref_##__cls ::sScrClsName, (std::string(Ref_##__cls ::sScrClsName) + " &opAssign(" + Ref_##__cls ::sScrClsName + " &in)").c_str(), asMETHODPR(__cls, operator=, (const __cls&), __cls&), asCALL_THISCALL);
#define AS_SIMPLE_REG2(__cls) \
    engine->RegisterObjectType(Ref_##__cls ::sScrClsName, sizeof(__cls), asOBJ_VALUE | asGetTypeTraits<__cls>()); \
    engine->RegisterObjectBehaviour(Ref_##__cls ::sScrClsName, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Construct_##__cls), asCALL_CDECL_OBJLAST); \
    engine->RegisterObjectBehaviour(Ref_##__cls ::sScrClsName, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(Destruct_##__cls), asCALL_CDECL_OBJLAST);
    
#define AS_REF_REG(__cls) \
    engine->RegisterObjectType(Ref_##__cls ::sScrClsName, sizeof(Ref_##__cls), asOBJ_REF); \
    engine->RegisterObjectBehaviour(Ref_##__cls ::sScrClsName, asBEHAVE_FACTORY, (std::string(Ref_##__cls ::sScrClsName) + "@ f()").c_str(), asFUNCTION(Ref_##__cls::Factory), asCALL_CDECL); \
    engine->RegisterObjectBehaviour(Ref_##__cls ::sScrClsName, asBEHAVE_ADDREF, "void f()", asMETHOD(Ref_##__cls,AddRef), asCALL_THISCALL); \
    engine->RegisterObjectBehaviour(Ref_##__cls ::sScrClsName, asBEHAVE_RELEASE, "void f()", asMETHOD(Ref_##__cls,Release), asCALL_THISCALL); \
    AS_PROP_REGGET_NOREFBIND(__cls, bool, isnull);
#define AS_REF_REG_CONSTRUCT(__cls) \
    engine->RegisterObjectType(Ref_##__cls ::sScrClsName, sizeof(Ref_##__cls), asOBJ_REF); \
    engine->RegisterObjectBehaviour(Ref_##__cls ::sScrClsName, asBEHAVE_FACTORY, (std::string(Ref_##__cls ::sScrClsName) + "@ f()").c_str(), asFUNCTION(Ref_##__cls::Factory), asCALL_CDECL); \
    engine->RegisterObjectBehaviour(Ref_##__cls ::sScrClsName, asBEHAVE_ADDREF, "void f()", asMETHOD(Ref_##__cls,AddRef), asCALL_THISCALL); \
    engine->RegisterObjectBehaviour(Ref_##__cls ::sScrClsName, asBEHAVE_RELEASE, "void f()", asMETHOD(Ref_##__cls,Release), asCALL_THISCALL); \
    engine->RegisterObjectMethod(Ref_##__cls ::sScrClsName, (std::string(Ref_##__cls ::sScrClsName) + " &opAssign(" + Ref_##__cls ::sScrClsName + " &in)").c_str(), asMETHODPR(__cls, operator=, (const __cls&), __cls&), asCALL_THISCALL); \
    AS_PROP_REGGET_NOREFBIND(__cls, bool, isnull);
#define AS_PROP_REG(__cls, __type, __propname) \
    engine->RegisterObjectMethod(Ref_##__cls ::sScrClsName, #__type "& get_" #__propname "() property", asMETHOD(Ref_##__cls,GetProp_##__propname), asCALL_THISCALL); \
    engine->RegisterObjectMethod(Ref_##__cls ::sScrClsName, "void set_" #__propname "(" #__type ") property", asMETHOD(Ref_##__cls,SetProp_##__propname), asCALL_THISCALL);
#define AS_PROP_REGGET(__cls, __type, __propname) \
    engine->RegisterObjectMethod(Ref_##__cls ::sScrClsName, #__type "& get_" #__propname "() property", asMETHOD(Ref_##__cls,GetProp_##__propname), asCALL_THISCALL);
#define AS_PROP_REGGET_NOREFBIND(__cls, __type, __propname) \
    engine->RegisterObjectMethod(Ref_##__cls ::sScrClsName, #__type " get_" #__propname "() property", asMETHOD(Ref_##__cls,GetProp_##__propname), asCALL_THISCALL);
#define AS_PROP_REGSET(__cls, __type, __propname) \
    engine->RegisterObjectMethod(Ref_##__cls ::sScrClsName, "void set_" #__propname "(" #__type ") property", asMETHOD(Ref_##__cls,SetProp_##__propname), asCALL_THISCALL);

#endif // __ASDECL_H
