#pragma once

#include "VcyberVoiceSDK.h"
#include <sstream>
#include <curl/curl.h>
#include <json/json.h>
#include "Configs.h"


bool GetUserInfo(std::string & ciphertext, Configs *con, SessionParam *sp);

std::string url_encode(std::string data);
Json::Value get_data(std::vector<char*> &vc);
std::string get_cookie(std::vector<char*> &vc);
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
size_t get_header( void *ptr, size_t size,size_t nmemb, void *stream);
size_t Debug(void *curl, int nType, char * pData, size_t size, void *);
std::string get_data(SessionParam *sp, Configs *config, const char* method, const char* audio, eAudioStatus dataStatus);


Json::Value get_data_str(std::string &vc);
std::string get_cookie_str(std::string &vc);
size_t write_data_str(void *ptr, size_t size, size_t nmemb, void *stream);
size_t get_header_str( void *ptr, size_t size,size_t nmemb, void *stream);