//
//  MovieClip.cpp
//  Giraffe
//
//  Created by kins on 11/21/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#include <typeinfo>
#include <algorithm>
#include "MovieClip.h"
#include "RenderContextGL2.h"

JS_PROPERTY(DisplayObject, STRING, name)
JS_PROPERTY(DisplayObject, BOOL, visible)
JS_PROPERTY(DisplayObject, NUMBER, alpha)
JS_PROPERTY(DisplayObject, BOOL, touchable)

JS_PROPERTY(DisplayObject, NUMBER, x)
JS_PROPERTY(DisplayObject, NUMBER, y)
JS_PROPERTY(DisplayObject, NUMBER, width)
JS_PROPERTY(DisplayObject, NUMBER, height)
JS_PROPERTY(DisplayObject, NUMBER, scaleX)
JS_PROPERTY(DisplayObject, NUMBER, scaleY)
JS_PROPERTY(DisplayObject, NUMBER, rotation)

JS_PROPERTY(Bitmap, NUMBER, anchorX)
JS_PROPERTY(Bitmap, NUMBER, anchorY)


JSPropertySpec Bitmap::js_ps[] = {
    JS_PROPERTY_DEF(DisplayObject, name),
    JS_PROPERTY_DEF(DisplayObject, visible),
    JS_PROPERTY_DEF(DisplayObject, alpha),
    JS_PROPERTY_DEF(DisplayObject, touchable),
    JS_PROPERTY_DEF(DisplayObject, x),
    JS_PROPERTY_DEF(DisplayObject, y),
    JS_PROPERTY_DEF(DisplayObject, width),
    JS_PROPERTY_DEF(DisplayObject, height),
    JS_PROPERTY_DEF(DisplayObject, scaleX),
    JS_PROPERTY_DEF(DisplayObject, scaleY),
    JS_PROPERTY_DEF(DisplayObject, rotation),
    JS_PROPERTY_DEF(Bitmap, anchorX),
    JS_PROPERTY_DEF(Bitmap, anchorY),
    JS_PS_END
};

JS_SIMPLE_FUNC(DisplayObject, render, VOID)
JS_FUNC_WITH_ARG4(DisplayObject, setClipRect, VOID, NUMBER, NUMBER, NUMBER, NUMBER)
JS_SIMPLE_FUNC(DisplayObject, unsetClipRect, VOID)
JS_FUNC_WITH_ARG1(DisplayObject, removeEventListener, VOID, INT)
JS_FUNC_WITH_ARG2(DisplayObject, addEventListener, VOID, INT, JSVAL)
JS_SIMPLE_FUNC(DisplayObject, removeAllEventListener, VOID)

JSFunctionSpec Bitmap::js_fs[] = {
    JS_FN_DEF(DisplayObject, render, 0),
    JS_FN_DEF(DisplayObject, setClipRect, 4),
    JS_FN_DEF(DisplayObject, unsetClipRect, 0),
    JS_FN_DEF(DisplayObject, removeEventListener, 1),
    JS_FN_DEF(DisplayObject, addEventListener, 2),
    JS_FN_DEF(DisplayObject, removeAllEventListener, 0),
    JS_FS_END
};

JSPropertySpec Bitmap::js_sps[] = {
    JS_PS_END
};
JSFunctionSpec Bitmap::js_sfs[] = {
    JS_FS_END
};

JS_CLASS_EXPORT(Bitmap)

static JSBool js_func_MovieClip_addChild(JSContext *cx, unsigned argc, jsval *vp) {
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
    MovieClip *container = static_cast<MovieClip *>(JS_GetPrivate(obj));
    if( container == NULL ) return JS_FALSE;
    JSObject *child = JS_ARGV(cx, vp)->toObjectOrNull();
    container->addChild((DisplayObject *)JS_GetPrivate(child));

    JS_SET_RVAL(cx, vp, JS::UndefinedValue());

    return JS_TRUE;
}

static JSBool js_func_MovieClip_addChildAt(JSContext *cx, unsigned argc, jsval *vp) {
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
    MovieClip *container = static_cast<MovieClip *>(JS_GetPrivate(obj));
    if( container == NULL ) return JS_FALSE;
    JSObject *child = JS_ARGV(cx, vp)->toObjectOrNull();
    unsigned int index = JSVAL_TO_INT(*(JS_ARGV(cx, vp)+1));
    container->addChildAt((DisplayObject *)JS_GetPrivate(child), index);

    JS_SET_RVAL(cx, vp, JS::UndefinedValue());

    return JS_TRUE;
}

