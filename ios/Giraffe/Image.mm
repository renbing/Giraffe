//
//  Image.mm
//  Giraffe
//
//  Created by kins on 12/9/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#import "Image.h"
#import "Http.h"

void Image::createTexture(const void *input, unsigned int size)
{
    UIImage *img = [UIImage imageWithData:[NSData dataWithBytes:input length:size]];
	CGImageRef image = img.CGImage;
	width = CGImageGetWidth(image);
	height = CGImageGetHeight(image);
	 
	POTWidth = computePOT(width);
	POTHeight = computePOT(height);
	 
	 
	CGImageAlphaInfo info = CGImageGetAlphaInfo(image);
	hasAlpha = ((info == kCGImageAlphaPremultipliedLast) || (info == kCGImageAlphaPremultipliedFirst) || (info == kCGImageAlphaLast) || (info == kCGImageAlphaFirst) ? YES : NO);
	info = hasAlpha ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast;
	 
	 
	GLubyte *buffer = (GLubyte *)malloc( width * height * 4 );
	CGContextRef ctx = CGBitmapContextCreate( buffer, width, height, 8, 4 * width,
											  CGImageGetColorSpace(image),  info | kCGBitmapByteOrder32Big);
	CGContextClearRect( ctx, CGRectMake( 0, 0, width, height ) );
	CGContextDrawImage( ctx, CGRectMake( 0, 0, width, height ), image );
	 
	CGContextRelease(ctx);
	 
	if( POTWidth != width || POTHeight != height )
	{
		GLubyte *tmpData = (GLubyte *)malloc(POTWidth * POTHeight * 4);
		bzero(tmpData, POTWidth * POTHeight * 4);
		for( int i=0; i<height; i++ )
		{
			memcpy(tmpData+i*POTWidth*4, buffer+i*width*4, width*4);
		}
		 
		free(buffer);
		buffer = tmpData;
	}
    
    setupTexture(buffer);

	free(buffer);

    JSCContext::getInstance()->callJSFunction(onload);
    LOG("load Texture finished:%s", m_src.c_str());
}
