#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>
#include <cstdlib>



std::string Get_IP(void* url);
std::string Get_Port(void* url);
std::string DNS_pod(void* url);

void set_share_handle(CURL* curl_handle);

	

