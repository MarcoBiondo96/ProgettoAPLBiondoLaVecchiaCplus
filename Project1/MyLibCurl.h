#pragma once
#include <string>
#include "crow.h"
#include <curl\curl.h>
#include "rapidjson/document.h"
#define CURL_STATICLIB
namespace MyLibCurl {
		
	void SendPythonRequest(crow::websocket::connection& conn,const std::string& document);
	size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data);
	void replaceSpaces(std::string& str);
	std::string gamesreturnValue(const std::string& url);
}