//
//  Audio.m
//  Giraffe
//
//  Created by kins on 2/22/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>

#import "Audio.h"
#import "Http.h"
#import "JSCContext.h"

@interface AudioManager : NSObject<AVAudioPlayerDelegate> {
    NSMutableArray *audios;
}

+ (AudioManager *)getInstance;
- (void) add:(AVAudioPlayer *)audio;

@end

@implementation AudioManager

+ (AudioManager *)getInstance
{
    static AudioManager *instance = NULL;
    if( !instance ) {
        instance = [[AudioManager alloc] init];
    }
    
    return instance;
}

- (id) init
{
    if( (self = [super init]) ){
        audios = [[NSMutableArray alloc] init];
    }
    
    return self;
}

- (void) add:(AVAudioPlayer *)player
{
    [audios addObject:player];
    player.delegate = self;
}

- (void) audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
    [audios removeObject:player];
}

@end

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

void onAudioLoad(const void *data, int size, void *arg)
{
    if( !arg || !data || size <= 0 ) return;
    
    Audio *audio = static_cast<Audio *>(arg);
    audio->onLoad(data, size);
}


Audio::Audio()
: m_player(NULL),m_playing(false),m_volume(1.0),m_loop(false),m_muted(false),m_autoplay(true)
{
}

Audio::~Audio()
{
    [(AVAudioPlayer *)m_player release];
}

void Audio::set_src(string src)
{
    m_src = src;
    
    JSCContext::getInstance()->loadFileAsync(m_src, onAudioLoad, this);
}

void Audio::set_volume(float volume) {
    m_volume = volume;
    float realVolume = m_muted ? 0.0 : m_volume;
    [(AVAudioPlayer *)m_player setVolume:realVolume];
}

void Audio::set_muted(bool muted) {
    m_muted = muted;
    float realVolume = m_muted ? 0.0 : m_volume;
    [(AVAudioPlayer *)m_player setVolume:realVolume];
}

void Audio::set_loop(bool loop) {
    m_loop = loop;
    [(AVAudioPlayer *)m_player setNumberOfLoops:(m_loop ? -1 : 0)];
}

void Audio::play()
{
    m_playing = true;
    AVAudioPlayer *player = static_cast<AVAudioPlayer *>(m_player);
    if( !m_loop && player.playing ) {
        player = [[[AVAudioPlayer alloc] initWithData:player.data error:NULL] autorelease];
        player.volume = m_volume;
        [[AudioManager getInstance] add:player];
    }
    [player play];
}

void Audio::pause()
{
    m_playing = false;
    [(AVAudioPlayer *)m_player pause];
}

void Audio::onLoad(const void *data, int size)
{
    NSData *audioData = [NSData dataWithBytes:data length:size];
    
   // AVAudioPlayer *player = [[[AVAudioPlayer alloc] initWithData:audioData error:NULL] autorelease];
    AVAudioPlayer *player = [[AVAudioPlayer alloc] initWithData:audioData error:NULL];
    if( !player ) {
        ERROR("Audio Create Error: %s", m_src.c_str());
        return;
    }
    
    m_player = player;
    
    set_autoplay(m_autoplay);
    set_muted(m_muted);
    set_volume(m_volume);
    
    if( m_autoplay || m_playing ) {
        play();
    }
}