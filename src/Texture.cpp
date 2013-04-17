//
//  Texture.cpp
//  Giraffe
//
//  Created by kins on 3/27/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#include "Texture.h"

Texture::Texture() : m_texture(0),hasAlpha(0),premultiAlpha(0),width(0),height(0),POTWidth(0),POTHeight(0)
{
    
}

Texture::~Texture()
{
    if( m_texture > 0 ) {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
}