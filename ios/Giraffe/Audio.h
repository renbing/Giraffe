//
//  Audio.h
//  Giraffe
//
//  Created by kins on 2/22/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#ifndef __Giraffe__Audio__
#define __Giraffe__Audio__

#import <AVFoundation/AVFoundation.h>
#import <string>
#import "JSCContext.h"

using std::string;


class Audio
{
private:
    string m_src;
    AVAudioPlayer *m_player;
    
    bool m_playing;
    
    float m_volume;
    bool m_muted;
    bool m_loop;
    bool m_autoplay;
    
public:
    Audio();
    ~Audio();
    
    JS_CLASS_EXPORT_DEF
    
    void set_volume(float volume) {
        m_volume = volume;
        float realVolume = m_muted ? 0.0 : m_volume;
        [m_player setVolume:realVolume];
    }
    float get_volume() { return m_volume; }
    
    void set_muted(float muted) {
        m_muted = muted;
        float realVolume = m_muted ? 0.0 : m_volume;
        [m_player setVolume:realVolume];
    }
    bool get_muted() { return m_muted; }
    
    void set_loop(bool loop) {
        m_loop = loop;
        [m_player setNumberOfLoops:(m_loop ? -1 : 0)];
    }
    bool get_loop() { return m_loop; }
    
    void set_autoplay(bool autoplay) { m_autoplay = autoplay; }
    bool get_autoplay() { return m_autoplay; }

    void set_src(string src);
    string get_src() { return m_src; }
    
    void play();
    void pause();

private:
    friend void onAudioLoad(const void *data, int size, void *arg);
    void onLoad(const void *data, int size);
    
};

#endif