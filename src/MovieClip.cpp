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

#define MY_JSVAL_TO_DisplayObject(cx, v)   static_cast<DisplayObject *>(JS_GetPrivate(JSVAL_TO_OBJECT(v)))
static jsval MY_DisplayObject_TO_JSVAL(JSContext *cx, DisplayObject *cobj) {
    if( cobj ) {
        return OBJECT_TO_JSVAL(cobj->jsthis);
    }
    
    return JS::NullValue();
}

static jsval MY_DisplayObjectArray_TO_JSVAL(JSContext *cx, vector<DisplayObject *> cobjs) {
    JSObject *array = JS_NewArrayObject(cx, 0, NULL);
    
    int j=0;
    for( int i=0,max=cobjs.size(); i<max; i++) {
        DisplayObject *cobj = cobjs.at(i);
        if( cobj ) {
            jsval vp = OBJECT_TO_JSVAL(cobj->jsthis);
            JS_SetElement(cx, array, j++, &vp);
        }
    }
    
    return OBJECT_TO_JSVAL(array);
}

#define MY_JSVAL_TO_Texture(cx, v)   static_cast<Texture *>(JS_GetPrivate(JSVAL_TO_OBJECT(v)))
static jsval MY_Texture_TO_JSVAL(JSContext *cx, Texture *cobj) {
    if( cobj ) {
        return OBJECT_TO_JSVAL(cobj->jsthis);
    }
    
    return JS::NullValue();
}

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

JS_SIMPLE_FUNC(DisplayObject, render, VOID)
JS_FUNC_WITH_ARG4(DisplayObject, setClipRect, VOID, NUMBER, NUMBER, NUMBER, NUMBER)
JS_SIMPLE_FUNC(DisplayObject, unsetClipRect, VOID)
JS_FUNC_WITH_ARG1(DisplayObject, removeEventListener, VOID, INT)
JS_FUNC_WITH_ARG2(DisplayObject, addEventListener, VOID, INT, OBJECT)
JS_SIMPLE_FUNC(DisplayObject, removeAllEventListener, VOID)

JS_PROPERTY(Bitmap, NUMBER, anchorX)
JS_PROPERTY(Bitmap, NUMBER, anchorY)

JS_PROPERTY_READONLY_FUNC(Bitmap, Texture, texture)

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
    JS_PROPERTY_READONLY_DEF(Bitmap, texture),
    JS_PS_END
};

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

template<>
Bitmap * js_native_constructor<Bitmap>(JSContext *cx, unsigned argc, jsval *vp) {
    
    jsval *argv = JS_ARGV(cx, vp);
    Bitmap *cobj = NULL;
    Texture *texture = NULL;
    if( argc == 1 ) {
        texture = static_cast<Texture *>(JS_GetPrivate(JSVAL_TO_OBJECT(argv[0])));
        cobj = new Bitmap(texture);
    }else if( argc == 2 ) {
        texture = static_cast<Texture *>(JS_GetPrivate(JSVAL_TO_OBJECT(argv[0])));
        cobj = new Bitmap(texture, MY_JSVAL_TO_STRING(cx, argv[1]));
    }else if( argc == 6 ) {
        texture = static_cast<Texture *>(JS_GetPrivate(JSVAL_TO_OBJECT(argv[0])));
        cobj = new Bitmap(texture, MY_JSVAL_TO_STRING(cx, argv[1]), MY_JSVAL_TO_NUMBER(cx, argv[2]),
                    MY_JSVAL_TO_NUMBER(cx, argv[3]), MY_JSVAL_TO_NUMBER(cx, argv[4]), MY_JSVAL_TO_NUMBER(cx, argv[5]),
                    MY_JSVAL_TO_NUMBER(cx, argv[4]), MY_JSVAL_TO_NUMBER(cx, argv[5]));
    }else if( argc == 8 ) {
        texture = static_cast<Texture *>(JS_GetPrivate(JSVAL_TO_OBJECT(argv[0])));
        cobj = new Bitmap(texture, MY_JSVAL_TO_STRING(cx, argv[1]), MY_JSVAL_TO_NUMBER(cx, argv[2]),
                    MY_JSVAL_TO_NUMBER(cx, argv[3]), MY_JSVAL_TO_NUMBER(cx, argv[4]), MY_JSVAL_TO_NUMBER(cx, argv[5]),
                    MY_JSVAL_TO_NUMBER(cx, argv[6]), MY_JSVAL_TO_NUMBER(cx, argv[7]));
    }else {
        return NULL;
    }
    
    return cobj;
}

template<>
void js_native_constructor_end<Bitmap>(JSContext *cx, Bitmap *obj) {
    JSObject *slot = JSVAL_TO_OBJECT(JS_GetReservedSlot(obj->jsthis, 0));
    jsval v = OBJECT_TO_JSVAL(obj->getTexture()->jsthis);
    JS_SetProperty(cx, slot, "texture", &v);
}

JS_CLASS_EXPORT(Bitmap)

