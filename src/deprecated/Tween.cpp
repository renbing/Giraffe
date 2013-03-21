//
//  Tween.cpp
//  Giraffe
//
//  Created by kins on 1/15/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#include <math.h>
#include "Tween.h"

#define PI 3.1415926535897932384626433832795f

// t经过时间 c变化值 d总时间
static float liner(float t, float c, float d)
{
    return c * t / d;
}

static float backEaseIn(float t, float c, float d)
{
    float s = 1.70158;
    return c * (t /= d) * t * ((s + 1) * t - s);
}

static float backEaseOut(float t, float c, float d)
{
    float s = 1.70158;
    return c * ((t = t / d - 1) * t * ((s + 1) * t + s) + 1);
}

static float backEaseInOut(float t, float c, float d)
{
    float s = 1.70158;
    if((t /= d / 2) < 1) {
        return c / 2 * (t * t * (((s *= (1.525)) + 1) * t - s));
    }
    return c / 2 * ((t -= 2) * t * (((s *= (1.525)) + 1) * t + s) + 2);
}

static float bounceEaseOut(float t, float c, float d)
{
    if((t /= d) < (1 / 2.75)) {
        return c * (7.5625 * t * t);
    } else if(t < (2 / 2.75)) {
        return c * (7.5625 * (t -= (1.5 / 2.75)) * t + 0.75);
    } else if(t < (2.5 / 2.75)) {
        return c * (7.5625 * (t -= (2.25 / 2.75)) * t + 0.9375);
    } else {
        return c * (7.5625 * (t -= (2.625 / 2.75)) * t + 0.984375);
    }
}

static float bounceEaseIn(float t, float c, float d)
{
    return c - bounceEaseOut(d - t, c, d);
}

static float bounceEaseInOut(float t, float c, float d)
{
    if(t < d / 2) {
        return bounceEaseIn(t * 2, c, d) * 0.5;
    } else {
        return bounceEaseOut(t * 2 - d, c, d) * 0.5 + c * 0.5;
    }
}

static float regularEaseIn(float t, float c, float d)
{
    return c * (t /= d) * t;
}

static float regularEaseOut(float t, float c, float d)
{
    return-c * (t /= d) * (t - 2);
}
static float regularEaseInOut(float t, float c, float d)
{
    if((t /= d / 2) < 1) {
        return c / 2 * t * t;
    }
    return-c / 2 * ((--t) * (t - 2) - 1);
}

static float strongEaseIn(float t, float c, float d)
{
    return c * (t /= d) * t * t * t * t;
}

static float strongEaseOut(float t, float c, float d)
{
    return c * ((t = t / d - 1) * t * t * t * t + 1);
}

static float strongEaseInOut(float t, float c, float d)
{
    if((t /= d / 2) < 1) {
        return c / 2 * t * t * t * t * t;
    }
    return c / 2 * ((t -= 2) * t * t * t * t + 2);
}

list<Tween *> Tween::m_tweens = list<Tween *>();

Tween & Tween::move(DisplayObject *obj, TweenType type, float duration, float x, float y, float delay)
{
    vector<TweenChange> changes;
    TweenChange changeX = {&obj->x, obj->x, x};
    changes.push_back(changeX);
    TweenChange changeY = {&obj->y, obj->y, y};
    changes.push_back(changeY);
    
    return Tween::tween(type, duration, changes, delay);
}

Tween & Tween::scale(DisplayObject *obj, TweenType type, float duration, float sx, float sy, float delay)
{
    vector<TweenChange> changes;
    TweenChange changeX = {&obj->scaleX, obj->scaleX, sx};
    changes.push_back(changeX);
    TweenChange changeY = {&obj->scaleY, obj->scaleY, sy};
    changes.push_back(changeY);
    
    return Tween::tween(type, duration, changes, delay);
}

Tween & Tween::rotate(DisplayObject *obj, TweenType type, float duration, float rotation, float delay)
{
    TweenChange change = {&obj->rotation, obj->rotation, rotation};
    
    return Tween::tween(type, duration, change, delay);
}

Tween & Tween::seqMove(DisplayObject *obj, TweenType type, float duration, float x, float y, float delay)
{
    Tween &tween = Tween::move(obj, type, duration, x, y, delay);
    tween.state = TweenStateWaiting;
    this->next = &tween;
    
    return tween;
}

