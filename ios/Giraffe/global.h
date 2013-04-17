//
//  global.h
//  Giraffe
//
//  Created by kins on 12/4/12.
//  Copyright (c) 2012 kins. All rights reserved.
//

#ifndef Giraffe_global_h
#define Giraffe_global_h

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#define TRACE(...) printf(__VA_ARGS__); printf("\n");
#else
#define TRACE(...)
#endif

#define LOG(...)
//#define LOG(...) printf(__VA_ARGS__); printf("\n");
#define ERROR(...) printf(__VA_ARGS__); printf("\n");

#define EventCallbackFunc
#define TimerCallbackFunc

static unsigned long computePOT(unsigned long x)
{
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >>16);
	return x + 1;
}

#endif
