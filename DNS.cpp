//#include "stdafx.h"

#include <regex> 
#include "DNS.h"


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
	std::string server_url = DNS + domain + ID;
	CURLcode res;
	curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
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

	if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0) IP = "";
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

void Time_sleep(unsigned long time_ms)
{
#ifndef _WIN32
	usleep(1000*time_ms);
#else
	Sleep(1*time_ms);
#endif	
}

#ifdef WIN32
	pthread_t pod_ID ,normal_ID;
#else
	pthread_t pod_ID = 0; 
	pthread_t normal_ID = 0;
	struct sigaction actions;
#endif
#ifndef WIN32
	//信号处理函数
void thread_exit_handler(int sig)
{
	if(sig == SIGUSR1)
	{
		p_Timelog->tprintf("tid = %ld exit\n", pthread_self());
		pthread_exit(0);
	}
}
#endif

static void *Get_IP_pod(void* parameter)
{
#ifdef _WIN32
		clock_t st = clock();
#else
		struct timeval stv;
		gettimeofday(&stv, NULL);
#endif
	p_Timelog->tprintf("[Get_IP_pod]tid = %ld\n",pthread_self());
	ip_param* thread_param = (ip_param*) parameter;
	thread_param->b_start = true;

#ifdef WIN32
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    /*异步取消， 线程接到取消信号后，立即退出 PTHREAD_CANCEL_ASYNCHRONOUS:0*/
	//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL); 
	//由于需要用到pthread_cleanup_push/pthread_cleanup_pop, PTHREAD_CANCEL_DEFERRED模式
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); 
#else   
	sigemptyset(&actions.sa_mask);  
	actions.sa_flags = 0;   
	actions.sa_handler = thread_exit_handler;  
	sigaction(SIGUSR1,&actions,NULL);  
#endif

	std::string IP = "";
	std::string data="";
	char* ipstr = nullptr;
	CURL *curl = NULL;
#ifdef WIN32
	pthread_cleanup_push(curl_easy_cleanup,(void*) curl);
#endif
	 curl = curl_easy_init();
#ifdef WIN32
	pthread_testcancel();
	std::string domain = Get_Domain((void*)thread_param->url.c_str());
	pthread_testcancel();
#else
	std::string domain = Get_Domain((void*)thread_param->url.c_str());
#endif
	std::string server_url = DNS + domain + ID;// ;
	CURLcode res;
	curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);   
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT ,2L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToString);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
#ifdef WIN32
	pthread_testcancel();
	res = curl_easy_perform(curl);
	pthread_testcancel();
#else 
	res = curl_easy_perform(curl);
#endif
	int ip[4]={0};
	if (res == CURLE_OK)
	{
		sscanf(data.c_str(), "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	}

	if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0) IP = "";
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
		p_Timelog->tprintf("[Get_IP_pod]IP=%s\n", IP.c_str());
	}

	curl_easy_cleanup(curl);
#ifdef WIN32
	pthread_cleanup_pop(0);
#endif

	if (IP == "")
	{
		thread_param->b_status = UNSUCCEEDED;
		goto label;
	}
	{
		thread_param->IP = IP;
		thread_param->b_status = SUCCEEDED;

		int kill_rc = pthread_kill(normal_ID,0);
		if(kill_rc == 3)
			p_Timelog->tprintf("[Get_IP_pod]the normal_ID thread did not exists or already quit\n");
		else if(kill_rc == 22)
			p_Timelog->tprintf("[Get_IP_pod]signal is invalid\n");
		else
		{
			p_Timelog->tprintf("[Get_IP_pod]the normal_ID thread is alive;Do pthread_cancel\n");
#ifdef WIN32
		pthread_cancel(normal_ID);//使另外一个线程退出
#else
		if (pthread_kill(normal_ID, SIGUSR1) != 0) 
		{ 
			p_Timelog->tprintf("[Get_IP_pod]Error cancelling pod_ID thread : %d", normal_ID);
		} 
#endif
		}
	}
label:
	p_Timelog->tprintf("[Get_IP_pod]tid = %ld exit\n",pthread_self());
#ifdef _WIN32
	p_Timelog->EndTimeLog("[Get_IP_pod]Get_IP_pod End Time[%d]\n",st);	
