//
//  Point.h
//  Giraffe
//
//  Created by kins on 12/2/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#ifndef __Giraffe__Point__
#define __Giraffe__Point__

#include <string>
#include "JSCContext.h"

using std::string;

class CPoint
{
public:
    int x,y;
    void move(int dx, int dy);
    void talk();
    static void call();
    static const string NAME;
    JSObject *callback;
    JSObject *jsthis;

public:
    JS_CLASS_EXPORT_DEF
};

#endif /* defined(__Giraffe__Point__) */