Tween & Tween::seqScale(DisplayObject *obj, TweenType type, float duration, float sx, float sy, float delay)
{
    Tween &tween = Tween::scale(obj, type, duration, sx, sy, delay);
    tween.state = TweenStateWaiting;
    this->next = &tween;
    
    return tween;
}

Tween & Tween::seqRotate(DisplayObject *obj, TweenType type, float duration, float rotation, float delay)
{
    Tween &tween = Tween::rotate(obj, type, duration, rotation, delay);
    tween.state = TweenStateWaiting;
    this->next = &tween;
    
    return tween;
}

Tween & Tween::tween(TweenType type, float duration, vector<TweenChange> &changes, float delay)
{
    Tween *tween = new Tween();
    tween->duration = duration;
    tween->delay = delay;
    tween->state = TweenStateDelaying;
    tween->setTweenType(type);
    tween->changes = changes;
    
    m_tweens.push_back(tween);
    return *tween;
}

Tween & Tween::seqTween(TweenType type, float duration, vector<TweenChange> &changes, float delay)
{
    Tween &tween = Tween::tween(type, duration, changes, delay);
    tween.state = TweenStateWaiting;
    this->next = &tween;
    
    return tween;
}

Tween & Tween::tween(TweenType type, float duration, TweenChange &change, float delay)
{
    vector<TweenChange> changes;
    changes.push_back(change);
    
    return Tween::tween(type, duration, changes, delay);
}

Tween & Tween::seqTween(TweenType type, float duration, TweenChange &change, float delay)
{
    vector<TweenChange> changes;
    changes.push_back(change);
    
    return Tween::seqTween(type, duration, changes, delay);
    
}

void Tween::update(float passed)
{
    list<Tween *>::iterator it = m_tweens.begin();
    for( ; it != m_tweens.end(); it++ ) {
        Tween *tween = *it;
        if( tween->state == TweenStateWaiting ) {
            continue;
        }else if( tween->state == TweenStateDelaying ) {
            tween->passed += passed;
            if( tween->passed >= tween->delay ) {
                tween->state = TweenStateWorking;
                tween->passed = 0;
            }
        }else {
            tween->passed += passed;
            if( tween->passed >= tween->duration ) {
                for( int i=0,max=tween->changes.size(); i<max; i++ ) {
                    TweenChange &change = tween->changes.at(i);
                    *change.refer = change.to;
                }
                
                if( tween->next ) {
                    tween->next->state = TweenStateDelaying;
                    for( int i=0,max=tween->next->changes.size(); i<max; i++ ) {
                        TweenChange &change = tween->next->changes.at(i);
                        change.from = *change.refer;
                    }
                }
                delete tween;
                m_tweens.erase(it);
            }else {
                // 缓动
                for( int i=0,max=tween->changes.size(); i<max; i++ ) {
                    TweenChange &change = tween->changes.at(i);
                    float changed = tween->method(tween->passed, change.to-change.from, tween->duration);
                    *change.refer = change.from + changed;
                }
            }
        }
    }
}


void Tween::setTweenType(TweenType type)
{
    switch(type) {
        case TweenTypeLiner:
            this->method = liner;
            break;
            
        case TweenTypeBackEaseIn:
            this->method = backEaseIn;
            break;
        case TweenTypeBackEaseOut:
            this->method = backEaseOut;
            break;
        case TweenTypeBackEaseInOut:
            this->method = backEaseInOut;
            break;
            
        case TweenTypeBounceEaseIn:
            this->method = bounceEaseIn;
            break;
        case TweenTypeBounceEaseOut:
            this->method = bounceEaseOut;
            break;
        case TweenTypeBounceEaseInOut:
            this->method = bounceEaseInOut;
            break;
            
        case TweenTypeRegularEaseIn:
            this->method = regularEaseIn;
            break;
        case TweenTypeRegularEaseOut:
            this->method = regularEaseOut;
            break;
        case TweenTypeRegularEaseInOut:
            this->method = regularEaseInOut;
            break;
            
        case TweenTypeStrongEaseIn:
            this->method = strongEaseIn;
            break;
        case TweenTypeStrongEaseOut:
            this->method = strongEaseOut;
            break;
        case TweenTypeStrongEaseInOut:
            this->method = strongEaseInOut;
            break;
            
        default:
            ERROR("Tween new %d invalid", type)
            this->method = liner;
            break;
    }
}
