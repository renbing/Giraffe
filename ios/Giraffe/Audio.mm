//
//  Audio.m
//  Giraffe
//
//  Created by kins on 2/22/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#import "Audio.h"
#import "Http.h"
#import "JSCContext.h"

JS_PROPERTY_FUNC(Audio, NUMBER, volume)
JS_PROPERTY_FUNC(Audio, BOOL, muted)
JS_PROPERTY_FUNC(Audio, BOOL, loop)
JS_PROPERTY_FUNC(Audio, BOOL, autoplay)

JS_PROPERTY_FUNC(Audio, STRING, src)

JSPropertySpec Audio::js_ps[] = {
    JS_PROPERTY_DEF(Audio, volume),
    JS_PROPERTY_DEF(Audio, muted),
    JS_PROPERTY_DEF(Audio, loop),
    JS_PROPERTY_DEF(Audio, autoplay),
    JS_PROPERTY_DEF(Audio, src),
    JS_PS_END
};

JSFunctionSpec Audio::js_fs[] = {
    JS_FS_END
};


JSPropertySpec Audio::js_sps[] = {
    JS_PS_END
};
JSFunctionSpec Audio::js_sfs[] = {
    JS_FS_END
};

JS_CLASS_EXPORT(Audio)

void onAudioLoad(const void *data, int size, void *arg)
{
    if( !arg || !data || size <= 0 ) return;
    
    Audio *audio = static_cast<Audio *>(arg);
    audio->onLoad(data, size);
}


Audio::Audio()
: m_player(NULL),m_playing(false),m_volume(1.0),m_loop(false),m_muted(false),m_autoplay(true)
{
    m_player = [AVAudioPlayer alloc];
}

Audio::~Audio()
{
    [m_player release];
}

void Audio::set_src(string src)
{
    m_src = src;
    
    JSCContext::getInstance()->loadFileAsync(m_src, onAudioLoad, this);
}

void Audio::play()
{
    m_playing = true;
    [m_player play];
}

void Audio::pause()
{
    m_playing = false;
    [m_player pause];
}

void Audio::onLoad(const void *data, int size)
{
    NSData *audioData = [NSData dataWithBytes:data length:size];
    
    [m_player initWithData:audioData error:NULL];
    if( m_autoplay ) {
        play();
    }
}