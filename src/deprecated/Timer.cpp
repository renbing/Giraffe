//
//  TimerManager.cpp
//  Giraffe
//
//  Created by kins on 1/15/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#include "Timer.h"

TimerManager * TimerManager::m_instance = NULL;

void TimerManager::setTimeout(TimerCallback callback, float second, void *arg)
{
    Timer timer = {callback, second, 0, false, arg};
    m_timers[++m_counter] = timer;
}

int TimerManager::setInterval(TimerCallback callback, float second, void *arg)
{
    Timer timer = {callback, second, 0, true, arg};
    m_timers[++m_counter] = timer;
    
    return m_counter;
}

void TimerManager::clearInterval(int timerId)
{
    m_timers.erase(timerId);
}

void TimerManager::update(float passed)
{
    vector<int> overdue;
    
    map<int, Timer>::iterator it = m_timers.begin();
    for( ; it != m_timers.end(); it++ ) {
        Timer &timer = it->second;
        timer.passed += passed;
        if( timer.passed >= timer.second ) {
            if( timer.callback ) {
                timer.callback(timer.arg);
            }
            if( !timer.repeat ) {
                overdue.push_back(it->first);
            }
            
            timer.passed -= timer.second;
        }
    }
    
    for( int i=0; i<overdue.size(); i++ ) {
        m_timers.erase(overdue[i]);
    }
}