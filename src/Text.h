//
//  Text.h
//  Giraffe
//
//  Created by kins on 2/22/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#ifndef __Giraffe__Text__
#define __Giraffe__Text__

#include "Texture.h"

class Text : public Texture
{
public:
    string  text;
    string  color;
    string  align;
    string  font;
    
private:
    
public:
    Text();
    
    void render();
    
    JS_CLASS_EXPORT_DEF
    
};

#endif