//
//  JSCContext.h
//  Giraffe
//
//  Created by kins on 12/2/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#ifndef __Giraffe__JSCContex__
#define __Giraffe__JSCContex__

#include <sys/time.h>
#include <string>
#include <jsapi.h>
#include "global.h"

using std::string;

class Event;

typedef void (*LoadFileCallback)(const void *data, int size, void *arg);
typedef struct
{
    LoadFileCallback callback;
    void             *callbackArg;
}LoadFileCallbackWrapper;

class JSCContext
{
private:
    JSContext *m_cx;
    JSRuntime *m_rt;
    JSObject *m_globalObj;
    jsval m_mainLoop;
    jsval m_onPinch;
    string m_path;
    static JSCContext *m_instance;
    
    timeval m_lastTime;
    
public:
    ~JSCContext();
    static JSCContext *getInstance();

    bool run(const char *path);
    void callJSFunction(jsval callback, unsigned int argc = 0, jsval *argv = NULL, JSObject *caller=NULL);
    void callJSFunction(JSObject *callback, unsigned int argc = 0, jsval *argv = NULL, JSObject *caller=NULL) {
        callJSFunction(OBJECT_TO_JSVAL(callback), argc, argv, caller);
    }
    
    void addToSlot(JSObject *obj, const char *name, JSObject *value);
    void removeFromSlot(JSObject *obj, const char *name);
    
    void runMainLoop();
    void onPinch(float scale);

    const string &path() { return m_path; }
    JSContext *jsContext() { return m_cx; }
    JSObject *globalObj() { return m_globalObj; }
    
    jsval createEvent(const Event *e);
    
    int loadFile(const string &file, string &content);
    int loadLocalFile(const string &file, string &content);
    
    void loadFileAsync(const string &file, LoadFileCallback callback, void *callbackArg=NULL);
    void loadLocalFileAsync(const string &file, LoadFileCallback callback, void *callbackArg=NULL);
    
    static void logException(JSContext *cx, const char *message, JSErrorReport *report);

private:
    JSCContext();
    void registNative();
};

inline JSCContext * JSCContext::getInstance()
{
    if( m_instance == NULL ) {
        m_instance = new JSCContext();
    }
    
    return m_instance;
}

inline string JSVAL_TO_STDSTRING(JSContext *cx, jsval v)
{
    char *rawStr = JS_EncodeString(cx, JS_ValueToString(cx, v));
    string ret = string(rawStr);
    JS_free(cx, rawStr);
    return ret;
}

inline jsval STDSTRING_TO_JSVAL(JSContext *cx, const string &v)
{
    JSString *str = JS_NewStringCopyZ(cx, v.c_str());
    if( str == NULL ){
        //str = JS_NewUCStringCopyZ(cx, (jschar *)v.c_str());
        int i=0;
    }
    return STRING_TO_JSVAL(str);
}

inline jsval STDSTRING_TO_JSVAL(JSContext *cx, const char *v)
{
    JSString *str = JS_NewStringCopyZ(cx, v);
    if( str == NULL ){
        //str = JS_NewUCStringCopyZ(cx, (jschar *)v);
    }
    return STRING_TO_JSVAL(str);
}

inline float MY_JSVAL_TO_NUMBER(JSContext *cx, jsval v)
{
    if( JSVAL_IS_INT(v)) {
        return (float)JSVAL_TO_INT(v);
    }else if( JSVAL_IS_NUMBER(v) ) {
        return JSVAL_TO_DOUBLE(v);
    }
    
    return 0.0;
}

#define MY_VOID_TO_JSVAL(cx, v)     JS::UndefinedValue()
#define MY_INT_TO_JSVAL(cx, v)      INT_TO_JSVAL(v)
#define MY_BOOL_TO_JSVAL(cx, v)     BOOLEAN_TO_JSVAL(v)
#define MY_NUMBER_TO_JSVAL(cx, v)   DOUBLE_TO_JSVAL(v)
#define MY_STRING_TO_JSVAL(cx, v)   STDSTRING_TO_JSVAL(cx, v)
#define MY_OBJECT_TO_JSVAL(cx, v)   OBJECT_TO_JSVAL(v)

#define MY_JSVAL_TO_INT(cx, v)      JSVAL_TO_INT(v)
#define MY_JSVAL_TO_BOOL(cx, v)     JSVAL_TO_BOOLEAN(v)
#define MY_JSVAL_TO_STRING(cx, v)   JSVAL_TO_STDSTRING(cx, v)
#define MY_JSVAL_TO_OBJECT(cx, v)   JSVAL_TO_OBJECT(v)