#else
	p_Timelog->EndTimeLog("[Get_IP_pod]Get_IP_pod End Time[%d]\n",stv);
#endif
	return NULL;
}

static void *Get_IP_normal(void* parameter)
{
#ifdef _WIN32
		clock_t st = clock();
#else
		struct timeval stv;
		gettimeofday(&stv, NULL);
#endif

	p_Timelog->tprintf("[Get_IP_normal]tid = %ld\n",pthread_self());
	ip_param* thread_param = (ip_param*) parameter;
	thread_param->b_start = true;

#ifdef WIN32
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    /*异步取消， 线程接到取消信号后，立即退出 PTHREAD_CANCEL_ASYNCHRONOUS:0*/
    //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	//由于需要用到pthread_cleanup_push/pthread_cleanup_pop, PTHREAD_CANCEL_DEFERRED模式
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); 
#else
	sigemptyset(&actions.sa_mask);  
	actions.sa_flags = 0;   
	actions.sa_handler = thread_exit_handler;  
	sigaction(SIGUSR1,&actions,NULL);  
#endif

	char ip[16] = {0};
#ifdef WIN32
	pthread_testcancel();
	std::string domain = Get_Domain((void*)thread_param->url.c_str());
	pthread_testcancel();
#else
	std::string domain = Get_Domain((void*)thread_param->url.c_str());
#endif 

	const char *strDomain2Resolve = domain.c_str();

	int nStatus;
#ifdef WIN32
	// 初始化 Winsock
    WSADATA wsaData;
	pthread_cleanup_push(WSACleanup,NULL);
    nStatus = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (NO_ERROR != nStatus)
    {
		thread_param->b_status = UNSUCCEEDED;
		goto lable;
    }
#endif
	
    addrinfo Hints, *AddrList = NULL;
    memset(&Hints, 0, sizeof(Hints));
	Hints.ai_family = AF_INET;
	Hints.ai_flags = AI_PASSIVE;
	Hints.ai_protocol = 0;
	Hints.ai_socktype = SOCK_STREAM;

	{
#ifdef WIN32
	 // 进行域名解析
	pthread_cleanup_push(freeaddrinfo,(void*)AddrList);
	pthread_testcancel();
    nStatus = getaddrinfo(strDomain2Resolve, NULL, &Hints, &AddrList);
	pthread_testcancel();
#else
	 nStatus = getaddrinfo(strDomain2Resolve, NULL, &Hints, &AddrList);
#endif
    if (0 != nStatus)
    {
        // 处理出错的情况，
		//p_Timelog->tprintf("[Get_IP_normal]getaddrinfo() failed %d: %s\n",nStatus, gai_strerror(nStatus));
		p_Timelog->tprintf("[Get_IP_normal]getaddrinfo() failed %d\n",nStatus);
		thread_param->b_status = UNSUCCEEDED;
		goto lable;
    }
	{
	char pBuf[64] = {0};  // 打印缓冲

    // 一般来说，客户端程序使用解析出的 IP 列表中的第一个即可
    // （如果解析出多个地址的话）
	if (AddrList->ai_family == AF_INET)
	{
		sockaddr_in *addr = (sockaddr_in *)AddrList->ai_addr;
		inet_ntop(AF_INET, &addr->sin_addr, pBuf, 64);
		p_Timelog->tprintf("[Get_IP_normal]IP=%s\n", pBuf);
	}else if(AddrList->ai_family == AF_INET6)
	{
		sockaddr_in6 *addr6 = (sockaddr_in6 *)AddrList->ai_addr;
		inet_ntop(AF_INET6, &addr6->sin6_addr, pBuf, 64);
		p_Timelog->tprintf("[Get_IP_normal]IP=%s\n", pBuf);
	}

	thread_param->IP = pBuf;
	thread_param->b_status = SUCCEEDED;

	int kill_rc = pthread_kill(pod_ID,0);
	if(kill_rc == 3)
		p_Timelog->tprintf("[Get_IP_normal]the pod_ID thread did not exists or already quit\n");
	else if(kill_rc == 22)
		p_Timelog->tprintf("[Get_IP_normal]signal is invalid\n");
	else
	{
		p_Timelog->tprintf("[Get_IP_normal]the pod_ID thread is alive;Do pthread_cancel\n");
#ifdef WIN32
		pthread_cancel(pod_ID);//使另外一个线程退出
#else
		if (pthread_kill(pod_ID, SIGUSR1) != 0) 
		{ 
			p_Timelog->tprintf("[Get_IP_normal]Error cancelling pod_ID thread : %d", pod_ID);
		} 
#endif
	}
	
	}
	}

