//
//  downloader.m
//  Giraffe
//
//  Created by kins ren on 2/8/12.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "downloader.h"

@implementation Downloader

@synthesize receivedData;
@synthesize status;

- (id) init
{
	if (self = [super init])
	{
		/* create the NSMutableData instance that will hold the received data */
		receivedData = [[NSMutableData alloc] initWithLength:0];
	}

	return self;
}

- (void) downloadURL: (NSString *)strURL withPost:(NSString *)post_data withHeader:(NSDictionary *)headers target:(id)obj action:(SEL)func
{
	callbackObj = obj;
	callbackFunc = func;

	NSURL *url = [NSURL URLWithString:strURL];

	NSMutableURLRequest *theRequest = [[NSMutableURLRequest alloc] init];
	[theRequest setURL:url];
	[theRequest setTimeoutInterval:DOWNLOAD_TIMEOUT];

	/* set property like http headers,post data*/
	if(post_data && ([post_data length] > 0))
	{
		[theRequest setHTTPMethod:@"POST"];
		[theRequest setHTTPBody:[post_data dataUsingEncoding:NSUTF8StringEncoding]];
	}

	if(headers)
	{
		[theRequest setAllHTTPHeaderFields:headers];
	}
//	[theRequest setValue:@"Firefox" forHTTPHeaderField:@"User-Agent"];

	

	/* Create the connection with the request and start loading the
	 data. The connection object is owned both by the creator and the
	 loading system. if no delegate supply,use default*/
		
	NSURLConnection *connection = [[NSURLConnection alloc] 
									initWithRequest:theRequest 
								  	delegate:self
								  	startImmediately:YES];
	if (connection == nil) {
		/* inform the user that the connection failed */
		status = -1;
	}

	[theRequest release];
}

- (void)dealloc
{
	[receivedData release];
	[super dealloc];
}


#pragma mark DownloadResponse delegate methods

- (void) connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    /* This method is called when the server has determined that it has
	 enough information to create the NSURLResponse. It can be called
	 multiple times, for example in the case of a redirect, so each time
	 we reset the data. */
	
    [receivedData setLength:0];
	
	/* get response information like response headers,http response status code */	
	if ([response isKindOfClass:[NSHTTPURLResponse self]])
	{
		status = [(NSHTTPURLResponse *)response statusCode];

		/* deal with response header
		printf("Response headers:\n");
		NSDictionary *headers = [(NSHTTPURLResponse *)response allHeaderFields];
		NSString *key = nil;
		for (key in headers)
		{
			printf("%s: %s\n",[key UTF8String],[[headers valueForKey:key] UTF8String]);
		}
		*/
	}
}


- (void) connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    /* Append the new data to the received data. */
    [receivedData appendData:data];
}


- (void) connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	status = -1;
	[callbackObj performSelector:callbackFunc withObject:self];
    
	[connection release];
	connection = nil;
}

- (void) connectionDidFinishLoading:(NSURLConnection *)connection
{

	[callbackObj performSelector:callbackFunc withObject:self];

	[connection release];
	connection = nil;
}

@end
