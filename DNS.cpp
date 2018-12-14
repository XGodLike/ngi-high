#include "stdafx.h"
#include <regex> 
#include "DNS.h"

#define DNS "http://119.29.29.29/d?dn="

 size_t writeToString(void *ptr, size_t size, size_t count, void *stream)
{
     ((std::string*)stream)->append((char*)ptr, 0, size* count);
     return size* count;
}

std::string Get_IP(void* url)
 {
	 std::string tmp_url = (char*)url;
	 char domain[64] = {0};
	 sscanf(tmp_url.c_str(), "http://%[a-zA-Z0-9\.][^:]:%*", domain);
	 for (int i = 0; i < strlen(domain); i++)
	 {
	  if ((domain[i] < '0' || domain[i] > '9')&& domain[i] != '.')
	  {
	 	 return "";
	  }
	 }
	 return domain;
 }

 std::string Get_Port(void* url)
 {
	 std::string tmp_url = (char*)url;
	 char port[64] = {0};
	 sscanf(tmp_url.c_str(), "http://%*[^:]:%[0-9]s", port);
	 if(port[0] == 0)
		 strcpy(port,"80");
	 return port;
 }

 std::string Get_Domain(void* url)
 {
	 std::string tmp_url = (char*)url;
	 char domain[64] = {0};
	 sscanf(tmp_url.c_str(), "http://%[a-zA-Z0-9\.][^:]:%*", domain);
	 return domain;
 }

std::string DNS_pod(void *url)
{	
	std::string IP = Get_IP(url);
	if (IP != "")
	{
		return IP;
	}

	std::string data="";
	char* ipstr = nullptr;
	CURL *curl = curl_easy_init();
	std::string domain = Get_Domain(url);
	std::string server_url = DNS + domain;
	CURLcode res = curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);   
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT ,2L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToString);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
	res = curl_easy_perform(curl);
	int ip[4]={0};
	if (res == CURLE_OK)
	{
		sscanf(data.c_str(), "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	}

	if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0) IP = domain;
	else 
	{
		char strip[16]={0};
		for (int i = 0; i < 3; i++)
		{
			sprintf(strip, "%d", *(ip+i)); 
			IP.append(strip);
			IP.append(".");
		}
		sprintf(strip, "%d", *(ip+3)); 
		IP.append(strip);
	}

	curl_easy_cleanup(curl);
	return IP;
}

void set_share_handle(CURL* curl_handle)
{
	static CURLSH* share_handle = NULL;
	if(!share_handle)
	{
		share_handle = curl_share_init();
		curl_share_setopt(share_handle,CURLSHOPT_SHARE,CURL_LOCK_DATA_DNS);

	}
	curl_easy_setopt(curl_handle,CURLOPT_SHARE,share_handle);
	curl_easy_setopt(curl_handle,CURLOPT_DNS_CACHE_TIMEOUT,60*5);
}

