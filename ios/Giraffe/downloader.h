//
//  downloader.h
//  Giraffe
//
//  Created by kins ren on 2/8/12.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#define DOWNLOAD_TIMEOUT 60

@protocol DownloadResponseDelegate

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response;

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data;

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error;

- (void)connectionDidFinishLoading:(NSURLConnection *)connection;


@optional

/* handle server redirect request */
- (NSURLRequest *)connection:(NSURLConnection *)connection 
					willSendRequest:(NSURLRequest *)request 
					redirectResponse:(NSURLResponse *)redirectResponse;

/* handle server require authentication */
- (void)connection:(NSURLConnection *)connection didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge;

/* Note: The delegate receives connection:willCacheResponse: messages only for protocols that support caching. */
- (NSCachedURLResponse *)connection:(NSURLConnection *)connection willCacheResponse:(NSCachedURLResponse *)cachedResponse;

@end



@interface Downloader : NSObject<DownloadResponseDelegate> {

	NSMutableData *receivedData;
	int status;

	id callbackObj;
	SEL callbackFunc;
}

- (void) downloadURL: (NSString *)strURL 
			withPost:(NSString *)post_data 
			withHeader:(NSDictionary *)headers 
			target:(id)obj
			action:(SEL)func;

@property (nonatomic, retain, readonly) NSMutableData *receivedData;
@property (readonly) int status;

@end

