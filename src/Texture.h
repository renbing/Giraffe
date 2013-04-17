//
//  Texture.h
//  Giraffe
//
//  Created by kins on 3/27/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#ifndef __Giraffe__Texture__
#define __Giraffe__Texture__

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

class Texture
{
public:
    unsigned long width;
    unsigned long height;
    unsigned long POTWidth;
    unsigned long POTHeight;
    bool hasAlpha;
    bool premultiAlpha;

    JSObject *jsthis;
    
protected:
    GLuint m_texture;
    
protected:
    Texture();
    ~Texture();
    
public:
    GLuint getTexture() const { return m_texture; }
};

#endif /* defined(__Giraffe__Texture__) */
