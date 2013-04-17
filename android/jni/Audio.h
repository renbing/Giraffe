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

#include <jni.h>
#include "JSCContext.h"

class Audio
{
private:
    string m_src;
    jobject m_player;
    
    bool m_playing;
    
    float m_volume;
    bool m_muted;
    bool m_loop;
    bool m_autoplay;
    
public:
    JSObject *jsthis;
    
public:
    Audio();
    ~Audio();
    
    JS_CLASS_EXPORT_DEF
    
    void    set_volume(float volume);
    void    set_muted(bool muted);
    void    set_loop(bool loop);
    void    set_autoplay(bool autoplay);
    void    set_src(string src);

    float   get_volume() { return m_volume; }
    bool    get_muted() { return m_muted; }
    bool    get_loop() { return m_loop; }
    bool    get_autoplay() { return m_autoplay; }
    string  get_src() { return m_src; }
    
    void    play();
    void    pause();

private:
    friend void onAudioLoad(const void *data, int size, void *arg);
    void onLoad(const void *data, int size);
    
};
