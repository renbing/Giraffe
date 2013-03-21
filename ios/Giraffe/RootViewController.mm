//
//  RootViewController.m
//  Giraffe
//
//  Created by kins on 12/9/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import "RootViewController.h"
#import "MovieClip.h"

TimerCallbackFunc static void updateFPS(void *arg) {
    RootViewController * viewController = (RootViewController *)arg;
    [viewController updateFPS];
}

@implementation RootViewController

//@synthesize orientation = UIInterfaceOrientationLandscapeLeft;
@synthesize orientation = UIInterfaceOrientationPortrait;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        
        // landscape M_PI/2, default portrait
        CGRect frame = self.view.bounds;
        if( UIInterfaceOrientationIsLandscape(self.orientation)) {
            [self.view setTransform: CGAffineTransformMakeRotation(M_PI / 2)];
            frame = CGRectMake(frame.origin.x, frame.origin.y, frame.size.height, frame.size.width);
        }
        
        // Custom initialization
        EAGLContext *glContext = [EAGLContext currentContext];
        if( !glContext ) {
            glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        }
        
        [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        m_glView = [[EAGLView alloc] initWithFrame:frame];
        m_glView.context = glContext;
        m_glView.layoutViewTarget = self;
        [self.view addSubview: m_glView];
        
        // 坐标系变换(right,down)
        [m_glView setTransform:CGAffineTransformMakeScale(1, -1)];
        
        [m_glView setFrameBuffer];
        
        m_glView.multipleTouchEnabled = NO;
        
        m_glRenderContext = RenderContextGL2::getInstance();
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        LOG("MaxTextureSize supported=%d", maxTextureSize);
        
        // 显示Console调试
        m_console = [[UITextView alloc] initWithFrame:frame];
        m_console.font = [UIFont fontWithName:@"Arial" size:18];
        m_console.editable = false;
        m_console.userInteractionEnabled = true;
        m_console.scrollEnabled = true;
        m_console.showsVerticalScrollIndicator = true;
        m_console.text = @"\nConsole Log:\n";
        m_console.hidden = true;
        [self.view addSubview:m_console];
        
        // 显示FPS
        m_frameCount = 0;
        m_fpsLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 50, 25)];
        m_fpsLabel.userInteractionEnabled = true;
        //[m_fpsLabel setBackgroundColor:[UIColor clearColor]];
        [self.view addSubview:m_fpsLabel];
        
        // 手势识别
        UITapGestureRecognizer *fpsTapGesture = [[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(fpsTapDetected)] autorelease];
        [m_fpsLabel addGestureRecognizer:fpsTapGesture];
        
        UITapGestureRecognizer *tapRecognizer = [[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(glTapDetected:)] autorelease];
        [m_glView addGestureRecognizer:tapRecognizer];
        
        UISwipeGestureRecognizer *swipLeftRecognizer = [[[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(glSwipLeftDetected:)] autorelease];
        swipLeftRecognizer.direction = UISwipeGestureRecognizerDirectionLeft;
        swipLeftRecognizer.delegate = self;
        [m_glView addGestureRecognizer:swipLeftRecognizer];
        
        UISwipeGestureRecognizer *swipRightRecognizer = [[[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(glSwipRightDetected:)] autorelease];
        swipRightRecognizer.direction = UISwipeGestureRecognizerDirectionRight;
        swipRightRecognizer.delegate = self;
        [m_glView addGestureRecognizer:swipRightRecognizer];
        
        UISwipeGestureRecognizer *swipDownRecognizer = [[[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(glSwipDownDetected:)] autorelease];
        swipDownRecognizer.direction = UISwipeGestureRecognizerDirectionUp;
        swipDownRecognizer.delegate = self;
        [m_glView addGestureRecognizer:swipDownRecognizer];
        
        UISwipeGestureRecognizer *swipUpRecognizer = [[[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(glSwipUpDetected:)] autorelease];
        swipUpRecognizer.direction = UISwipeGestureRecognizerDirectionDown;
        swipUpRecognizer.delegate = self;
        [m_glView addGestureRecognizer:swipUpRecognizer];
        
        UIPanGestureRecognizer *panRecognizer = [[[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(glDragDetected:)] autorelease];
        panRecognizer.minimumNumberOfTouches = 1;
        panRecognizer.maximumNumberOfTouches = 1;
        panRecognizer.delegate = self;
        [m_glView addGestureRecognizer:panRecognizer];
        
        [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(updateFPS) userInfo:NULL repeats:YES];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)resizeGLBuffer
{
    LOG("resizeGLBuffer");
    m_glRenderContext->layout(m_glView.drawableWidth, m_glView.drawableHeight);
}

- (void)startAnimation
{
    if (!m_displayLink) {
        m_displayLink = [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(drawFrame)];
        [m_displayLink setFrameInterval:1];
        [m_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    }
}

- (void)stopAnimation
{
    [m_displayLink invalidate];
    m_displayLink = nil;
}

- (void)drawFrame
{
    m_frameCount ++;
    
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    JSCContext::getInstance()->runMainLoop();
    
    [m_glView presentFrameBuffer];
}

- (void) updateFPS
{
    [m_fpsLabel setText:[NSString stringWithFormat:@"fps:%ld", m_frameCount]];
    m_frameCount = 0;
}

- (CGPoint) locationInGL:(CGPoint) point
{
    CGPoint transformed;
    transformed.x = point.x * m_glView.layer.contentsScale;
    transformed.y = m_glView.drawableHeight - point.y * m_glView.layer.contentsScale;
    
    return transformed;
}

- (void) fpsTapDetected
{
    m_console.hidden = !m_console.hidden;
}

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGPoint point = [[touches anyObject] locationInView:m_glView];
    point = [self locationInGL:point];
    TRACE("Touched (%f, %f)", point.x, point.y)
    
    EventManager::getInstance()->onTouch(point.x, point.y);
}

- (void) glTapDetected:(UITapGestureRecognizer *)gestureRecognizer
{
    CGPoint point = [gestureRecognizer locationInView:m_glView];
    point = [self locationInGL:point];
    TRACE("Taped(%f, %f)", point.x, point.y)
    
    EventManager::getInstance()->onTap();
}

- (void) glSwipRightDetected:(UISwipeGestureRecognizer *)gestureRecognizer
{
    TRACE("Swiped Right")
    EventManager::getInstance()->onSwipe(SwipeDirectionRight);
}

- (void) glSwipLeftDetected:(UISwipeGestureRecognizer *)gestureRecognizer
{
    TRACE("Swiped Left")
    EventManager::getInstance()->onSwipe(SwipeDirectionLeft);
}

- (void) glSwipUpDetected:(UISwipeGestureRecognizer *)gestureRecognizer
{
    TRACE("Swiped Up")
    EventManager::getInstance()->onSwipe(SwipeDirectionUp);
}

- (void) glSwipDownDetected:(UISwipeGestureRecognizer *)gestureRecognizer
{
    TRACE("Swiped Down")
    EventManager::getInstance()->onSwipe(SwipeDirectionDown);
}

- (void) glDragDetected:(UIPanGestureRecognizer *)gestureRecognizer
{
    if( gestureRecognizer.state == UIGestureRecognizerStateBegan ) {
        CGPoint point = [gestureRecognizer locationInView:m_glView];
        point = [self locationInGL:point];
        TRACE("Move Begin (%f, %f)", point.x, point.y)
    }
    
    CGPoint moved = [gestureRecognizer translationInView:m_glView];
    moved.x *= m_glView.layer.contentsScale;
    moved.y *= -m_glView.layer.contentsScale;
    TRACE("Moved (%f, %f)", moved.x, moved.y)
    EventManager::getInstance()->onDrag(moved.x, moved.y);
    
    if( gestureRecognizer.state == UIGestureRecognizerStateEnded ) {
        TRACE("Move End")
        EventManager::getInstance()->onDragEnd();
    }
}

- (BOOL)gestureRecognizer:(UIPanGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UISwipeGestureRecognizer *)otherGestureRecognizer
{
    return YES;
}

@end
