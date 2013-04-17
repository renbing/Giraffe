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

#include "Audio.h"
#include "JSCContext.h"

JS_PROPERTY_FUNC(Audio, NUMBER, volume)
JS_PROPERTY_FUNC(Audio, BOOL, muted)
JS_PROPERTY_FUNC(Audio, BOOL, loop)
JS_PROPERTY_FUNC(Audio, BOOL, autoplay)

JS_PROPERTY_FUNC(Audio, STRING, src)

JS_SIMPLE_FUNC(Audio, play, VOID)
JS_SIMPLE_FUNC(Audio, pause, VOID)

JSPropertySpec Audio::js_ps[] = {
    JS_PROPERTY_DEF(Audio, volume),
    JS_PROPERTY_DEF(Audio, muted),
    JS_PROPERTY_DEF(Audio, loop),
    JS_PROPERTY_DEF(Audio, autoplay),
    JS_PROPERTY_DEF(Audio, src),
    JS_PS_END
};

JSFunctionSpec Audio::js_fs[] = {
    JS_FN_DEF(Audio, play, 0),
    JS_FN_DEF(Audio, pause, 0),
    JS_FS_END
};


JSPropertySpec Audio::js_sps[] = {
    JS_PS_END
};
JSFunctionSpec Audio::js_sfs[] = {
    JS_FS_END
};

JS_CLASS_EXPORT(Audio)

#include "jnihelper.h"

#include "audio.h"

extern JavaVM *g_jvm;
extern jobject g_jgl;

Audio::Audio()
{
	m_autoplay = false;
	m_loop = false;
	m_player = NULL;
	m_muted = false;
	m_volume = 1.0;

	JniHelper jniHelper;
	JNIEnv *env = jniHelper.getEnv();
	jmethodID newAudioPlayer = jniHelper.getMethod(g_jgl, "newAudioPlayer", "()Lcom/woyouquan/Giraffe/JavaAudioPlayer;");
	if( env != NULL && newAudioPlayer != NULL )
	{
		jobject player = env->CallObjectMethod(g_jgl, newAudioPlayer);
		m_player = env->NewGlobalRef(player);
	}
}

Audio::~Audio()
{
	LOG("Audio destructor: %s", m_src.c_str());
	if( m_player )
	{
		JniHelper jniHelper;

		JNIEnv *env = jniHelper.getEnv();
		jmethodID method = jniHelper.getMethod(m_player, "release", "()V");
		if( env != NULL && method != NULL )
		{
			LOG("Audio release");
			env->CallVoidMethod(m_player, method);
		}
		jniHelper.DeleteGlobalRef(m_player);
	}
}

void Audio::set_src(string src)
{
	m_src = src;

	if( !m_player )
	{
		return;
	}

	string fullPath = JSCContext::getInstance()->path() + m_src;
	TRACE("Audio audio src:%s", fullPath.c_str());

	JniHelper jniHelper;

	JNIEnv *env = jniHelper.getEnv();
	jmethodID setSource = jniHelper.getMethod(m_player, "setSource", "(Ljava/lang/String;)V");
	if( env != NULL && setSource != NULL )
	{
		jstring path = env->NewStringUTF( fullPath.c_str() );
		env->CallVoidMethod(m_player, setSource, path);
        TRACE("Audio setSource");
	}
}

void Audio::play()
{
	if( !m_player )
	{
		return;
	}

	JniHelper jniHelper;

	JNIEnv *env = jniHelper.getEnv();
	jmethodID method = jniHelper.getMethod(m_player, "play", "()V");
	if( env != NULL && method != NULL )
	{
		env->CallVoidMethod(m_player, method);
	}
}

void Audio::pause()
{
	if( !m_player )
	{
		return;
	}

	JniHelper jniHelper;

	JNIEnv *env = jniHelper.getEnv();
	jmethodID method = jniHelper.getMethod(m_player, "pause", "()V");
	if( env != NULL && method != NULL )
	{
		env->CallVoidMethod(m_player, method);
	}
}

void Audio::set_loop(bool loop)
{
	m_loop = loop;

	if( !m_player )
	{
		return;
	}

	JniHelper jniHelper;

	JNIEnv *env = jniHelper.getEnv();
	jmethodID method = jniHelper.getMethod(m_player, "setLoop", "(Z)V");
	if( env != NULL && method != NULL )
	{
		env->CallVoidMethod(m_player, method, loop);
	}
}

void Audio::set_autoplay(bool autoplay)
{
	m_autoplay = autoplay;

	if( !m_player )
	{
		return;
	}

	JniHelper jniHelper;

	JNIEnv *env = jniHelper.getEnv();
	jmethodID method = jniHelper.getMethod(m_player, "setAutoplay", "(Z)V");
	if( env != NULL && method != NULL )
	{
		env->CallVoidMethod(m_player, method, autoplay);
	}
}

void Audio::set_muted(bool muted)
{
	m_muted = muted;

	if( !m_player )
	{
		return;
	}

	float realVolume = m_muted ? 0.0 : m_volume;

	JniHelper jniHelper;

	JNIEnv *env = jniHelper.getEnv();
	jmethodID method = jniHelper.getMethod(m_player, "setVolume", "(F)V");
	if( env != NULL && method != NULL )
	{
		env->CallVoidMethod(m_player, method, realVolume);
	}
}


void Audio::set_volume(float volume)
{
	m_volume = volume;

	if( !m_player )
	{
		return;
	}

	float realVolume = m_muted ? 0.0 : m_volume;

	JniHelper jniHelper;

	JNIEnv *env = jniHelper.getEnv();
	jmethodID method = jniHelper.getMethod(m_player, "setVolume", "(F)V");
	if( env != NULL && method != NULL )
	{
		env->CallVoidMethod(m_player, method, realVolume);
	}
}
