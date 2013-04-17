/***
 * -*- C++ -*-
 * -*- UTF-8 -*-
 *
 * Copyright (C),
 * @file
 * @brief
 * @author:
 * 		<name>		<email>
 * 		kins ren	kins.ren@me.com	
 * @version
 * @date
 * @warning
 * @History:
 * 		<author>	<time>	<desc>
*/

#pragma once

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <android/log.h>

#define DEBUG

#ifdef DEBUG
#define TRACE(...) __android_log_print(ANDROID_LOG_INFO, "ProjectName", __VA_ARGS__);
#else
#define TRACE(...)
#endif

#define LOG(...)
#define ERROR(...) __android_log_print(ANDROID_LOG_ERROR, "ProjectName", __VA_ARGS__);

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#include <stdlib.h>
#include <unistd.h>

#define CC_RGB_PREMULTIPLY_APLHA(vr, vg, vb, va) \
(unsigned)(((unsigned)((unsigned char)(vr) * ((unsigned char)(va) + 1)) >> 8) | \
		((unsigned)((unsigned char)(vg) * ((unsigned char)(va) + 1) >> 8) << 8) | \
		((unsigned)((unsigned char)(vb) * ((unsigned char)(va) + 1) >> 8) << 16) | \
		((unsigned)(unsigned char)(va) << 24))


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

static void mysleep(int msec)
{
	struct timeval delay;
	delay.tv_sec = (int) msec/1000;
	delay.tv_usec = (msec % 1000) * 1000;
	select(0, NULL, NULL, NULL, &delay);
}
