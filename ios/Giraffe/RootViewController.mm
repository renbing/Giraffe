//
//  RootViewController.m
//  Giraffe
//
//  Created by kins on 12/9/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <sys/sysctl.h>

#import "RootViewController.h"
#import "MovieClip.h"
#import "Device.h"

TimerCallbackFunc static void updateFPS(void *arg) {
    RootViewController * viewController = (RootViewController *)arg;
    [viewController updateFPS];
}

@implementation RootViewController

@synthesize orientation = UIInterfaceOrientationLandscapeLeft;
//@synthesize orientation = UIInterfaceOrientationPortrait;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        
        m_pinchScale = 1.0f;
        
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
        
        Device::width = m_glView.drawableWidth;
        Device::height = m_glView.drawableHeight;
        
        size_t bufSize = 128;
        char machine[128];
        sysctlbyname("hw.machine", machine, &bufSize, NULL, 0);
        Device::name = machine;
        
        m_glRenderContext = RenderContextGL2::getInstance();
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        LOG("MaxTextureSize supported=%d", maxTextureSize)
        
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
        UITapGestureRecognizer *fpsTapGesture = [[[UITapGestureRecognizer alloc]
                initWithTarget:self action:@selector(fpsTapDetected)] autorelease];
        [m_fpsLabel addGestureRecognizer:fpsTapGesture];
        
        UITapGestureRecognizer *tapRecognizer = [[[UITapGestureRecognizer alloc]
                initWithTarget:self action:@selector(glTapDetected:)] autorelease];
        [m_glView addGestureRecognizer:tapRecognizer];
        
        UISwipeGestureRecognizer *swipLeftRecognizer = [[[UISwipeGestureRecognizer alloc]
                initWithTarget:self action:@selector(glSwipLeftDetected:)] autorelease];
        swipLeftRecognizer.direction = UISwipeGestureRecognizerDirectionLeft;
        swipLeftRecognizer.delegate = self;
        [m_glView addGestureRecognizer:swipLeftRecognizer];
        
        UISwipeGestureRecognizer *swipRightRecognizer = [[[UISwipeGestureRecognizer alloc]
                initWithTarget:self action:@selector(glSwipRightDetected:)] autorelease];
        swipRightRecognizer.direction = UISwipeGestureRecognizerDirectionRight;
        swipRightRecognizer.delegate = self;
        [m_glView addGestureRecognizer:swipRightRecognizer];
        
        UISwipeGestureRecognizer *swipDownRecognizer = [[[UISwipeGestureRecognizer alloc]
                initWithTarget:self action:@selector(glSwipDownDetected:)] autorelease];
        swipDownRecognizer.direction = UISwipeGestureRecognizerDirectionUp;
        swipDownRecognizer.delegate = self;
        [m_glView addGestureRecognizer:swipDownRecognizer];
        
        UISwipeGestureRecognizer *swipUpRecognizer = [[[UISwipeGestureRecognizer alloc]
                initWithTarget:self action:@selector(glSwipUpDetected:)] autorelease];
        swipUpRecognizer.direction = UISwipeGestureRecognizerDirectionDown;
        swipUpRecognizer.delegate = self;
        [m_glView addGestureRecognizer:swipUpRecognizer];
        
        UIPanGestureRecognizer *panRecognizer = [[[UIPanGestureRecognizer alloc]
                initWithTarget:self action:@selector(glDragDetected:)] autorelease];
        panRecognizer.minimumNumberOfTouches = 1;
        panRecognizer.maximumNumberOfTouches = 1;
        panRecognizer.delegate = self;
        [m_glView addGestureRecognizer:panRecognizer];
        
        UIPinchGestureRecognizer *pinchRecognizer = [[[UIPinchGestureRecognizer alloc]
                initWithTarget:self action:@selector(glPinchDetected:)] autorelease];
        pinchRecognizer.delegate = self;
        [m_glView addGestureRecognizer:pinchRecognizer];
        
        UILongPressGestureRecognizer *longPressRecognizer = [[[UILongPressGestureRecognizer alloc]
                initWithTarget:self action:@selector(glLongPressDetected:)] autorelease];
        
        longPressRecognizer.minimumPressDuration=1;//默认0.5秒
        [m_glView addGestureRecognizer:longPressRecognizer];
        
        
        [m_glView addGestureRecognizer:panRecognizer];
        
        [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(updateFPS)
                                       userInfo:NULL repeats:YES];
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
    LOG("resizeGLBuffer")
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
    LOG("Touched (%f, %f)", point.x, point.y)
    
    EventManager::getInstance()->onTouch(point.x, point.y);
}