/*
static JSBool js_func_MovieClip_getChildByName(JSContext *cx, unsigned argc, jsval *vp) {
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
    MovieClip *container = static_cast<MovieClip *>(JS_GetPrivate(obj));
    if( container == NULL ) return JS_FALSE;
    string name = MY_JSVAL_TO_STRING(cx, *(JS_ARGV(cx, vp)));
    DisplayObject *child = container->getChildByName(name);
    if( !child ) {
        JS_SET_RVAL(cx, vp, JS::UndefinedValue());
    }else {
        if( typeid(child) == typeid(MovieClip) ) {
            JS_SET_RVAL(cx, vp, COBJECT_TO_JSVAL<MovieClip>(cx, dynamic_cast<MovieClip *>(child)));
        }else if( typeid(child) == typeid(Bitmap)) {
            JS_SET_RVAL(cx, vp,COBJECT_TO_JSVAL<Bitmap>(cx, dynamic_cast<Bitmap *>(child)));
        }
    }
    
    return JS_TRUE;
}

static JSBool js_func_MovieClip_getChildAt(JSContext *cx, unsigned argc, jsval *vp) {
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
    MovieClip *container = static_cast<MovieClip *>(JS_GetPrivate(obj));
    if( container == NULL ) return JS_FALSE;
    JSObject *child = JS_ARGV(cx, vp)->toObjectOrNull();
    unsigned int index = JSVAL_TO_INT(*(JS_ARGV(cx, vp)+1));
    container->addChildAt((DisplayObject *)JS_GetPrivate(child), index);

    JS_SET_RVAL(cx, vp, JS::UndefinedValue());

    return JS_TRUE;
}
*/

JS_FUNC_WITH_ARG1(MovieClip, addChild, VOID, DisplayObject)
JS_FUNC_WITH_ARG2(MovieClip, addChildAt, VOID, DisplayObject, INT)
JS_FUNC_WITH_ARG1(MovieClip, removeChildByName, VOID, STRING)
JS_FUNC_WITH_ARG1(MovieClip, removeChild, VOID, DisplayObject)
JS_FUNC_WITH_ARG1(MovieClip, removeChildAt, VOID, INT)
JS_SIMPLE_FUNC(MovieClip, removeAllChild, VOID)
JS_SIMPLE_FUNC(MovieClip, removeFromParent, VOID)
JS_SIMPLE_FUNC(MovieClip, stop, VOID)
JS_SIMPLE_FUNC(MovieClip, play, VOID)
JS_FUNC_WITH_ARG1(MovieClip, gotoAndStop, VOID, INT)
JS_FUNC_WITH_ARG1(MovieClip, gotoAndPlay, VOID, INT)
JS_SIMPLE_FUNC(MovieClip, nextFrame, VOID)
JS_SIMPLE_FUNC(MovieClip, prevFrame, VOID)
JS_FUNC_WITH_ARG1(MovieClip, getChildByName, DisplayObject, STRING)
JS_FUNC_WITH_ARG1(MovieClip, getChildAt, DisplayObject, INT)
JS_SIMPLE_FUNC(MovieClip, getChildren, DisplayObjectArray)

JSPropertySpec MovieClip::js_ps[] = {
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
    JS_PS_END
};

JSFunctionSpec MovieClip::js_fs[] = {
    JS_FN_DEF(DisplayObject, render, 0),
    JS_FN_DEF(DisplayObject, setClipRect, 4),
    JS_FN_DEF(DisplayObject, unsetClipRect, 0),
    JS_FN_DEF(DisplayObject, removeEventListener, 1),
    JS_FN_DEF(DisplayObject, addEventListener, 2),
    JS_FN_DEF(DisplayObject, removeAllEventListener, 0),
    JS_FN_DEF(MovieClip, addChild, 1),
    JS_FN_DEF(MovieClip, addChildAt, 2),
    JS_FN_DEF(MovieClip, removeChildByName, 1),
    JS_FN_DEF(MovieClip, removeChild, 1),
    JS_FN_DEF(MovieClip, removeChildAt, 1),
    JS_FN_DEF(MovieClip, removeAllChild, 0),
    JS_FN_DEF(MovieClip, removeFromParent, 0),
    JS_FN_DEF(MovieClip, stop, 0),
    JS_FN_DEF(MovieClip, play, 0),
    JS_FN_DEF(MovieClip, gotoAndStop, 1),
    JS_FN_DEF(MovieClip, gotoAndPlay, 1),
    JS_FN_DEF(MovieClip, nextFrame, 0),
    JS_FN_DEF(MovieClip, prevFrame, 0),
    JS_FN_DEF(MovieClip, getChildByName, 1),
    JS_FN_DEF(MovieClip, getChildAt, 1),
    JS_FN_DEF(MovieClip, getChildren, 0),
    JS_FS_END
};


JSPropertySpec MovieClip::js_sps[] = {
    JS_PS_END
};
JSFunctionSpec MovieClip::js_sfs[] = {
    JS_FS_END
};

