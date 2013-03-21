//
//  EAGLView.h
//  Giraffe
//
//  Created by 思诚 李 on 2/27/12.
//  Copyright (c) 2012 Perfect World. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "global.h"

@protocol ViewDidLayoutProtocol <NSObject>

- (void)resizeGLBuffer;

@end

@interface EAGLView : UIView {
    GLint frameBufferWidth_;
    GLint frameBufferHeight_;
    
    GLuint frameBuffer_;
    GLuint colorBuffer_;
    
    EAGLContext *context_;
    
    id<ViewDidLayoutProtocol> layoutViewTarget_;
}

@property (nonatomic) int drawableWidth;
@property (nonatomic) int drawableHeight;
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) id layoutViewTarget;

+ (Class)layerClass;

- (BOOL)setFrameBuffer;
- (BOOL)presentFrameBuffer;

@end
