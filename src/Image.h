//
//  Image.h
//  Giraffe
//
//  Created by kins on 12/9/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#ifndef __Giraffe__Image__
#define __Giraffe__Image__

#include "Texture.h"

class Image : public Texture
{
private:
    string m_src;

public:
    JSObject *onload;

public:
    Image();
    JS_CLASS_EXPORT_DEF
    
    void set_src(const string &src);
    const string & get_src() const { return m_src; };

public:
    void createTexture(const void *input, unsigned int size);
    
    void setupTexture(const void *data);
};

#endif /* defined(__Giraffe__Image__) */
