//
//  JSCContext.mm
//  Giraffe
//
//  Created by kins on 1/1/13.
//  Copyright (c) 2012 kins. All rights reserved.
//

#import "JSCContext.h"
/*

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

- (void)onCallback
{
    if( self.callback ) {
        self.callback(m_buffer.bytes, m_buffer.length, self.callbackArg);
    }
}

- (void)onLoad:(NSNotification *)notification
{
    NSFileHandle *handle = notification.object;
    NSData *data = [notification.userInfo objectForKey:NSFileHandleNotificationDataItem];
    if( data.length == 0 ) {
        [[NSNotificationCenter defaultCenter] removeObserver:self name:NSFileHandleReadToEndOfFileCompletionNotification object:handle];
        
        [self performSelectorOnMainThread:@selector(onCallback) withObject:nil waitUntilDone:YES];
        return;
    }
    
    [m_buffer appendData:data];
    [handle readInBackgroundAndNotify];
}

@end
 
*/

@interface AsyncFileRead : NSObject

@property LoadFileCallback callback;
@property void *callbackArg;

- (void)load:(NSString *)path;
- (void)onLoad:(NSData *)data;

@end

@implementation AsyncFileRead

- (void)load:(NSString *)path
{
    NSData *data = nil;
    NSFileHandle *handle = [NSFileHandle fileHandleForReadingAtPath:path];
    if( handle ) {
        data = [handle readDataToEndOfFile];
    }
    
    [self performSelectorOnMainThread:@selector(onLoad:) withObject:data waitUntilDone:YES];
}

- (void)onLoad:(NSData *)data
{
    if( self.callback ) {
        if( data ) {
            self.callback(data.bytes, data.length, self.callbackArg);
        }else{
            self.callback(NULL, -1, self.callbackArg);
        }
    }
}

@end

int JSCContext::loadLocalFile(const string &file, string &content)
{
    LOG("load file:%s", file.c_str());
    
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
    LOG("load file async:%s", file.c_str());
    NSString *fullPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:[NSString stringWithUTF8String:file.c_str()]];
    
    AsyncFileRead *reader = [[AsyncFileRead alloc] init];
    reader.callback = callback;
    reader.callbackArg = callbackArg;
    [reader performSelectorInBackground:@selector(load:) withObject:fullPath];
    
    
    /*
     NSData *data = [handle readDataToEndOfFile];
    if( callback ){
        if( data ) {
            callback([data bytes], data.length, callbackArg);
        }else{
            callback(NULL, -1, callbackArg);
        }
    }
    
    AsyncFileRead *reader = [[AsyncFileRead alloc] init];
    reader.callback = callback;
    reader.callbackArg = callbackArg;
    
    [[NSNotificationCenter defaultCenter] addObserver:reader selector:@selector(onLoad:) name:NSFileHandleReadCompletionNotification object:handle];
    
    [handle readInBackgroundAndNotify];
    */
}