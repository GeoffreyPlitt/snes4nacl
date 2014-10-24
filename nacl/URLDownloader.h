/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#ifndef _SNES4NACL_URLDOWNLOADER_H
#define _SNES4NACL_URLDOWNLOADER_H

#include <vector>
#include <string>
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/url_loader.h"
#include "ppapi/cpp/url_request_info.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/utility/completion_callback_factory.h"

namespace snes4nacl {

class URLDownloader
{
public:
	void start();
	virtual ~URLDownloader();
protected:
	URLDownloader(pp::Instance* instance, const std::string& url);
	
	
	
	virtual void onFinish(int httpCode, const unsigned char* data, int size) = 0;
private:
	pp::Instance* instance;  // Weak pointer.
	std::string url;  // URL to be downloaded.
	pp::URLRequestInfo url_request;
	pp::URLLoader url_loader;  // URLLoader provides an API to download URLs.
	unsigned char buffer[1024 * 32];  // Temporary buffer for reads.
	std::vector<unsigned char> url_response_body;  // Contains accumulated downloaded data.
	pp::CompletionCallbackFactory<URLDownloader> cc_factory;
	
	void onOpen(int result);
	void onRead(int result);
	void append(const unsigned char* data, int numBytes);
	void readBody();
};

}

#endif

