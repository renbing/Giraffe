//
//  MovieClip.h
//  Giraffe
//
//  Created by kins on 11/21/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#ifndef __Giraffe__MovieClip__
#define __Giraffe__MovieClip__

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "global.h"
#include "JSCContext.h"
#include "MatrixUtil.h"
#include "Image.h"

using std::vector;
using std::string;
using std::map;

typedef enum {
    EventTypeMouseDown  = 1,
    EventTypeMouseMove  = 2,
    EventTypeMouseUp    = 3,
    EventTypeEnterFrame = 4,
    EventTypeTap        = 10,
    EventTypeSwipe      = 11,
    EventTypeDragStart  = 12,
    EventTypeDrag       = 13,
    EventTypeDragEnd    = 14
} EventType;

typedef enum {
    SwipDirectionLeft,
    SwipDirectionRight,
    SwipDirectionUp,
    SwipDirectionDown
} SwipDirection;

class Event
{
public:
    EventType       type;
    esPoint         point;
    esPoint         move;
    SwipDirection   direction;
    void            *data;
    
    Event(EventType type, esPoint &point) {
        this->type = type;
        this->point = point;
        this->data = NULL;
    }
};


class MovieClip;

class DisplayObject
{
protected:
    typedef void (*EventCallback) (DisplayObject *obj, const Event *e);
    map<EventType, EventCallback>       eventBubbleCallBack;
    
    Rect4f      *clipRect;
    esMatrix    matrix;

public:
    string      name;

    bool        visible;
    float       alpha;
    bool        touchable;

    float       x;
    float       y;
    float       width;
    float       height;
    float       scaleX;
    float       scaleY;
    float       rotation;
    
    MovieClip   *parent;
    
protected:
    DisplayObject();

public:
    virtual void render() = 0;
    virtual ~DisplayObject();

    
    void setClipRect(float x, float y, float w, float h);
    void unsetClipRect();
    virtual DisplayObject *hitTest(float x, float y);

    // 增加事件监听
    void addEventListener(EventType type, EventCallback callback) {
        eventBubbleCallBack[type] = callback;
    }

    // 卸载事件监听
    void removeEventListener(EventType type) {
        eventBubbleCallBack.erase(type);
    }
    
    // 卸载所有事件
    void removeAllEventListener() {
        eventBubbleCallBack.clear();
    }
    
    // 冒泡事件(从子节点到父节点,递归到舞台)
    void bubbleEvent(Event *e);
};

class Bitmap : public DisplayObject
{
public:
    float       anchorX;
    float       anchorY;
    
private:
    GLuint  m_vbo[1];
    Image   *m_image;
    
public:
    JS_CLASS_EXPORT_DEF
    
    Bitmap(Image *image, const string &name, float sx, float sy, float sw, float sh, float width, float height);
    Bitmap(Image *image, const string &name="");
    ~Bitmap();
    virtual void render();
};

class MovieClip : public DisplayObject
{
private:
    vector< vector<DisplayObject *> >   frames;
    vector<DisplayObject *>             *children;

    unsigned int    totalFrames;
    unsigned int    currentFrame;
    bool            isStoped;

public:
    static MovieClip *stage;

public:
    JS_CLASS_EXPORT_DEF
    
    MovieClip(const string &name="", const unsigned int frameCounts=1);

    virtual void render();
    virtual DisplayObject * hitTest(float x, float y);

    // 添加子节点(默认放到容器最尾端)
    void addChild(DisplayObject *obj);
    void addChildAt(DisplayObject *obj, unsigned int frameIndex);

    // 获取子节点
    vector<DisplayObject *> & getChildren() { return *children; }
    DisplayObject * getChildByName(const string &name);
    DisplayObject * getChildAt(unsigned int index);

    // 删除子节点(如果有多个重名的子节点,都会删除)
    void removeChildByName(const string &name);
    void removeChild(DisplayObject *obj);
    void removeChildAt(unsigned int index);
    void removeAllChild();
    void removeFromParent();
    
    // 播放/停止播放当前MovieClip,不影响子节点是否播放
    void stop(){ isStoped = true; }
    void play() { isStoped = false; }

    // 跳到指定帧,并停止/开始播放
    void gotoAndStop(unsigned int frame) { gotoFrame(currentFrame); stop(); }
    void gotoAndPlay(unsigned int frame) { gotoFrame(currentFrame); play(); }

    void nextFrame() { gotoFrame(currentFrame + 1); }
    void prevFrame() { gotoFrame(currentFrame - 1); }

    // 停在时间轴头部
    // @param isRecursive: 是否递归到子节点
    void stopAtHead(bool isRecursive=false);

    // 让MovieClip只播放一次,不循环
    void playOnce();

private:
    void gotoFrame(unsigned int frame) {
        if( frame < 1 ) {
            currentFrame = totalFrames;
        }else if( frame > totalFrames ) {
            currentFrame = 1;
        }else {
            currentFrame = frame;
        }
        children = &frames[currentFrame];
    }
};

class EventManager
{
private:
    static EventManager *m_instance;
    DisplayObject       *m_hited;
    esPoint             m_hitedPoint;

public:
    static EventManager *getInstance();
    
    void onTouch(float x, float y);
    void onTap();
    void onDrag(float dx, float dy);
    void onDragEnd();
    void onSwip(SwipDirection direction);
    
private:
    EventManager() {
        m_hited = NULL;
    }
};

inline EventManager * EventManager::getInstance()
{
    if( m_instance == NULL ) {
        m_instance = new EventManager();
    }
    
    return m_instance;
}

#endif /* defined(__Giraffe__MovieClip__) */