template<>
MovieClip * js_native_constructor<MovieClip>(JSContext *cx, unsigned argc, jsval *vp) {
    
    jsval *argv = JS_ARGV(cx, vp);
    MovieClip *cobj = NULL;
    if( argc == 1 ) {
        cobj = new MovieClip(MY_JSVAL_TO_STRING(cx, argv[0]));
    }else if( argc == 2 ) {
        cobj = new MovieClip(MY_JSVAL_TO_STRING(cx, argv[0]), MY_JSVAL_TO_INT(cx, argv[1]));
    }else{
        cobj = new MovieClip();
    }
    
    return cobj;
}

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

Bitmap::Bitmap(Texture *texture, const string &name, float sx, float sy, float sw, float sh, float width, float height)
{
    m_texture = texture;
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
        sx/texture->POTWidth, sy/texture->POTHeight, sx/texture->POTWidth, (sy+sh)/texture->POTHeight,
        (sx+sw)/texture->POTWidth, (sy+sh)/texture->POTHeight, (sx+sw)/texture->POTWidth, sy/texture->POTHeight
    };
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
}

Bitmap::Bitmap(Texture *texture, const string &name)
{
    new (this)Bitmap(texture, name, 0, 0, texture->width, texture->height, texture->width, texture->height);
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
    glRender->drawRect(m_texture, m_vbo[0]);
    
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
    
    JS_AddObjectRoot(JSCContext::getInstance()->jsContext(), &obj->jsthis);
}

void MovieClip::addChildAt(DisplayObject *obj, unsigned int frameIndex)
{
    if( !obj ) return;
    if( obj->parent ) {
        obj->parent->removeChild(obj);
    }

    children->insert(children->begin() + frameIndex, obj);
    obj->parent = this;
    
    JS_AddObjectRoot(JSCContext::getInstance()->jsContext(), &obj->jsthis);
}

DisplayObject * MovieClip::getChildByName(const string &name)
{
    for( int i=0,max=children->size(); i<max; i++ ) {
        DisplayObject *child = children->at(i);
        if( child->name == name ) {
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
    for( int i=children->size()-1; i>=0; i-- ) {
        DisplayObject *child = children->at(i);
        if( child->name == name ) {
            children->erase(children->begin() + i);
            JS_RemoveObjectRoot(JSCContext::getInstance()->jsContext(), &(child->jsthis));
        }
    }
}

void MovieClip::removeChild(DisplayObject *obj)
{
    if( !obj ) return;

    vector<DisplayObject *>::iterator it = find(children->begin(), children->end(), obj);
    if( it != children->end() ) {
        children->erase(it);
        JS_RemoveObjectRoot(JSCContext::getInstance()->jsContext(), &(*it)->jsthis);
    }
}

void MovieClip::removeChildAt(unsigned int index)
{
    if( index >= children->size() ) return;

    children->erase(children->begin() + index);
    JS_RemoveObjectRoot(JSCContext::getInstance()->jsContext(), &(children->at(index)->jsthis));
}

void MovieClip::removeAllChild()
{
    for( int i=0, max=children->size(); i<max; i++ ) {
        DisplayObject *child = children->at(i);
        JS_RemoveObjectRoot(JSCContext::getInstance()->jsContext(), &(child->jsthis));
    }
    children->clear();
}

void MovieClip::removeFromParent()
{
    if( !parent ) return;

    parent->removeChild(this);
}

void DisplayObject::bubbleEvent(Event *e)
{
    map<int, JSObject *>::iterator it = eventBubbleCallBack.find(e->type);
    if( it != eventBubbleCallBack.end() ) {
        jsval events[1] = {JSCContext::getInstance()->createEvent(e)};
        JSCContext::getInstance()->callJSFunction(it->second, 1, events, this->jsthis);
        
        // 停止冒泡
        return;
        
        // Drag/Swip事件停止冒泡
        if( e->type == EventTypeDrag || e->type == EventTypeDragEnd ||
           e->type == EventTypeDragStart || e->type == EventTypeSwipe ) {
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
    map<int, JSObject *>::iterator it = eventBubbleCallBack.find(EventTypeEnterFrame);
    if( it != eventBubbleCallBack.end() ) {
        JSCContext::getInstance()->callJSFunction(it->second);
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
    
    if( !isStoped ) {
        nextFrame();
    }
}


DisplayObject * MovieClip::hitTest(float x, float y)
{
    if( !touchable || !visible || scaleX == 0 || scaleY == 0 || alpha == 0 ) {
        return NULL;
    }
    
    for( int i=children->size()-1; i>=0; i-- ) {
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
    isStoped = true;
        
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

void EventManager::onSwipe(SwipeDirection direction)
{
    if( !m_hited ) return;
    
    Event e(EventTypeSwipe, m_hitedPoint);
    e.direction = direction;
    
    m_hited->bubbleEvent(&e);
}

void EventManager::onPinch(float scale)
{
    JSCContext::getInstance()->onPinch(scale);
}

void EventManager::onLongPress()
{
    if( !m_hited ) return;
    
    Event e(EventTypeLongPress, m_hitedPoint);
    
    m_hited->bubbleEvent(&e);
}
