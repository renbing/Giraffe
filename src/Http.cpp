//
//  Http.cpp
//  Giraffe
//
//  Created by kins on 2/5/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#include "Http.h"

static int writer(void *data, size_t size, size_t nmemb, Downloader *downloader)
{
    if( downloader )
    {
        downloader->didReceiveData(data, size*nmemb);
    }
    
    return size * nmemb;
}

Downloader::Downloader(CURL *curl, HttpCallback callback, void *callbackArg)
{
    m_curl = curl;
    m_bufSize = 100 * 1024;
    m_buf = malloc(m_bufSize);
    m_recievedLen = 0;
    m_status = -1;
    m_code = CURL_LAST;
    m_callback = callback;
    m_callbackArg = callbackArg;
    m_headers = NULL;
}

Downloader::~Downloader()
{
    if( m_headers ) {
        curl_slist_free_all(m_headers);
        m_headers = NULL;
    }
    
    curl_easy_cleanup(m_curl);
    free(m_buf);
    m_buf = NULL;
}

void Downloader::didReceiveData(void *data, size_t len)
{
    // 多一个字节来处理string结尾字符
    if( m_recievedLen + len  >= m_bufSize )
    {
        // 超出缓冲区大小,增加缓冲区100K
        size_t newBufSize = m_bufSize + 100 * 1024;
        void *newBuf = realloc(m_buf, newBufSize);
        
        m_buf = newBuf;
        m_bufSize = newBufSize;
    }
    
    memcpy((char *)m_buf+m_recievedLen, data, len);
    m_recievedLen += len;
    
    *((char *)m_buf+m_recievedLen) = 0;
}

void Downloader::didFinish(CURLcode code, int status)
{
    m_code = code;
    m_status = status;
    
    if( m_callback ) {
        m_callback(this, m_callbackArg);
    }
}

Http * Http::m_instance = NULL;

Http::Http()
{
    m_mcurl = curl_multi_init();
    
    m_timeout.tv_sec = 0;
    m_timeout.tv_usec = 5000;
}

Http::~Http()
{
    curl_multi_cleanup(m_mcurl);
}

void Http::Get(const string &url, HttpCallback callback, void *callbackArg, bool async,
               map<string, string> *header, int timeout)
{
    Request(url, async, false, NULL, 0, callback, callbackArg, header, timeout);
}

void Http::Post(const string &url, const string &data, HttpCallback callback,
                void * callbackArg, bool async, map<string, string> *header, int timeout)
{
    Request(url, async, true, data.c_str(), data.size(), callback, callbackArg, header, timeout);
}

void Http::Post(const string &url, const map<string, string> &form, HttpCallback callback,
                void *callbackArg, bool async, map<string, string> *header, int timeout)
{
    string data;
    map<string,string>::const_iterator it = form.begin();
    for(; it != form.end(); it++ ) {
        if( !data.empty() ) {
            data += "&";
        }
        data += it->first + "=" + it->second;
    }
    Request(url, async, true, data.c_str(), data.size(), callback, callbackArg, header, timeout);
}

void Http::Request(const string &url, bool async, bool usePost, const void *data, size_t dataLen, HttpCallback callback,
                   void *callbackArg, map<std::string, string> *header, int timeout)
{
    if( timeout <= 0 ) {
        timeout = 60;
    }
    
    CURL *handle = curl_easy_init();
    Downloader *downloader = new Downloader(handle, callback, callbackArg);
    
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writer);
    curl_easy_setopt(handle, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, downloader);
    curl_easy_setopt(handle, CURLOPT_PRIVATE, downloader);
    if( usePost ){
        curl_easy_setopt(handle, CURLOPT_POST, 1);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, dataLen);
        curl_easy_setopt(handle, CURLOPT_COPYPOSTFIELDS, data);
    }
    if( header != NULL ) {
        struct curl_slist *headers = NULL;
        map<string,string>::const_iterator it = header->begin();
        for(; it != header->end(); it++ ) {
            headers = curl_slist_append(headers, (it->first + ": " + it->second).c_str());
        }
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        downloader->setHeaders(headers);
    }
    
    if( async ) {
        curl_multi_add_handle(m_mcurl, handle);
        int running = 0;
        curl_multi_perform(m_mcurl, &running);
    }else {
        int repCode = -1;
        CURLcode code = curl_easy_perform(handle);
        if ( CURLE_OK == code )
        {
            curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &repCode);
        }
        
        downloader->didFinish(code, repCode);
        delete  downloader;
    }
}

void Http::update()
{
    int running = 0;
    curl_multi_perform(m_mcurl, &running);
    
    CURLMsg *msg;
    int msgs_left;
    while ((msg = curl_multi_info_read(m_mcurl, &msgs_left))) {
        if(msg->msg == CURLMSG_DONE) {
            int repCode = -1;
            Downloader *downloader = NULL;
            
            curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &repCode);
            curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &downloader);
            curl_multi_remove_handle(m_mcurl, msg->easy_handle);
            
            downloader->didFinish(msg->data.result, repCode);
            delete downloader;
        }
    }
}

int Http::Get(const string &url, string &content, map<std::string, string> *header, int timeout)
{
    if( timeout <= 0 ) {
        timeout = 60;
    }
    
    CURL *handle = curl_easy_init();
    Downloader *downloader = new Downloader(handle);
    
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writer);
    curl_easy_setopt(handle, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, downloader);
    curl_easy_setopt(handle, CURLOPT_PRIVATE, downloader);
    if( header != NULL ) {
        struct curl_slist *headers = NULL;
        map<string,string>::const_iterator it = header->begin();
        for(; it != header->end(); it++ ) {
            headers = curl_slist_append(headers, (it->first + ": " + it->second).c_str());
        }
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        downloader->setHeaders(headers);
    }
    
    int repCode = -1;
    CURLcode code = curl_easy_perform(handle);
    if ( CURLE_OK == code )
    {
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &repCode);
    }
    
    int downloaded = -1;
    
    if( repCode == 200 ) {
        content = downloader->getText();
        downloaded = content.size();
    }
    
    delete  downloader;
    
    return downloaded;
}
