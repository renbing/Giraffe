//
//  Image.cpp
//  Giraffe
//
//  Created by kins on 12/9/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#include "Image.h"

JS_PROPERTY_READONLY(Image, INT, width)
JS_PROPERTY_READONLY(Image, INT, height)

JS_PROPERTY_SLOT(Image, OBJECT, onload)
JS_PROPERTY_FUNC(Image, STRING, src)

JSPropertySpec Image::js_ps[] = {
    JS_PROPERTY_READONLY_DEF(Image, width),
    JS_PROPERTY_READONLY_DEF(Image, height),
    JS_PROPERTY_DEF(Image, onload),
    JS_PROPERTY_DEF(Image, src),
    JS_PS_END
};

JSFunctionSpec Image::js_fs[] = {
    JS_FS_END
};


JSPropertySpec Image::js_sps[] = {
    JS_PS_END
};
JSFunctionSpec Image::js_sfs[] = {
    JS_FS_END
};

JS_CLASS_EXPORT(Image)

Image::Image() : onload(NULL)
{
}

void Image::setupTexture(const void *data)
{
    // 创建Opengl ES Texture
    if( m_texture <= 0 ) {
        glGenTextures(1, &m_texture);
    }
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if( hasAlpha )
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, POTWidth, POTHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, POTWidth, POTHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
}

static void onImageLoad(const void *data, int size, void *arg) {
    if( !arg || !data || size <= 0 ) {
        return;
    }
    
    Image *image = static_cast<Image *>(arg);
    image->createTexture(data, size);
}

void Image::set_src(const string &src)
{
    m_src = src;
   
    JSCContext::getInstance()->loadFileAsync(src, onImageLoad, this);
}

