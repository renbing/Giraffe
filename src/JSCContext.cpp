//
//  JSCContext.cpp
//  Giraffe
//
//  Created by kins on 12/2/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#include <vector>

#include "JSCContext.h"
#include "stringutil.h"
#include "Point.h"
#include "Image.h"
#include "Text.h"
#include "MovieClip.h"
#include "Http.h"
#include "XMLHttpRequest.h"
#include "Audio.h"
#include "Device.h"

using std::vector;

int Device::width = 0;
int Device::height = 0;
string Device::name = "default";

// C++ 全局导出函数
static JSBool js_trace(JSContext *cx, unsigned argc, jsval *vp ) {
    
    if (argc > 0) {
        string content;
        jsval *argv = JS_ARGV(cx, vp); \
        for( int i=0; i<argc; i++ ){
            content += " " + JSVAL_TO_STDSTRING(cx, argv[i]);
        }
        
        TRACE("trace:%s", content.c_str());
    }
    
    return JS_TRUE;
}

static JSBool js_getUID(JSContext *cx, unsigned argc, jsval *vp ) {
    
    JS_SET_RVAL(cx, vp, INT_TO_JSVAL(1));
    
    return JS_TRUE;
}

static void JS_FinalizeStub(JSFreeOp *freeOp, JSObject *obj) {
    return;
}

static JSClass globalClass = {
    "global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_DeletePropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

JSCContext * JSCContext::m_instance = NULL;

JSCContext::JSCContext()
{
    m_rt = JS_NewRuntime(10 * 1024 * 1024, JS_USE_HELPER_THREADS);
    m_cx = JS_NewContext(m_rt, 10240);
    
    JS_SetOptions(m_cx, JSOPTION_TYPE_INFERENCE);
    JS_SetVersion(m_cx, JSVERSION_LATEST);
    JS_SetOptions(m_cx, JS_GetOptions(m_cx) & ~JSOPTION_METHODJIT);
    JS_SetOptions(m_cx, JS_GetOptions(m_cx) & ~JSOPTION_METHODJIT_ALWAYS);
    
    JS_SetErrorReporter(m_cx, JSCContext::logException);
	m_globalObj = JS_NewGlobalObject(m_cx, &globalClass, NULL);
    JS_InitStandardClasses(m_cx, m_globalObj);
    
    registNative();
    
    gettimeofday(&m_lastTime, NULL);
}

JSCContext::~JSCContext()
{
}

void JSCContext::registNative()
{
    // 注册C++全局函数
    JS_DefineFunction(m_cx, m_globalObj, "trace", js_trace, 1, JSPROP_READONLY);
    JS_DefineFunction(m_cx, m_globalObj, "getUID", js_getUID, 0, JSPROP_READONLY);
    
    // 注册C++类
    CPoint::exportJS("Point", m_cx, m_globalObj, NULL);
    Image::exportJS("Image", m_cx, m_globalObj, NULL);
    Text::exportJS("TextField", m_cx, m_globalObj, NULL);
    MovieClip::exportJS("MovieClip", m_cx, m_globalObj, NULL);
    Bitmap::exportJS("Bitmap", m_cx, m_globalObj, NULL);
    XMLHttpRequest::exportJS("XMLHttpRequest", m_cx, m_globalObj, NULL);
    Audio::exportJS("Audio", m_cx, m_globalObj, NULL);
    
    // 注册C++全局对象
    JSObject *stage = JS_DefineObject(m_cx, m_globalObj, "stage", &MovieClip::js_class, NULL, JSPROP_READONLY);
    JS_SetPrivate(stage, MovieClip::stage);
    
    // 注册Device全局对象 Device{width, height, name}
    jsval device = OBJECT_TO_JSVAL(JS_NewObject(m_cx, NULL, NULL, NULL));
    jsval width = INT_TO_JSVAL(Device::width);
    jsval height = INT_TO_JSVAL(Device::height);
    jsval name = STDSTRING_TO_JSVAL(m_cx, Device::name);
    JS_SetProperty(m_cx, device.toObjectOrNull(), "width", &width);
    JS_SetProperty(m_cx, device.toObjectOrNull(), "height", &height);
    JS_SetProperty(m_cx, device.toObjectOrNull(), "name", &name);
    JS_SetProperty(m_cx, m_globalObj, "Device", &device);
}

void JSCContext::callJSFunction(jsval callback, unsigned int argc, jsval *argv, JSObject *caller)
{
    if( !callback.isObject()) return;
    
    JSObject *funcObj = callback.toObjectOrNull();
    if( !funcObj || !JS_ObjectIsFunction(m_cx, funcObj)) return;
    if( !caller ){
        caller = m_globalObj;
    }
    
    jsval rval;
    if( !JS_CallFunctionValue(m_cx, caller, callback, argc, argv, &rval) ) {
        ERROR("callJSFunction fail");
    }
}

void JSCContext::logException(JSContext *cx, const char *message, JSErrorReport *report)
{
    ERROR("jsError: %s:%u:%u, %s\n    %s\n",
        report->filename ? report->filename : "<no filename=\"filename\">",
        (unsigned int) report->lineno, report->column,
        message, report->linebuf);
};

bool JSCContext::run(const char *path)
{
    if( !path ) return false;
    
    m_path = path;
    if( !StringUtil::startWith(m_path, "http://") ) {
        if( !StringUtil::endWith(m_path, "/") ){
            m_path += "/";
        }
    }
    
	// 下载index.js分析js文件列表(\n分割)
    string indexContent;
    if( loadFile("index.js", indexContent) <= 0 ) {
        ERROR("JSCContet::run can't load index.js");
        return false;
    }
    
	// 下载所有js文件内容
    vector<string> jsFiles;
    StringUtil::split(indexContent, "\n", jsFiles);
    
    JSBool ok;
    jsval rval;
    for(int i=0; i<jsFiles.size(); i++ )
    {
        const string &jsFile = jsFiles[i];
        
		if( jsFile.size() <= 0 || StringUtil::startWith(jsFile, "//") )
		{
			continue;
		}
        
        string jsContent;
        if( loadFile(jsFile, jsContent) <= 0 ) {
            ERROR("JSContext::run can't load js file %s", jsFile.c_str());
            continue;
        }
        ok = JS_EvaluateScript(m_cx, m_globalObj, jsContent.c_str(), jsContent.size(),
                                   jsFile.c_str(), 0, &rval);
            
        if( !ok ) return false;
	}

	// 获取JS加载完成执行主函数main
    ok = JS_LookupProperty(m_cx, m_globalObj, "main", &rval);
    if( !ok || JSVAL_IS_VOID(rval) ) {
        ERROR("jsError: no main function");
        return false;
    }
    
    callJSFunction(rval);
    
    ok = JS_LookupProperty(m_cx, m_globalObj, "mainLoop", &m_mainLoop);
    if( !ok || JSVAL_IS_VOID(m_mainLoop) ) {
        ERROR("jsError: no mainLoop function");
        return false;
    }
    
    ok = JS_LookupProperty(m_cx, m_globalObj, "onPinch", &m_onPinch);
    if( !ok || JSVAL_IS_VOID(m_onPinch) ) {
        ERROR("jsError: no onPinch function");
        return false;
    }
    
    return true;
}

void JSCContext::addToSlot(JSObject *obj, const char *name, JSObject *value)
{
    JSObject *slot = JSVAL_TO_OBJECT(JS_GetReservedSlot(obj, 0));
    jsval jsvalValue = OBJECT_TO_JSVAL(value);
    JS_SetProperty(m_cx, slot, name, &jsvalValue);
}

void JSCContext::removeFromSlot(JSObject *obj, const char *name)
{
    JSObject *slot = JSVAL_TO_OBJECT(JS_GetReservedSlot(obj, 0));
    JS_DeleteProperty(m_cx, slot, name);
}

jsval JSCContext::createEvent(const Event *e)
{
    JSObject *event = JS_NewObject(m_cx, NULL, NULL, NULL);
    jsval direction = INT_TO_JSVAL(e->direction);
    jsval type = INT_TO_JSVAL(e->type);
    
    jsval point = OBJECT_TO_JSVAL(JS_NewObject(m_cx, NULL, NULL, NULL));
    jsval x = INT_TO_JSVAL(e->point.x);
    jsval y = INT_TO_JSVAL(e->point.y);
    JS_SetProperty(m_cx, point.toObjectOrNull(), "x", &x);
    JS_SetProperty(m_cx, point.toObjectOrNull(), "y", &y);
    
    jsval move = OBJECT_TO_JSVAL(JS_NewObject(m_cx, NULL, NULL, NULL));
    x = INT_TO_JSVAL(e->move.x);
    y = INT_TO_JSVAL(e->move.y);
    JS_SetProperty(m_cx, move.toObjectOrNull(), "x", &x);
    JS_SetProperty(m_cx, move.toObjectOrNull(), "y", &y);
    
    JS_SetProperty(m_cx, event, "direction", &direction);
    JS_SetProperty(m_cx, event, "point", &point);
    JS_SetProperty(m_cx, event, "move", &move);
    JS_SetProperty(m_cx, event, "type", &type);
    
    return OBJECT_TO_JSVAL(event);
}

void JSCContext::runMainLoop()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    float passed = (now.tv_sec - m_lastTime.tv_sec) + (now.tv_usec - m_lastTime.tv_usec) / 1000000.0f;
    m_lastTime = now;
    
    jsval args[] = {DOUBLE_TO_JSVAL(passed)};
    callJSFunction(m_mainLoop, 1, args);
    
    Http::getInstance()->update();
    
    JS_GC(m_rt);
}