- (void) glTapDetected:(UITapGestureRecognizer *)gestureRecognizer
{
    CGPoint point = [gestureRecognizer locationInView:m_glView];
    point = [self locationInGL:point];
    LOG("Taped(%f, %f)", point.x, point.y)
    
    EventManager::getInstance()->onTap();
}

- (void) glSwipRightDetected:(UISwipeGestureRecognizer *)gestureRecognizer
{
    LOG("Swiped Right")
    EventManager::getInstance()->onSwipe(SwipeDirectionRight);
}

- (void) glSwipLeftDetected:(UISwipeGestureRecognizer *)gestureRecognizer
{
    LOG("Swiped Left")
    EventManager::getInstance()->onSwipe(SwipeDirectionLeft);
}

- (void) glSwipUpDetected:(UISwipeGestureRecognizer *)gestureRecognizer
{
    LOG("Swiped Up")
    EventManager::getInstance()->onSwipe(SwipeDirectionUp);
}

- (void) glSwipDownDetected:(UISwipeGestureRecognizer *)gestureRecognizer
{
    LOG("Swiped Down")
    EventManager::getInstance()->onSwipe(SwipeDirectionDown);
}

- (void) glDragDetected:(UIPanGestureRecognizer *)gestureRecognizer
{
    //TRACE("Drag %d", [NSThread isMainThread]);
    if( gestureRecognizer.state == UIGestureRecognizerStateBegan ) {
        CGPoint point = [gestureRecognizer locationInView:m_glView];
        point = [self locationInGL:point];
        LOG("Drag Start (%f, %f)", point.x, point.y)
        m_dragLastMoved = CGPointMake(0, 0);
    }
    
    CGPoint moved = [gestureRecognizer translationInView:m_glView];
    moved.x *= m_glView.layer.contentsScale;
    moved.y *= -m_glView.layer.contentsScale;
    EventManager::getInstance()->onDrag(moved.x - m_dragLastMoved.x, moved.y - m_dragLastMoved.y);
    m_dragLastMoved.x = moved.x;
    m_dragLastMoved.y = moved.y;
    
    LOG("Drag (%f, %f)", moved.x - m_dragLastMoved.x, moved.y - m_dragLastMoved.y)
    
    if( gestureRecognizer.state == UIGestureRecognizerStateEnded ) {
        LOG("Drag End")
        EventManager::getInstance()->onDragEnd();
    }
}

- (void) glPinchDetected:(UIPinchGestureRecognizer *)gestureRecognizer
{
    if( gestureRecognizer.state == UIGestureRecognizerStateBegan ) {
        LOG("Pinch begin")
        m_pinchScale = 1.0;
    }
    LOG("Pinch %f", gestureRecognizer.scale)
    EventManager::getInstance()->onPinch(gestureRecognizer.scale/m_pinchScale);
    m_pinchScale = gestureRecognizer.scale;
}

- (void) glLongPressDetected:(UILongPressGestureRecognizer *)gestureRecognizer
{
    LOG("LongPress")
    EventManager::getInstance()->onLongPress();
}

- (BOOL)gestureRecognizer:(UIPanGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UISwipeGestureRecognizer *)otherGestureRecognizer
{
    return YES;
}

@end