JS_PROPERTY(MovieClip, STRING, name)
JS_PROPERTY(MovieClip, BOOL, visible)
JS_PROPERTY(MovieClip, NUMBER, alpha)
JS_PROPERTY(MovieClip, BOOL, touchable)

JS_PROPERTY(MovieClip, NUMBER, x)
JS_PROPERTY(MovieClip, NUMBER, y)
JS_PROPERTY(MovieClip, NUMBER, width)
JS_PROPERTY(MovieClip, NUMBER, height)
JS_PROPERTY(MovieClip, NUMBER, scaleX)
JS_PROPERTY(MovieClip, NUMBER, scaleY)
JS_PROPERTY(MovieClip, NUMBER, rotation)


JSPropertySpec MovieClip::js_ps[] = {
    JS_PROPERTY_DEF(MovieClip, name),
    JS_PROPERTY_DEF(MovieClip, visible),
    JS_PROPERTY_DEF(MovieClip, alpha),
    JS_PROPERTY_DEF(MovieClip, touchable),
    JS_PROPERTY_DEF(MovieClip, x),
    JS_PROPERTY_DEF(MovieClip, y),
    JS_PROPERTY_DEF(MovieClip, width),
    JS_PROPERTY_DEF(MovieClip, height),
    JS_PROPERTY_DEF(MovieClip, scaleX),
    JS_PROPERTY_DEF(MovieClip, scaleY),
    JS_PROPERTY_DEF(MovieClip, rotation),
    JS_PS_END
};

JS_SIMPLE_FUNC(MovieClip, render, VOID)
JS_FUNC_WITH_ARG4(MovieClip, setClipRect, VOID, NUMBER, NUMBER, NUMBER, NUMBER)
JS_SIMPLE_FUNC(MovieClip, unsetClipRect, VOID)
JS_FUNC_WITH_ARG1(MovieClip, removeEventListener, VOID, INT)
JS_FUNC_WITH_ARG2(MovieClip, addEventListener, VOID, INT, JSVAL)
JS_SIMPLE_FUNC(MovieClip, removeAllEventListener, VOID)

JSFunctionSpec MovieClip::js_fs[] = {
    JS_FN_DEF(MovieClip, render, 0),
    JS_FN_DEF(MovieClip, setClipRect, 4),
    JS_FN_DEF(MovieClip, unsetClipRect, 0),
    JS_FN_DEF(MovieClip, removeEventListener, 1),
    JS_FN_DEF(MovieClip, addEventListener, 2),
    JS_FN_DEF(MovieClip, removeAllEventListener, 0),
    JS_FN_DEF(MovieClip, addChild, 1),
    JS_FN_DEF(MovieClip, addChildAt, 2),
    JS_FS_END
};


JSPropertySpec MovieClip::js_sps[] = {
    JS_PS_END
};
JSFunctionSpec MovieClip::js_sfs[] = {
    JS_FS_END
};

JS_CLASS_EXPORT(MovieClip)


MovieClip *MovieClip::stage = new MovieClip("stage", 1);
EventManager *EventManager::m_instance = NULL;

DisplayObject::DisplayObject()
{
    visible = true;
    alpha = 1.0;
    touchable = true;

    x = 0;
    y = 0;
    width = 0;
    height = 0;
    scaleX = 1;
    scaleY = 1;
    rotation = 0;

    parent = NULL;
    clipRect = NULL;
}

DisplayObject::~DisplayObject() {
    if( clipRect != NULL ) {
        delete clipRect;
        clipRect = NULL;
    }
}

void DisplayObject::setClipRect(float x, float y, float w, float h)
{
    if( clipRect == NULL ) {
        clipRect = new Rect4f;
    }

    clipRect->x = x;
    clipRect->y = y;
    clipRect->w = w;
    clipRect->h = h;
}

void DisplayObject::unsetClipRect()
{
    if( clipRect != NULL ) {
        delete clipRect;
        clipRect = NULL;
    }
}