void JSCContext::onPinch(float scale)
{
    jsval args[] = {DOUBLE_TO_JSVAL(scale)};
    callJSFunction(m_onPinch, 1, args);
}

int JSCContext::loadFile(const string &file, string &content)
{
    string fullPath = m_path + file;
    
    if( StringUtil::startWith(fullPath, "http://")){
        return Http::getInstance()->Get(fullPath, content);
    }else{
        return loadLocalFile(fullPath, content);
    }
    
    return 0;
}

void loadFileAsyncCallback(Downloader *downloader, void *arg)
{
    LoadFileCallbackWrapper *wrapper = static_cast<LoadFileCallbackWrapper *>(arg);
    int loaded = -1;
    if( downloader->getStatus() == 200 ) {
        loaded = downloader->getDataLen();
    }
    
    wrapper->callback(downloader->getData(), loaded, wrapper->callbackArg);
}

void JSCContext::loadFileAsync(const string &file, LoadFileCallback callback, void *callbackArg)
{
    string fullPath = m_path + file;
    
    if( StringUtil::startWith(fullPath, "http://")){
        LoadFileCallbackWrapper *wrapper = new LoadFileCallbackWrapper;
        wrapper->callback = callback;
        wrapper->callbackArg = callbackArg;
        
        Http::getInstance()->Get(fullPath, loadFileAsyncCallback, wrapper);
    }else{
        loadLocalFileAsync(fullPath, callback, callbackArg);
    }
}
