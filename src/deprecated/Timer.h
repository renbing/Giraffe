//
//  Timer.h
//  Giraffe
//
//  Created by kins on 1/15/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#ifndef __Giraffe__Timer__
#define __Giraffe__Timer__

#include <map>
#include <vector>

using std::map;
using std::vector;

typedef void (*TimerCallback) (void *arg);

class TimerManager
{
private:
    static TimerManager *m_instance;
    int     m_counter;
    
    typedef struct
    {
        TimerCallback   callback;
        float           second;
        float           passed;
        bool            repeat;
        void            *arg;
    } Timer;
    map<int, Timer> m_timers;

public:
    static TimerManager * getInstance();
    void    update(float passed);
    
    void    setTimeout(TimerCallback callback, float second, void *arg=NULL);
    int     setInterval(TimerCallback callback, float second, void *arg=NULL);
    void    clearInterval(int timerId);
    
private:
    TimerManager() {
        m_counter = 1;
    }
    
};

inline TimerManager * TimerManager::getInstance()
{
    if( m_instance == NULL ) {
        m_instance = new TimerManager();
    }
    
    return m_instance;
}

#endif /* defined(__Giraffe__Timer__) */
