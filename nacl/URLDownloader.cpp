/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#include "URLDownloader.h"
#include "ppapi/cpp/url_response_info.h"
#include "Instance.h"

#define READ_BUFFER_SIZE 1024 * 32

namespace snes4nacl {

URLDownloader::URLDownloader(pp::Instance* instance, const std::string& _url) :  url(_url), url_request(instance), url_loader(instance), cc_factory(this)
{
	this->instance = instance;
	url_request.SetURL(url);
	url_request.SetMethod("GET");
	url_request.SetRecordDownloadProgress(true);
}

URLDownloader::~URLDownloader()
{
	
}

void URLDownloader::start()
{
	printf("URLDownloader::start() -- Fectching %s", url.c_str());
	pp::CompletionCallback cc = cc_factory.NewCallback(&URLDownloader::onOpen);
	url_loader.Open(url_request, cc);
}

void URLDownloader::onOpen(int result)
{
	if(result != PP_OK)
	{
		onFinish(0, NULL, 0);
		printf("URLDownloader::onOpen() -- Error opening URL (%d)", result);
		return;
	}
		
	pp::URLResponseInfo response = url_loader.GetResponseInfo();
	int httpResponseCode = response.GetStatusCode();
	if(httpResponseCode != 200)
	{
		onFinish(httpResponseCode, NULL, 0);
		printf("URLDownloader::onOpen() -- Error opening URL (%d)", httpResponseCode);
		return;
	}
	
	int64_t bytes_received = 0;
	int64_t total_bytes_to_be_received = 0;
	if (url_loader.GetDownloadProgress(&bytes_received,	&total_bytes_to_be_received)) 
	{
		if (total_bytes_to_be_received > 0) 
			url_response_body.reserve(total_bytes_to_be_received);
	}
	
	url_request.SetRecordDownloadProgress(false);
	readBody();
}

void URLDownloader::readBody()
{
	pp::CompletionCallback cc = cc_factory.NewOptionalCallback(&URLDownloader::onRead);
	int32_t result = PP_OK;
	do {
		result = url_loader.ReadResponseBody(buffer, READ_BUFFER_SIZE, cc);
		if (result > 0) 
			append(buffer, result);
	} while (result > 0);
	if (result != PP_OK_COMPLETIONPENDING) 
		cc.Run(result);
}

void URLDownloader::append(const unsigned char* data, int numBytes)
{
	if (numBytes <= 0)
		return;
	numBytes = std::min(READ_BUFFER_SIZE, numBytes);
	url_response_body.insert(url_response_body.end(), data, data + numBytes);
}

void URLDownloader::onRead(int result)
{
	if (result == PP_OK) 
	{
		printf("URLDownloader::onRead() -- Download finished");
		onFinish(200, &url_response_body.front(), url_response_body.size());
	}
	else if (result > 0) 
	{
		append(buffer, result);
		readBody();
	} 
	else 
	{
		printf("URLDownloader::onRead() -- Error reading (%d)", result);
		onFinish(0, NULL, 0);
	}
}

}

