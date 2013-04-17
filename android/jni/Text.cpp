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

#include "global.h"
#include "stringutil.h"
#include "jnihelper.h"

#include "Text.h"

extern JavaVM *g_jvm;
extern jobject g_jgl;

void Text::render()
{
	JniHelper jniHelper;
	JNIEnv *env = jniHelper.getEnv();

	jmethodID newJavaBitmap = jniHelper.getMethod(g_jgl, "newBitmap", "(II)Lcom/woyouquan/Giraffe/JavaBitmap;");
	if( env == NULL || newJavaBitmap == NULL )
	{
        return;
	}

    jobject jbitmap = env->CallObjectMethod(g_jgl, newJavaBitmap, POTWidth, POTHeight);

	// String text, int x, int y, int w, int h, String textAlign,String fontName,
	// float fontSize, String fontSizeUnit, int textColor, boolean bold, boolean italic
	jmethodID method = jniHelper.getMethod(jbitmap, "fillText", 
                    "(Ljava/lang/String;IIIILjava/lang/String;Ljava/lang/String;FLjava/lang/String;IZZ)Landroid/graphics/Rect;");
	if( method == NULL )
	{
        return;
    }

    jstring jText = env->NewStringUTF( text.c_str() );
    jstring jTextAlign = env->NewStringUTF( align.c_str() );
    jstring jFontName = env->NewStringUTF( "sans-serif" );
    jstring jFontSizeUnit = env->NewStringUTF( "px" );

    float fontSize = atof(font.c_str());
    if( fontSize < 14.0 ) {
        fontSize = 14.0;
    }

    unsigned int fillColor;
	StringUtil::convertHTMLColor(color, fillColor);

    jobject jrect = env->CallObjectMethod(jbitmap, method, jText, 0, 0, width, height, 
                    jTextAlign, jFontName, fontSize, jFontSizeUnit, fillColor, 0, 0);
	
    method = jniHelper.getMethod(jbitmap, "getRectPixels", "(IIII)[B");
	if( method == NULL )
	{
		return;
	}
	jbyteArray bytes = (jbyteArray) env->CallObjectMethod(jbitmap, method, 0, 0, POTWidth, POTHeight);

	unsigned long size = POTWidth * POTHeight * 4;
	unsigned char *buf  = new unsigned char[size];

	env->GetByteArrayRegion(bytes, 0, size, (jbyte*)buf);
	
	// 开始预乘
	for( int i=0; i<POTHeight; i++ )
	{
		for( int j=0; j<POTWidth; j++ )
		{
			//ARGB
			unsigned char *pixel = (unsigned char *)(buf + (i * POTWidth + j) * 4);
			*((unsigned int *)pixel) = CC_RGB_PREMULTIPLY_APLHA( pixel[1], pixel[2], pixel[3], pixel[0] );
		}
	}

	// 更新Opengl ES Texture
    if( m_texture <= 0 ) {
        glGenTextures(1, &m_texture);
    }

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, POTWidth, POTHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);

	delete[] buf;
}
