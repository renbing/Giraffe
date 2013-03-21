//
//  Director.cpp
//  Giraffe
//
//  Created by kins on 1/15/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#include "Director.h"
#include "Timer.h"
#include "Tween.h"

EventCallbackFunc static void glOnClicked(DisplayObject *obj, const Event *e) {
    TRACE("DisplayObject %s onClicked", obj->name.c_str())
}

EventCallbackFunc static void bitmapOnClicked(DisplayObject *obj, const Event *e) {
    TRACE("Bitmap %s onClicked", obj->name.c_str())
}

TimerCallbackFunc static void timerCallback(void *arg){
    TRACE("Timer triggered")
}

struct timeval Director::m_pasttv = timeval();

void Director::init()
{
    gettimeofday(&m_pasttv, NULL);
    
    // 测试代码
    Image *logo = new Image();
    logo->set_src("/cubetexture.png");
    Bitmap *bitmap = new Bitmap(logo, "Bitmap", 0, 0, logo->width, logo->height, 180, 180);
    bitmap->anchorX = 0.5;
    bitmap->anchorY = 0.5;
    bitmap->x = 200;
    bitmap->y = 200;
    //bitmap->setClipRect(0, 0, 100, 100);
    //bitmap->scaleX = -2.0;
    //bitmap->scaleY = 2.0;
    MovieClip::stage->addChild(bitmap);
    MovieClip::stage->addEventListener(EventTypeTap, glOnClicked);
    bitmap->addEventListener(EventTypeTap, bitmapOnClicked);
    
    //TimerManager::getInstance()->setInterval(timerCallback, 1);
    
    Tween::move(bitmap, TweenTypeBackEaseIn, 1, 400, 200).seqMove(bitmap, TweenTypeBackEaseIn, 1, 200, 200);
    Tween::scale(bitmap, TweenTypeBackEaseIn, 1, 1.2, 1.2).seqScale(bitmap, TweenTypeBackEaseIn, 1, 1, 1);
    Tween::rotate(bitmap, TweenTypeBackEaseIn, 1, 60).seqRotate(bitmap, TweenTypeBackEaseIn, 1, 0);
}

void Director::run()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    float passed = (now.tv_sec - m_pasttv.tv_sec) + (now.tv_usec - m_pasttv.tv_usec) / 1000000.0f;
    m_pasttv = now;
    
    TimerManager::getInstance()->update(passed);
    Tween::update(passed);
    
    MovieClip::stage->render();
}