#define JS_PROPERTY_READONLY(className, varType, varName) \
static JSBool js_get_##className##_##varName(JSContext *cx, JSHandleObject obj, JSHandleId id, JSMutableHandleValue vp) {\
    className *cobj = static_cast<className *>(JS_GetPrivate(obj.get()));\
    if( cobj == NULL ) return JS_FALSE; \
    vp.set(MY_##varType##_TO_JSVAL(cx, cobj->varName));\
    return JS_TRUE; \
}

#define JS_PROPERTY_WRITE(className, varType, varName) \
static JSBool js_set_##className##_##varName(JSContext *cx, JSHandleObject obj, JSHandleId id, JSBool strict, JSMutableHandleValue vp) {\
    className *cobj = static_cast<className *>(JS_GetPrivate(obj.get()));\
    if( cobj == NULL ) return JS_FALSE; \
    cobj->varName = MY_JSVAL_TO_##varType(cx, vp.get());\
    return JS_TRUE; \
}

#define JS_PROPERTY_READONLY_FUNC(className, varType, varName) \
static JSBool js_get_##className##_##varName(JSContext *cx, JSHandleObject obj, JSHandleId id, JSMutableHandleValue vp) {\
    className *cobj = static_cast<className *>(JS_GetPrivate(obj.get()));\
    if( cobj == NULL ) return JS_FALSE; \
    vp.set(MY_##varType##_TO_JSVAL(cx, cobj->get_##varName()));\
    return JS_TRUE; \
}

#define JS_PROPERTY_WRITE_FUNC(className, varType, varName) \
static JSBool js_set_##className##_##varName(JSContext *cx, JSHandleObject obj, JSHandleId id, JSBool strict, JSMutableHandleValue vp) {\
    className *cobj = static_cast<className *>(JS_GetPrivate(obj.get()));\
    if( cobj == NULL ) return JS_FALSE; \
    cobj->set_##varName(MY_JSVAL_TO_##varType(cx, vp));\
    return JS_TRUE; \
}

#define JS_PROPERTY_WRITE_SLOT(className, varType, varName) \
static JSBool js_set_##className##_##varName(JSContext *cx, JSHandleObject obj, JSHandleId id, JSBool strict, JSMutableHandleValue vp) {\
    className *cobj = static_cast<className *>(JS_GetPrivate(obj.get()));\
    if( cobj == NULL ) return JS_FALSE; \
    JSObject *slot = JSVAL_TO_OBJECT(JS_GetReservedSlot(obj.get(), 0)); \
    JS_SetProperty(cx, slot, #varName, vp.address()); \
    cobj->varName = MY_JSVAL_TO_##varType(cx, vp.get());\
    return JS_TRUE; \
}

#define JS_PROPERTY_STATIC(className, varType, varName) \
static JSBool js_sget_##className##_##varName(JSContext *cx, JSHandleObject obj, JSHandleId id, JSMutableHandleValue vp) {\
    *(vp.address()) = MY_##varType##_TO_JSVAL(cx, className::varName);\
\
    return JS_TRUE; \
}

#define JS_PROPERTY(className, varType, varName) \
JS_PROPERTY_READONLY(className, varType, varName) \
JS_PROPERTY_WRITE(className, varType, varName)

#define JS_PROPERTY_FUNC(className, varType, varName) \
JS_PROPERTY_READONLY_FUNC(className, varType, varName) \
JS_PROPERTY_WRITE_FUNC(className, varType, varName)

#define JS_PROPERTY_SLOT(className, varType, varName) \
JS_PROPERTY_READONLY(className, varType, varName) \
JS_PROPERTY_WRITE_SLOT(className, varType, varName)