DisplayObject * DisplayObject::hitTest(float x, float y)
{
    
    if( !touchable || !visible || scaleX == 0 || scaleY == 0 || alpha == 0 ) {
        return NULL;
    }
    
    esMatrix invertMatrix;
    esMatrixInvert(&invertMatrix, &matrix);
    esPoint src = {x, y};
    esPoint dst = esTransformPoint(&src, &invertMatrix);
    
    if( dst.x >= 0 && dst.x <= width && dst.y >= 0 && dst.y <= height ) {
        return this;
    }
    
    return NULL;
}

Bitmap::Bitmap(Image *image, const string &name, float sx, float sy, float sw, float sh, float width, float height)
{
    m_image = image;
    this->width = width;
    this->height = height;
    this->name = name;
    this->anchorX = 0;
    this->anchorY = 0;
    
    glGenBuffers(1, m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    GLfloat vertices[16] = {
        // vertices
        0, 0, 0, height, width, height, width, 0,
        // texture
        sx/image->POTWidth, sy/image->POTHeight, sx/image->POTWidth, (sy+sh)/image->POTHeight,
        (sx+sw)/image->POTWidth, (sy+sh)/image->POTHeight, (sx+sw)/image->POTWidth, sy/image->POTHeight
    };
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
}

Bitmap::Bitmap(Image *image, const string &name)
{
    new (this)Bitmap(image, name, 0, 0, image->width, image->height, image->width, image->height);
}

Bitmap::~Bitmap()
{
    glDeleteBuffers(1, m_vbo);
}

void Bitmap::render()
{
    if( !visible || scaleX == 0 || scaleY == 0 || alpha == 0 ) {
        return;
    }
    
    RenderContextGL2 *glRender = RenderContextGL2::getInstance();
    
    glRender->save();
    glRender->translate(x, y);
    
    if( anchorX > 0 || anchorY > 0 ) {
        glRender->translate(anchorX * width, anchorY * height);
    }
    
    if( scaleX != 1 || scaleY != 1 ) {
        glRender->scale(scaleX, scaleY);
    }
    
    if( rotation != 0 ) {
        glRender->rotate(rotation);
    }
    
    if( anchorX > 0 || anchorY > 0 ) {
        glRender->translate(-anchorX * width, -anchorY * height);
    }
    
    if( alpha != 1.0 ) {
        glRender->changeAlpha(alpha);
    }
    
    if( clipRect ) {
        glRender->scissor(clipRect);
    }
    
    esMatrixCopy(&matrix, glRender->tsrMatrix());
    glRender->drawRect(m_image, m_vbo[0]);
    
    if( clipRect ) {
        glRender->unscissor();
    }
    
    glRender->restore();
}


void MovieClip::addChild(DisplayObject *obj)
{
    if( !obj ) return;
    if( obj->parent ) {
        obj->parent->removeChild(obj);
    }
   
    getChildren().push_back(obj);
    obj->parent = this;
}

void MovieClip::addChildAt(DisplayObject *obj, unsigned int frameIndex)
{
    if( !obj ) return;
    if( obj->parent ) {
        obj->parent->removeChild(obj);
    }

    children->insert(children->begin() + frameIndex, obj);
    obj->parent = this;
}

DisplayObject * MovieClip::getChildByName(const string &name)
{
    for( int i=0,max=children->size(); i<max; i++ ) {
        DisplayObject *child = children->at(i);
        if( typeid(*child) == typeid(MovieClip) && ((MovieClip *)child)->name == name ) {
            return child;
        }
    }

    return NULL;
}

DisplayObject * MovieClip::getChildAt(unsigned int index)
{
    if( index >= children->size() ) {
        return NULL;
    }

    return children->at(index);
}

void MovieClip::removeChildByName(const string &name)
{
    for( int i=0,max=children->size(); i<max; i++ ) {
        DisplayObject *child = children->at(i);
        if( typeid(*child) == typeid(MovieClip) && ((MovieClip *)child)->name == name ) {
            children->erase(children->begin() + i);
            return;
        }
    }
}

void MovieClip::removeChild(DisplayObject *obj)
{
    if( !obj ) return;

    vector<DisplayObject *>::iterator it = find(children->begin(), children->end(), obj);
    if( it != children->end() ) {
        children->erase(it);
    }
}

void MovieClip::removeChildAt(unsigned int index)
{
    if( index >= children->size() ) return;

    children->erase(children->begin() + index);
}

void MovieClip::removeAllChild()
{
    children->clear();
}

void MovieClip::removeFromParent()
{
    if( !parent ) return;

    parent->removeChild(this);
}

void DisplayObject::bubbleEvent(Event *e)
{
    map<EventType, EventCallback>::iterator it = eventBubbleCallBack.find(e->type);
    if( it != eventBubbleCallBack.end() && it->second ) {
        it->second(this, e);
        
        // Drag/Swip事件停止冒泡
        if( e->type == EventTypeDrag || e->type == EventTypeDragEnd ||
           e->type == EventTypeDragStart || e->type == EventTypeSwipe ) {
            return;
        }
    }
    
    if( parent ) {
        parent->bubbleEvent(e);
    }
}

void MovieClip::render()
{
    if( !visible || scaleX == 0 || scaleY == 0 || alpha == 0 || children->empty()) {
        return;
    }
    
    RenderContextGL2 *glRender = RenderContextGL2::getInstance();
    
    glRender->save();
    glRender->translate(x, y);
    
    if( scaleX != 1 || scaleY != 1 ) {
        glRender->scale(scaleX, scaleY);
    }
    
    if( rotation != 0 ) {
        glRender->rotate(rotation);
    }
    
    if( alpha != 1.0 ) {
        glRender->changeAlpha(alpha);
    }

    // 处理ENTER_FRAME事件
    map<EventType, EventCallback>::iterator it = eventBubbleCallBack.find(EventTypeEnterFrame);
    if( it != eventBubbleCallBack.end() && it->second) {
        it->second(this, NULL);
    }
    
    if( clipRect ) {
        glRender->scissor(clipRect);
    }
    for( int i=0,max=children->size(); i<max; i++ ) {
        DisplayObject *child = children->at(i);
        if( !child ) continue;
        
        child->render();
    }
    
    if( clipRect ){
        glRender->unscissor();
    }
    glRender->restore();
    
    nextFrame();
}


DisplayObject * MovieClip::hitTest(float x, float y)
{
    if( !touchable || !visible || scaleX == 0 || scaleY == 0 || alpha == 0 ) {
        return NULL;
    }
    
    for( int i=0; i<children->size(); i++ ) {
        DisplayObject *hited = children->at(i)->hitTest(x, y);
        if( hited ) {
            return hited;
        }
    }
    
    return NULL;
}


MovieClip::MovieClip(const string & name, const unsigned int frameCounts)
    : totalFrames(frameCounts)
{
    this->name = name;
    
    if( totalFrames == 0 ) {
        totalFrames = 1;
    }
    
    currentFrame = 1;
    isStoped = false;
        
    for( int i=0; i<=totalFrames; i++ ) {
        frames.push_back(vector<DisplayObject*>());
    };
    
    children = &frames[currentFrame];
}

void MovieClip::stopAtHead(bool isRecursive)
{
    gotoAndStop(1);
    if( !isRecursive ) return;

    for( int i=0,max=children->size(); i<max; i++ ) {
        DisplayObject *child = children->at(i);
        if( typeid(*child) == typeid(MovieClip) ) {
            ((MovieClip *)child)->stopAtHead(isRecursive);
        }
    }
}

void EventManager::onTouch(float x, float y)
{
    m_hited = MovieClip::stage->hitTest(x, y);
    if( m_hited ) {
        m_hitedPoint = esPointMake(x, y);
    } else {
        m_hitedPoint = esPointMake(0, 0);
    }
}

void EventManager::onTap()
{
    if( !m_hited ) return;
    
    Event e(EventTypeTap, m_hitedPoint);
    
    m_hited->bubbleEvent(&e);
}

void EventManager::onDrag(float dx, float dy)
{
    if( !m_hited ) return;
    
    Event e(EventTypeDrag, m_hitedPoint);
    e.move = esPointMake(dx, dy);
    
    m_hited->bubbleEvent(&e);
}

void EventManager::onDragEnd()
{
    if( !m_hited ) return;
    
    Event e(EventTypeDragEnd, m_hitedPoint);
    
    m_hited->bubbleEvent(&e);
}

void EventManager::onSwip(SwipDirection direction)
{
    if( !m_hited ) return;
    
    Event e(EventTypeSwipe, m_hitedPoint);
    e.direction = direction;
    
    m_hited->bubbleEvent(&e);
}
