//
//  Image.h
//  Giraffe
//
//  Created by kins on 12/9/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#ifndef __Giraffe__Image__
#define __Giraffe__Image__

#include <string>
#include "global.h"
#include "JSCContext.h"

using std::string;

typedef struct
{
    float r,g,b,a;
}Color4f;

typedef struct
{
    float x,y,w,h;
}Rect4f;

class Image
{
public:
    unsigned long width;
    unsigned long height;
    unsigned long POTWidth;
    unsigned long POTHeight;
    bool hasAlpha;
    bool premultiAlpha;

private:
    GLuint m_texture;
    string m_src;

public:
    JSObject *jsthis;
    
    JSObject *onload;

public:
    Image();
    ~Image();
    JS_CLASS_EXPORT_DEF
    
    void set_src(const string &src);
    const string & get_src() const { return m_src; };
    GLuint getTexture() const { return m_texture; }
    
    void createTexture(const void *input, unsigned int size);
    
private:
    void setupTexture(const void *data);
};

#endif /* defined(__Giraffe__Image__) */
