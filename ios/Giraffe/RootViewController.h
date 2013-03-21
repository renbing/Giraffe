//
//  RootViewController.h
//  Giraffe
//
//  Created by kins on 12/9/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EAGLView.h"
#import "RenderContextGL2.h"

@interface RootViewController : UIViewController<ViewDidLayoutProtocol,UIGestureRecognizerDelegate> {

    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *m_displayLink;
	
	size_t m_frameCount;
	UILabel *m_fpsLabel;
	UITextView *m_console;
    
    EAGLView *m_glView;
    RenderContextGL2 *m_glRenderContext;
}

@property (atomic, readonly) UIInterfaceOrientation orientation;

- (void)startAnimation;
- (void)stopAnimation;
- (void)drawFrame;

- (void)updateFPS;

@end
