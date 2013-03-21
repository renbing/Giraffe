//
//  EAGLView.m
//  Giraffe
//
//  Created by 思诚 李 on 2/27/12.
//  Copyright (c) 2012 Perfect World. All rights reserved.
//

#import "EAGLView.h"
#import <QuartzCore/QuartzCore.h>

@interface EAGLView (Private)
- (BOOL)createFrameBuffer;
- (void)deleteFrameBuffer;
@end

@implementation EAGLView

@synthesize drawableWidth = frameBufferWidth_;
@synthesize drawableHeight = frameBufferHeight_;
@synthesize context = context_;
@synthesize layoutViewTarget = layoutViewTarget_;

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    [self deleteFrameBuffer];
    [self setFrameBuffer];
    
    [layoutViewTarget_ resizeGLBuffer];
}

- (void)dealloc
{
    [self deleteFrameBuffer];
    [EAGLContext setCurrentContext:nil];
    [context_ release];
    [super dealloc];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {
            eaglLayer.contentsScale = 1;
        }
        else {
            eaglLayer.contentsScale = [[UIScreen mainScreen] scale];
        }
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, 
                                        nil];
    }
    return self;
}

- (BOOL)setFrameBuffer
{
    BOOL ret = YES;
    if (context_ && [EAGLContext setCurrentContext:context_]) {
        if (!frameBuffer_) {
            ret = [self createFrameBuffer];
            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
        }
    }
    else {
        ret = NO;
    }
    return ret;
}

- (BOOL)presentFrameBuffer
{
    BOOL ret = NO;
    if (context_) {
        glBindFramebuffer(GL_RENDERBUFFER, colorBuffer_);
        ret = [context_ presentRenderbuffer:GL_RENDERBUFFER];
    }
    return ret;
}

- (BOOL)createFrameBuffer
{
    BOOL ret = YES;
    if ([EAGLContext currentContext] != context_) {
        ret = [EAGLContext setCurrentContext:context_];
        if (!ret) {
            return ret;
        }
    }
    glGenFramebuffers(1, &frameBuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
    
    glGenRenderbuffers(1, &colorBuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer_);
    
    [context_ renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer_);
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &frameBufferWidth_);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &frameBufferHeight_);
    LOG("createFrameBuffer with frameBufferWidth=%d, frameBufferHeight=%d\n", frameBufferWidth_, frameBufferHeight_);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        ERROR("createFrameBuffer doesn't complete!!!\n");
        return NO;
    }
    return ret;
}

- (void)deleteFrameBuffer
{
    if ([EAGLContext currentContext] != context_) {
        [EAGLContext setCurrentContext:context_];
    }
    if (frameBuffer_) {
        glDeleteFramebuffers(1, &frameBuffer_);
        frameBuffer_ = 0;
    }
    if (colorBuffer_) {
        glDeleteRenderbuffers(1, &colorBuffer_);
    }
}

@end