lable:
	freeaddrinfo(AddrList);

#ifdef WIN32
	pthread_cleanup_pop(0);
    WSACleanup();
	pthread_cleanup_pop(0);
#endif

	p_Timelog->tprintf("[Get_IP_normal]tid = %ld exit\n",pthread_self());
#ifdef _WIN32
	p_Timelog->EndTimeLog("[Get_IP_normal]Get_IP_normal End Time[%d]\n",st);	
#else
	p_Timelog->EndTimeLog("[Get_IP_normal]Get_IP_normal End Time[%d]\n",stv);
#endif

	return NULL;
}

std::string Parsing_IP(const char* url)
{
#ifdef _WIN32
		clock_t st = clock();
#else
		struct timeval stv;
		gettimeofday(&stv, NULL);
#endif
	p_Timelog->tprintf("[Parsing_IP]Parsing_IP Start Time\n");
	std::string IP = "";
	ip_param pod_ip,normal_ip;
	pod_ip.b_start = false;
	pod_ip.url = url;
	normal_ip.b_start = false;
	normal_ip.url = url;
	pod_ip.b_status = INIT_STATUS;
	normal_ip.b_status = INIT_STATUS;
	pod_ip.IP = "";
	normal_ip.IP = "";

	//
#ifndef WIN32
	memset(&actions, 0, sizeof(actions));
	pthread_t tmp_id = 0;	
	if (pod_ID != 0)
	{
		tmp_id = pod_ID;
		pthread_kill(pod_ID,SIGTERM);
		pthread_join(tmp_id,NULL);
	}
	if (normal_ID != 0)
	{
		tmp_id = normal_ID;
		pthread_kill(normal_ID,SIGTERM);
		pthread_join(tmp_id,NULL);
	}
#else

#endif
	///////////////////智能DNS解析线程////////////////////////////////
	int ret = pthread_create(&pod_ID, NULL, Get_IP_pod, (void*)&pod_ip);
	if (ret != 0)
	{
		pod_ip.b_status == UNSUCCEEDED;
		pod_ip.IP = "";
	}
	else
	{
		while (!pod_ip.b_start)
		{
			Time_sleep(1);
		}
	}
	
	//////////////////普通方法解析线程///////////////////////////////
	ret = pthread_create(&normal_ID, NULL, Get_IP_normal, (void*)&normal_ip);
	if (ret != 0)
	{
		normal_ip.b_status == UNSUCCEEDED;
		normal_ip.IP = "";
	}
	else
	{
		while (!normal_ip.b_start)
		{
			Time_sleep(1);
		}
	}
	
	pthread_join(pod_ID,NULL);
	pthread_join(normal_ID,NULL);
	//等待IP解析，如果两线程中IP解析都失败，退出，返回""
	while((pod_ip.b_status == INIT_STATUS || pod_ip.b_status == UNSUCCEEDED) && 
		(normal_ip.b_status == INIT_STATUS || normal_ip.b_status == UNSUCCEEDED))
	{
		if (pod_ip.b_status == UNSUCCEEDED && normal_ip.b_status == UNSUCCEEDED)
		{
				IP = "";
				goto label;
		}
		Time_sleep(1);	
	}
	
	if (pod_ip.b_status == SUCCEEDED)
	{
		IP = pod_ip.IP;
	}
	else if(normal_ip.b_status == SUCCEEDED)
	{
		IP = normal_ip.IP ;	
	}
	


label:
#ifdef _WIN32
	p_Timelog->EndTimeLog("[Parsing_IP]Parsing_IP End Time[%d]\n",st);	
#else
	p_Timelog->EndTimeLog("[Parsing_IP]Parsing_IP End Time[%d]\n",stv);
#endif
	return IP;
}


