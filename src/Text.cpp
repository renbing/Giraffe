//
//  Text.m
//  Giraffe
//
//  Created by kins on 2/22/13.
//  Copyright (c) 2013 kins. All rights reserved.
//


#include "Text.h"
#include "stringutil.h"

JS_PROPERTY(Text, INT, width)
JS_PROPERTY(Text, INT, height)
JS_PROPERTY(Text, STRING, text)
JS_PROPERTY(Text, STRING, color)
JS_PROPERTY(Text, STRING, align)
JS_PROPERTY(Text, STRING, font)

JS_SIMPLE_FUNC(Text, render, VOID)

JSPropertySpec Text::js_ps[] = {
    JS_PROPERTY_DEF(Text, width),
    JS_PROPERTY_DEF(Text, height),
    JS_PROPERTY_DEF(Text, text),
    JS_PROPERTY_DEF(Text, color),
    JS_PROPERTY_DEF(Text, align),
    JS_PROPERTY_DEF(Text, font),
    JS_PS_END
};

JSFunctionSpec Text::js_fs[] = {
    JS_FN_DEF(Text, render, 0),
    JS_FS_END
};


JSPropertySpec Text::js_sps[] = {
    JS_PS_END
};
JSFunctionSpec Text::js_sfs[] = {
    JS_FS_END
};

JS_CLASS_EXPORT(Text)

Text::Text()
{
    hasAlpha = true;
    premultiAlpha = true;
    
    width = POTWidth = 64;
    height = POTHeight = 64;
    
    text = "default";
    color = "red";
    align = "center";
    font = "";
}
