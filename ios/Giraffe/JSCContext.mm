//
//  JSCContext.mm
//  Giraffe
//
//  Created by kins on 1/1/13.
//  Copyright (c) 2012 kins. All rights reserved.
//

#import "JSCContext.h"

@interface AsyncFileRead : NSObject {
    NSMutableData *m_buffer;
}

@property LoadFileCallback callback;
@property void *callbackArg;

- (void)onLoad:(NSNotification *)notification;

@end

@implementation AsyncFileRead

@synthesize callback,callbackArg;


- (id)init
{
    if((self = [super init])){
        self.callback = NULL;
        self.callbackArg = NULL;
        m_buffer = [[NSMutableData alloc] init];
    }
    
    return self;
}

- (void)dealloc
{
    [m_buffer release];
    [super dealloc];
}

- (void)onLoad:(NSNotification *)notification
{
    NSFileHandle *handle = notification.object;
    NSData *data = [notification.userInfo objectForKey:NSFileHandleNotificationDataItem];
    if( data.length == 0 ) {
        [[NSNotificationCenter defaultCenter] removeObserver:self name:NSFileHandleReadToEndOfFileCompletionNotification object:handle];
        
        if( self.callback ) {
            self.callback(m_buffer.bytes, m_buffer.length, self.callbackArg);
        }
        return;
    }
    
    [m_buffer appendData:data];
    [handle readInBackgroundAndNotify];
}

@end

int JSCContext::loadLocalFile(const string &file, string &content)
{
    NSString *fullPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:[NSString stringWithUTF8String:file.c_str()]];
    NSString *nscontent = [NSString stringWithContentsOfFile:fullPath encoding:NSUTF8StringEncoding error:NULL];
    if( !nscontent ) {
        return -1;
    }
    content = [nscontent UTF8String];
    
    return nscontent.length;
}

void JSCContext::loadLocalFileAsync(const string &file, LoadFileCallback callback, void *callbackArg)
{
    printf("load %s\n", file.c_str());
    NSString *fullPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:[NSString stringWithUTF8String:file.c_str()]];
    
    NSFileHandle *handle = [NSFileHandle fileHandleForReadingAtPath:fullPath];
    
    AsyncFileRead *reader = [[AsyncFileRead alloc] init];
    reader.callback = callback;
    reader.callbackArg = callbackArg;
    
    [[NSNotificationCenter defaultCenter] addObserver:reader selector:@selector(onLoad:) name:NSFileHandleReadCompletionNotification object:handle];
    
    [handle readInBackgroundAndNotify];
}