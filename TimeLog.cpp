#include "stdafx.h"
#include "TimeLog.h"
#include <time.h>
#include <stdio.h>
#include <string>
#include "stdlib.h"

#ifndef _WIN32
#include <android/log.h>

#define  LOG_TAG    "VD"  

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__) 
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)
#endif


static clock_t tTimer = 0;
static struct timeval stv;

CTimeLog::CTimeLog(void) :offset(0)
{
	offset = GetMsOffset();
}

CTimeLog::~CTimeLog(void)
{

}

void CTimeLog::tprintf(const char * format, ...)
{
	char buf[1024] = {0};

	{
		struct timeval tv;
		struct tm* pt ;
		char sTime[32] = {0};
		time_t st;
#ifdef _WIN32
		tv.tv_sec = time(0);
		tv.tv_usec = GetSysMs(offset);	
#else
		gettimeofday(&tv, NULL);
		tv.tv_usec /= 1000;
#endif
		st = tv.tv_sec;
		pt = localtime(&st);	

		strftime(sTime, 32, "[%Y-%m-%d %H:%M:%S.%%03d]", pt);
		sprintf(buf, sTime, tv.tv_usec);
	}

	{
		va_list args;
		va_start(args, format);
		vsprintf(buf + 25, format, args);
		va_end(args);
	}

#ifdef _WIN32
		printf(buf);
#else
		//__android_log_print(ANDROID_LOG_INFO  , "VD", buf);
		LOGD(buf);
#endif
}

void CTimeLog::PrintTimeLog(const char* logStr)
{
	char tbuffer[64] = { 0 };
	char chtime[64] = { 0 };
	time_t t = time(NULL);
	strftime(tbuffer, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));
	int SysMs = GetSysMs(offset);
	sprintf(chtime, "%s.%03d", tbuffer,SysMs);

	printf(logStr, chtime);
}

int CTimeLog::GetMsOffset()
{
	clock_t c =  clock();
	time_t tPre, tCur;
	tPre = time(NULL);
	for (tCur = time(NULL); tCur - tPre == 0; tPre = tCur, tCur = time(NULL), c = clock());
	return c;
}

int CTimeLog::GetSysMs(int offset)
{
	div_t tv = div((int)clock() - offset,CLOCKS_PER_SEC);
	return tv.rem;
}

//void CTimeLog::StartTime()
//{
//#ifdef _WIN32
//	tTimer = clock();
//#else
//	gettimeofday(&stv, NULL);
//#endif
//}
//
//void CTimeLog::EndTimeLog(const char * format)
//{
//#ifdef _WIN32
//		tprintf(format,clock()-tTimer);
//#else
//		struct timeval etv;
//		gettimeofday(&etv, NULL);
//		tprintf(format,(etv.tv_sec - stv.tv_sec)*1000 + (etv.tv_usec - stv.tv_usec)/1000);
//#endif	
//}

#ifdef _WIN32

void CTimeLog::EndTimeLog(const char * format,clock_t st)
{
	tprintf(format,clock()-st);
}
#else
void CTimeLog::EndTimeLog(const char * format,timeval stv)
{
	struct timeval etv;
	gettimeofday(&etv, NULL);
	tprintf(format,(etv.tv_sec - stv.tv_sec)*1000 + (etv.tv_usec - stv.tv_usec)/1000);
}
#endif	