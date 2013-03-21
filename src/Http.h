//
//  Http.h
//  Giraffe
//
//  Created by kins on 2/5/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#ifndef __Giraffe__Http__
#define __Giraffe__Http__

#include <curl.h>
#include <string>
#include <map>

using std::string;
using std::map;

class Downloader;

typedef void (*HttpCallback)(Downloader *downloader, void *arg);

typedef struct
{
    HttpCallback callback;
}HttpCallbackWrapper;

class Downloader
{
private:
    void                *m_buf;
    size_t              m_bufSize;
    size_t              m_recievedLen;
    
    CURL                *m_curl;
    HttpCallback        m_callback;
    void                *m_callbackArg;
    CURLcode            m_code;
    int                 m_status;
    struct curl_slist   *m_headers;

public:
    Downloader(CURL *curl, HttpCallback callback = NULL, void *callbackArg=NULL);
    ~Downloader();
    
    void setHeaders(struct curl_slist *headers) { m_headers = headers;};
    
    const char * getText() { return (char *)m_buf; }
    void * getData() { return m_buf; };
    size_t getDataLen() { return m_recievedLen; };
    
    CURLcode getCode() { return m_code; };
    int getStatus() { return m_status; };
    
    void didReceiveData(void *data, size_t len);
    void didFinish(CURLcode code, int status);
};

class Http
{
private:
    CURLM *m_mcurl;
    struct timeval m_timeout;
    
private:
    Http();
    static Http *m_instance;
    
public:
    ~Http();
    static Http *getInstance();
    
    int  Get(const string &url, string &content, map<string,string> *header = NULL, int timeout=0);
    void Get(const string &url, HttpCallback callback = NULL, void *callbackArg = NULL, bool async=true,
              map<string, string> *header = NULL, int timeout=0);
    void Post(const string &url, const string &data, HttpCallback callback = NULL, void *callbackArg = NULL,
              bool async=true, map<string,string> *header=NULL, int timeout=0);
    void Post(const string &url, const map<string, string> &form, HttpCallback callback=NULL,
              void *callbackArg = NULL, bool async=true, map<string,string> *header=NULL, int timeout=0);
    
    void update();

private:
    void Request(const string &url, bool async, bool usePost, const void *data, size_t dataLen, HttpCallback callback,
                 void *callbackArg, map<std::string, string> *header, int timeout);
};


inline Http * Http::getInstance()
{
    if( m_instance == NULL ) {
        m_instance = new Http();
    }
    
    return m_instance;
}

#endif /* defined(__Giraffe__Http__) */