#define JS_PROPERTY_READONLY_DEF(className, varName) \
{#varName, 0, JSPROP_READONLY | JSPROP_PERMANENT, js_get_##className##_##varName, NULL, NULL, NULL}

#define JS_PROPERTY_DEF(className, varName) \
{#varName, 1, JSPROP_SHARED | JSPROP_PERMANENT, {js_get_##className##_##varName, NULL}, {js_set_##className##_##varName, NULL}}

#define JS_PROPERTY_STATIC_DEF(className, varName) \
{#varName, 0, JSPROP_READONLY | JSPROP_PERMANENT, js_sget_##className##_##varName, NULL, NULL, NULL}

#define JS_PS_END \
{0, 0, 0, 0, 0}

template<class T>
static void js_destructor(JSFreeOp *freeOp, JSObject *obj)
{
    TRACE("GC ");
    T *cobj = static_cast<T*>(JS_GetPrivate(obj));
    if( cobj ) {
        delete cobj;
    }
}

template<class T>
static T * js_native_constructor(JSContext *cx, unsigned argc, jsval *vp) {
    T *cobj = new T();
    return cobj;
}

template<class T>
static void js_native_constructor_end(JSContext *cx, T *cobj) {
}

template<class T>
static JSBool js_constructor(JSContext *cx, unsigned argc, jsval *vp) {
    
    T *cobj = js_native_constructor<T>(cx, argc, vp);
    JSObject *obj = JS_NewObject(cx, &T::js_class, NULL, NULL);
    cobj->jsthis = obj;
    JSObject *slot = JS_NewObject(cx, NULL, NULL, NULL);
    JS_SetReservedSlot(obj, 0, OBJECT_TO_JSVAL(slot));
    JS_SetPrivate(obj, cobj);
    
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
    js_native_constructor_end<T>(cx, cobj);
    return JS_TRUE;
}

#define JS_FN_DEF(className, funcName, argc) \
JS_FN(#funcName, js_func_##className##_##funcName, argc, JSPROP_PERMANENT | JSPROP_SHARED)

#define JS_SFN_DEF(className, funcName, argc) \
JS_FN(#funcName, js_sfunc_##className##_##funcName, argc, JSPROP_PERMANENT | JSPROP_SHARED)

#define JS_SIMPLE_FUNC(className, funcName, returnType)\
static JSBool js_func_##className##_##funcName(JSContext *cx, unsigned argc, jsval *vp) { \
	JSObject *obj = JS_THIS_OBJECT(cx, vp); \
    className *cobj = static_cast<className *>(JS_GetPrivate(obj)); \
    if( cobj == NULL ) return JS_FALSE; \
    if( #returnType == "VOID" ) { \
        cobj->funcName(); JS_SET_RVAL(cx, vp, JS::UndefinedValue()); \
    }else{ \
        JS_SET_RVAL(cx, vp, MY_##returnType##_TO_JSVAL(cx, cobj->funcName())); \
    } \
\
    return JS_TRUE;\
}

#define JS_FUNC_WITH_ARG1(className, funcName, returnType, arg1Type)\
static JSBool js_func_##className##_##funcName(JSContext *cx, unsigned argc, jsval *vp) { \
	JSObject *obj = JS_THIS_OBJECT(cx, vp); \
    className *cobj = static_cast<className *>(JS_GetPrivate(obj)); \
    if( cobj == NULL ) return JS_FALSE; \
    if( argc <= 0 ) return JS_FALSE; \
    jsval argv1 = *(JS_ARGV(cx, vp)); \
    if( #returnType == "VOID" ) { \
        cobj->funcName(MY_JSVAL_TO_##arg1Type(cx,argv1)); JS_SET_RVAL(cx, vp, JS::UndefinedValue()); \
    }else{ \
        JS_SET_RVAL(cx, vp, MY_##returnType##_TO_JSVAL(cx, cobj->funcName(MY_JSVAL_TO_##arg1Type(cx,argv1)))); \
    } \
\
    return JS_TRUE;\
}

#define JS_FUNC_WITH_ARG2(className, funcName, returnType, arg1Type, arg2Type)\
static JSBool js_func_##className##_##funcName(JSContext *cx, unsigned argc, jsval *vp) { \
	JSObject *obj = JS_THIS_OBJECT(cx, vp); \
    className *cobj = static_cast<className *>(JS_GetPrivate(obj)); \
    if( cobj == NULL ) return JS_FALSE; \
    if( argc <= 0 ) return JS_FALSE; \
    jsval *argv = JS_ARGV(cx, vp); \
    if( #returnType == "VOID" ) { \
        cobj->funcName(MY_JSVAL_TO_##arg1Type(cx,argv[0]), MY_JSVAL_TO_##arg2Type(cx,argv[1])); \
        JS_SET_RVAL(cx, vp, JS::UndefinedValue()); \
    }else{ \
        JS_SET_RVAL(cx, vp, MY_##returnType##_TO_JSVAL(cx, cobj->funcName(MY_JSVAL_TO_##arg1Type(cx,argv[0]), \
            MY_JSVAL_TO_##arg2Type(cx,argv[1])))); \
    } \
\
    return JS_TRUE;\
}

#define JS_FUNC_WITH_ARG3(className, funcName, returnType, arg1Type, arg2Type, arg3Type)\
static JSBool js_func_##className##_##funcName(JSContext *cx, unsigned argc, jsval *vp) { \
	JSObject *obj = JS_THIS_OBJECT(cx, vp); \
    className *cobj = static_cast<className *>(JS_GetPrivate(obj)); \
    if( cobj == NULL ) return JS_FALSE; \
    if( argc <= 0 ) return JS_FALSE; \
    jsval *argv = JS_ARGV(cx, vp); \
    if( #returnType == "VOID" ) { \
        cobj->funcName(MY_JSVAL_TO_##arg1Type(cx,argv[0]), MY_JSVAL_TO_##arg2Type(cx,argv[1]), \
            MY_JSVAL_TO_##arg3Type(cx,argv[2])); \
        JS_SET_RVAL(cx, vp, JS::UndefinedValue()); \
    }else{ \
        JS_SET_RVAL(cx, vp, MY_##returnType##_TO_JSVAL(cx, cobj->funcName(MY_JSVAL_TO_##arg1Type(cx,argv[0]), \
            MY_JSVAL_TO_##arg2Type(cx,argv[1]), MY_JSVAL_TO_##arg3Type(cx,argv[2])))); \
    } \
\
    return JS_TRUE;\
}


#define JS_FUNC_WITH_ARG4(className, funcName, returnType, arg1Type, arg2Type, arg3Type, arg4Type)\
static JSBool js_func_##className##_##funcName(JSContext *cx, unsigned argc, jsval *vp) { \
	JSObject *obj = JS_THIS_OBJECT(cx, vp); \
    className *cobj = static_cast<className *>(JS_GetPrivate(obj)); \
    if( cobj == NULL ) return JS_FALSE; \
    if( argc <= 0 ) return JS_FALSE; \
    jsval *argv = JS_ARGV(cx, vp); \
    if( #returnType == "VOID" ) { \
        cobj->funcName(MY_JSVAL_TO_##arg1Type(cx,argv[0]), MY_JSVAL_TO_##arg2Type(cx,argv[1]), \
            MY_JSVAL_TO_##arg3Type(cx,argv[2]), MY_JSVAL_TO_##arg4Type(cx,argv[3])); \
        JS_SET_RVAL(cx, vp, JS::UndefinedValue()); \
    }else{ \
        JS_SET_RVAL(cx, vp, MY_##returnType##_TO_JSVAL(cx, cobj->funcName(MY_JSVAL_TO_##arg1Type(cx,argv[0]), \
            MY_JSVAL_TO_##arg2Type(cx,argv[1]), MY_JSVAL_TO_##arg3Type(cx,argv[2]), MY_JSVAL_TO_##arg4Type(cx,argv[3])))); \
    } \
\
    return JS_TRUE;\
}

#define JS_SIMPLE_SFUNC(className, funcName, returnType)\
static JSBool js_sfunc_##className##_##funcName(JSContext *cx, unsigned argc, jsval *vp) { \
    if( #returnType == "VOID" ) { \
        className::funcName(); JS_SET_RVAL(cx, vp, JS::UndefinedValue()); \
    }else{ \
        JS_SET_RVAL(cx, vp, MY_##returnType##_TO_JSVAL(cx, className::funcName())); \
    } \
\
    return JS_TRUE;\
}

#define JS_CLASS_EXPORT_DEF \
static JSClass js_class; \
static JSPropertySpec js_ps[]; \
static JSPropertySpec js_sps[]; \
static JSFunctionSpec js_fs[]; \
static JSFunctionSpec js_sfs[]; \
static void exportJS(const char *name, JSContext *cx, JSObject *pobj, JSObject *pproto);


#define JS_CLASS_EXPORT(className) \
JSClass className::js_class = { \
    "", JSCLASS_HAS_PRIVATE | JSCLASS_HAS_RESERVED_SLOTS(1), \
    JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_StrictPropertyStub, \
    JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,js_destructor<className>, \
    JSCLASS_NO_OPTIONAL_MEMBERS \
}; \
\
void className::exportJS(const char *name, JSContext *cx, JSObject *pobj, JSObject *pproto) \
{ \
    className::js_class.name = name; \
    JS_InitClass(cx, pobj, pproto, &className::js_class, js_constructor<className>, \
                 0, className::js_ps,className::js_fs,className::js_sps, className::js_sfs); \
}

#endif /* defined(__Giraffe__JSCContex__) */
