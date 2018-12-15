#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include <signal.h>
#ifndef WIN32
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <signal.h>
	#include <unistd.h>
	#include <pthread.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <err.h>
	#include <sys/ioctl.h>
	#include <sys/socket.h>
	#include <netdb.h>
#else
	#include <vld.h>
	#include "winsock2.h"
	#include "Ws2tcpip.h"
	#pragma comment(lib, "Ws2_32.lib")  
#endif

//http://119.29.29.29/d?dn=[域名加密后的字符串]&id=[授权ID]&ttl=1
#define DNS "http://119.29.29.29/d?dn="
#define ID "&id=7191"


std::string Get_IP(void* url);
std::string Get_Port(void* url);
std::string Get_Domain(void* url);
std::string DNS_pod(void* url);
 size_t writeToString(void *ptr, size_t size, size_t count, void *stream);
//int Get_IP_normal(char* IP, const char* url);
std::string Parsing_IP(const char *url);
void set_share_handle(CURL* curl_handle);
void Time_sleep(unsigned long time_ms);


//用于让主线程判断是那个线程较早的获取了IP
typedef enum STATUS {
	INIT_STATUS = 0,
	SUCCEEDED,//是否获取IP
	UNSUCCEEDED//获取IP失败
}dns_status;

typedef struct Thread_IP
{
	bool b_start; //线程是否启动
	dns_status b_status; //当前线程获取IP的状态
	std::string url;
	std::string IP;	
}ip_param;