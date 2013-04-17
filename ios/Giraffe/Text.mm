//
//  Text.m
//  Giraffe
//
//  Created by kins on 2/22/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "Text.h"
#import "stringutil.h"

void Text::render()
{
    POTWidth = computePOT(width);
    POTHeight = computePOT(height);
    
    int fontSize = atoi(font.c_str());
    int systemFontSize = (int)[UIFont systemFontSize];
    if( fontSize < systemFontSize ) {
        fontSize = systemFontSize;
    }
    UIFont *uifont = [UIFont systemFontOfSize:fontSize];
    
    UITextAlignment uialign = UITextAlignmentLeft;
    if( StringUtil::lower(align) == "center") {
        uialign = UITextAlignmentCenter;
    }
    else if( StringUtil::lower(align) == "right" ) {
        uialign = UITextAlignmentRight;
    }
    
    unsigned int uicolor;
    StringUtil::convertHTMLColor(color, uicolor);
    
    void *bitmapData = (GLubyte *)malloc( POTWidth * POTHeight * 4 );
    CGContextRef cgContext = CGBitmapContextCreate( bitmapData, POTWidth, POTHeight, 8, 4 * POTWidth,
            CGColorSpaceCreateDeviceRGB(),  kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    
    //调整坐标系 右上->右下
    CGContextTranslateCTM(cgContext, 0, POTHeight);
    CGContextScaleCTM(cgContext, 1.0, -1.0);
    CGContextSetShouldAntialias(cgContext, true);
    
    // 设置颜色
    CGContextSetRGBFillColor(cgContext, ((uicolor >> 16) & 255)/255.0, ((uicolor >> 8) & 255)/255.0,
                             (uicolor & 255)/255.0, ((uicolor >> 24) & 255)/255.0);
    
    UIGraphicsPushContext(cgContext);
    [[NSString stringWithUTF8String:text.c_str()] drawInRect:CGRectMake(0, 0, width, height) withFont:
        uifont lineBreakMode:UILineBreakModeWordWrap alignment:uialign];
    UIGraphicsPopContext();
    
    if( m_texture <= 0 ) {
        glGenTextures(1, &m_texture);
    }
    
    glBindTexture(GL_TEXTURE_2D, m_texture);
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, POTWidth, POTHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmapData);
}
