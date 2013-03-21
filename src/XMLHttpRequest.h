//
//  XMLHttpRequest.h
//  Giraffe
//
//  Created by kins on 2/6/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#ifndef __Giraffe__XMLHttpRequest__
#define __Giraffe__XMLHttpRequest__

#include <string>
#include <map>
#include "JSCContext.h"

using std::string;
using std::map;

typedef enum{
    HttpStateUninitialized  = 0,
    HttpStateOpen           = 1,
    HttpStateSend           = 2,
    HttpStateReceiving      = 3,
    HttpStateLoaded         = 4
} HttpState;

class XMLHttpRequest
{
private:
    bool m_post;
    string m_url;
    bool m_async;
    map<string, string> m_headers;
    
public:
    int status;
    int readyState;
    string responseText;
    
    JSObject *onreadystatechange;
    JSObject *jsthis;
    
public:
    XMLHttpRequest();
    ~XMLHttpRequest();
    
    JS_CLASS_EXPORT_DEF
    
    void open(const string &method, const string &url, bool async=true);
    void send(const string &postData);
    void setRequestHeader(const string &key, const string &value);
};

#endif /* defined(__Giraffe__XMLHttpRequest__) */
