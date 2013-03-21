//
//  Director.h
//  Giraffe
//
//  Created by kins on 1/15/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#ifndef __Giraffe__Director__
#define __Giraffe__Director__

#include <time.h>
#include <sys/time.h>

class Director
{
private:
    static struct timeval m_pasttv;
public:
    static void init();
    static void run();
    
};

#endif /* defined(__Giraffe__Director__) */
