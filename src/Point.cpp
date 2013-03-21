//
//  Point.cpp
//  Giraffe
//
//  Created by kins on 12/2/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#include "Point.h"

JS_PROPERTY(CPoint, INT, x)
JS_PROPERTY(CPoint, INT, y)
JS_PROPERTY(CPoint, OBJECT, callback)
JS_PROPERTY_STATIC(CPoint, STRING, NAME)

JS_SIMPLE_FUNC(CPoint, talk, VOID)

JS_SIMPLE_SFUNC(CPoint, call, VOID)

JSPropertySpec CPoint::js_ps[] = {
    JS_PROPERTY_DEF(CPoint, x),
    JS_PROPERTY_DEF(CPoint, y),
    JS_PROPERTY_DEF(CPoint, callback),
    JS_PS_END
};

JSFunctionSpec CPoint::js_fs[] = {
    JS_FN_DEF(CPoint, talk, 0),
    JS_FS_END
};


JSPropertySpec CPoint::js_sps[] = {
    JS_PROPERTY_STATIC_DEF(CPoint, NAME),
    JS_PS_END
};
JSFunctionSpec CPoint::js_sfs[] = {
    JS_SFN_DEF(CPoint, call, 0),
    JS_FS_END
};

JS_CLASS_EXPORT(CPoint)

const string CPoint::NAME = "POINT";

void CPoint::call()
{
    LOG("CPoint static call\n");
}

void CPoint::talk()
{
    LOG("CPoint talk\n");
    JSCContext::getInstance()->callJSFunction(callback, 0, NULL);
}
