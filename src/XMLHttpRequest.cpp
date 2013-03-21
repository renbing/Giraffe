//
//  XMLHttpRequest.cpp
//  Giraffe
//
//  Created by kins on 2/6/13.
//  Copyright (c) 2013 kins. All rights reserved.
//

#include "stringutil.h"
#include "Http.h"
#include "XMLHttpRequest.h"


JS_PROPERTY_READONLY(XMLHttpRequest, INT, status)
JS_PROPERTY_READONLY(XMLHttpRequest, INT, readyState)
JS_PROPERTY_READONLY(XMLHttpRequest, STRING, responseText)

JS_PROPERTY_SLOT(XMLHttpRequest, OBJECT, onreadystatechange)

JS_FUNC_WITH_ARG3(XMLHttpRequest, open, VOID, STRING, STRING, BOOL)
JS_FUNC_WITH_ARG1(XMLHttpRequest, send, VOID, STRING)
JS_FUNC_WITH_ARG2(XMLHttpRequest, setRequestHeader, VOID, STRING, STRING)

JSPropertySpec XMLHttpRequest::js_ps[] = {
    JS_PROPERTY_READONLY_DEF(XMLHttpRequest, status),
    JS_PROPERTY_READONLY_DEF(XMLHttpRequest, readyState),
    JS_PROPERTY_READONLY_DEF(XMLHttpRequest, responseText),
    JS_PROPERTY_DEF(XMLHttpRequest, onreadystatechange),
    JS_PS_END
};

JSFunctionSpec XMLHttpRequest::js_fs[] = {
    JS_FN_DEF(XMLHttpRequest, open, 0),
    JS_FN_DEF(XMLHttpRequest, send, 1),
    JS_FN_DEF(XMLHttpRequest, setRequestHeader, 2),
    JS_FS_END
};


JSPropertySpec XMLHttpRequest::js_sps[] = {
    JS_PS_END
};
JSFunctionSpec XMLHttpRequest::js_sfs[] = {
    JS_FS_END
};

JS_CLASS_EXPORT(XMLHttpRequest)

static void xmlHttpRequestCallback(Downloader *downloader, void *arg)
{
	XMLHttpRequest *request = (XMLHttpRequest *)arg;
	request->readyState = HttpStateLoaded;
    if( downloader->getCode() != CURLE_OK) {
        // 如果出错,把curl的错误码赋值给Http状态码
        request->status = downloader->getCode();
    }else {
        request->status = downloader->getStatus();
    }
	if( request->status == 200 )
	{
		request->responseText = downloader->getText();
	}
    
    JSCContext::getInstance()->callJSFunction(request->onreadystatechange);
}

XMLHttpRequest::XMLHttpRequest() : onreadystatechange(NULL)
{
	readyState = HttpStateUninitialized;
	status = -1;
    
	m_post = false;
	m_async = true;
}

XMLHttpRequest::~XMLHttpRequest()
{
}

void XMLHttpRequest::open(const string &method, const string &url, bool async)
{
	if( StringUtil::upper(method) == "POST" )
	{
		m_post = true;
	}
    if( StringUtil::startWith(url, "http://")) {
        m_url = url;
    }else {
        m_url = JSCContext::getInstance()->path() + url;
    }
    
	m_async = async;
}

void XMLHttpRequest::send(const string &postData)
{
    if(m_post) {
        Http::getInstance()->Post(m_url, postData, xmlHttpRequestCallback, this, m_async, &m_headers);
    }else {
        Http::getInstance()->Get(m_url, xmlHttpRequestCallback, this, m_async, &m_headers);
    }
}

void XMLHttpRequest::setRequestHeader(const string &key, const string &value)
{
	m_headers[key] = value;
}