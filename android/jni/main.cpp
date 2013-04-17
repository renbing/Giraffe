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

#include <jni.h>
#include <string.h>
#include <pthread.h>

#include <png.h>
#include <zip.h>
#include <curl/curl.h>

#include "global.h"
#include "JSCContext.h"
#include "MovieClip.h"
#include "RenderContextGL2.h"
#include "Device.h"

// Android资源文件zip读取全局变量
zip *apkArchive;

//  暴露给Java的函数接口
extern "C" {

/* Canvas 类JNI函数
*	stringFromJNI(void) 		测试JNI调用
*	nativeInit(String apkPath)	主程序初始化工作
*	nativeDone(void)			主程序结束清理工作
*/
jobject g_jgl = NULL;
JavaVM *g_jvm = NULL;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    return JNI_VERSION_1_4;
}

jstring Java_com_woyouquan_Giraffe_GiraffeActivity_stringFromJNI( JNIEnv *env, jobject obj )
{
	return env->NewStringUTF("Hello from NDK !");
}

void Java_com_woyouquan_Giraffe_GiraffeActivity_nativeInit( JNIEnv *env, jobject obj, jstring apkPath )
{
	const char *strApkPath = env->GetStringUTFChars(apkPath, 0);
	LOG("apk path: %s", strApkPath);

	curl_global_init(CURL_GLOBAL_NOTHING);

	apkArchive = zip_open(strApkPath, 0, NULL);
	env->ReleaseStringUTFChars(apkPath, strApkPath);

	if( apkArchive == NULL )
	{
		LOG("Loading APK failed");
	}

	LOG("canvas init");
}

void Java_com_woyouquan_Giraffe_GiraffeActivity_nativeDone( JNIEnv *env )
{
	zip_close(apkArchive);

	LOG("canvas done");
}


/* EAGLView 类JNI函数
*	nativeInit(void)						OpenGL ES初始化
*	nativeResize(int width, int height)		OpenGL ES窗口大小改变
*	nativeRenderer(void)					OpenGL ES逐帧绘图
*	nativeDown(int x, int y)				触摸点下
*	nativeMove(int x, int y)				触摸移动
*	nativeUp(int x, int y)					触摸结束
*/
void Java_com_woyouquan_Giraffe_EAGLView_nativeInit( JNIEnv *env , jobject jgl)
{
	env->GetJavaVM(&g_jvm);
	g_jgl = env->NewGlobalRef(jgl);

	LOG("opengl init");
}

void Java_com_woyouquan_Giraffe_EAGLView_nativeResize( JNIEnv *env, jobject obj, jint width, jint height )
{
	TRACE("opengl resize: width=%d height=%d", width, height);

    RenderContextGL2::getInstance()->layout(width, height, true);
    Device::width = width;
    Device::height = height;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    JSCContext::getInstance()->run("assets/battle/");
}

void Java_com_woyouquan_Giraffe_EAGLView_nativeRender( JNIEnv *env )
{
    glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

    JSCContext::getInstance()->runMainLoop();
}

void Java_com_woyouquan_Giraffe_EAGLView_nativeDown( JNIEnv *env, jobject obj, jfloat x, jfloat y )
{
}

void Java_com_woyouquan_Giraffe_EAGLView_nativeMove( JNIEnv *env, jobject obj, jfloat x, jfloat y )
{
}

void Java_com_woyouquan_Giraffe_EAGLView_nativeUp( JNIEnv *env, jobject obj, jfloat x, jfloat y )
{
}

void Java_com_woyouquan_Giraffe_EAGLView_nativeTouch( JNIEnv *env, jobject obj, jfloat x, jfloat y )
{
    EventManager::getInstance()->onTouch(x, y);
}

void Java_com_woyouquan_Giraffe_EAGLView_nativeTap( JNIEnv *env, jobject obj, jfloat x, jfloat y )
{
    EventManager::getInstance()->onTap();
}

void Java_com_woyouquan_Giraffe_EAGLView_nativeDrag( JNIEnv *env, jobject obj, jint state, jfloat dx, jfloat dy)
{
    if( state == 0 ) {
        // DragStart
    }else if( state == 1 ) {
        // Drag
        EventManager::getInstance()->onDrag(dx, dy);
    }else{
        // DragEnd
        EventManager::getInstance()->onDragEnd();
    }
}

void Java_com_woyouquan_Giraffe_EAGLView_nativeSwipe( JNIEnv *env, jobject obj, jint direction)
{
    EventManager::getInstance()->onSwipe((SwipeDirection)direction);
}

void Java_com_woyouquan_Giraffe_EAGLView_nativePinch( JNIEnv *env, jobject obj, jfloat scale)
{
    EventManager::getInstance()->onPinch(scale);
}

void Java_com_woyouquan_Giraffe_EAGLView_nativeLongPress( JNIEnv *env, jobject obj)
{
    EventManager::getInstance()->onLongPress();
}

};
