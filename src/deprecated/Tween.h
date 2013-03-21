//
//  Tween.h
//  Giraffe
//
//  Created by kins on 1/15/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#ifndef __Giraffe__Tween__
#define __Giraffe__Tween__

#include <list>

#include "MovieClip.h"

using std::list;

typedef enum{
    TweenTypeLiner,
    
    TweenTypeBackEaseIn,
    TweenTypeBackEaseOut,
    TweenTypeBackEaseInOut,
    
    TweenTypeBounceEaseIn,
    TweenTypeBounceEaseOut,
    TweenTypeBounceEaseInOut,
    
    TweenTypeRegularEaseIn,
    TweenTypeRegularEaseOut,
    TweenTypeRegularEaseInOut,
    
    TweenTypeStrongEaseIn,
    TweenTypeStrongEaseOut,
    TweenTypeStrongEaseInOut,
} TweenType;

typedef struct {
    float *refer;
    float from;
    float to;
} TweenChange;

class Tween {
private:
    typedef enum {
        TweenStateWaiting, TweenStateDelaying, TweenStateWorking
    } TweenState;
    
    typedef float (*TweenMethod)(float t, float c, float d);
    
    TweenMethod         method;
    float               duration;
    float               delay;
    
    vector<TweenChange> changes;
    
    float           passed;
    Tween           *next;
    TweenState      state;

private:
    static list<Tween *> m_tweens;
    
public:
    static void update(float passed);
    
    static Tween & move(DisplayObject *obj, TweenType type, float duration, float x, float y, float delay = 0);
    static Tween & scale(DisplayObject *obj, TweenType type, float duration, float sx, float sy, float delay = 0);
    static Tween & rotate(DisplayObject *obj, TweenType type, float duration, float rotation, float delay = 0);
    
    static Tween & tween(TweenType type, float duration, vector<TweenChange> &changes, float delay=0);
    static Tween & tween(TweenType type, float duration, TweenChange &change, float delay=0);
    
    Tween & seqMove(DisplayObject *obj, TweenType type, float duration, float x, float y, float delay = 0);
    Tween & seqScale(DisplayObject *obj, TweenType type, float duration, float sx, float sy, float delay = 0);
    Tween & seqRotate(DisplayObject *obj, TweenType type, float duration, float rotation, float delay = 0);
    
    Tween & seqTween(TweenType type, float duration, vector<TweenChange> &changes, float delay=0);
    Tween & seqTween(TweenType type, float duration, TweenChange &change, float delay=0);
    
private:
    Tween() {
        next = NULL;
        passed = 0;
        state = TweenStateWaiting;
    }
    
    void setTweenType(TweenType type);
};

#endif /* defined(__Giraffe__Tween__) */
