
#ifndef WIN32
//#include "com_example_testndk_JniClient.h"
#include <jni.h>

#include <stdlib.h>
#include <stdio.h>

#include <android/log.h>
#include "VcyberVoiceSDK.h"
#include "base64.hpp"
#include <json/json.h>
#include <curl/curl.h>

#define  LOG_TAG    "HelloJni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

/*
static char abc[5012];

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{;
	//int written = fwrite(ptr, size, nmemb, (FILE *)fp);
	//printf("%s \n",ptr);
	LOGI("%s \n",ptr);
	strcpy(abc,(char*)ptr);
	return size * nmemb;
}

void getUrl()
{
	CURL *curl;
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: Agent-007");
	curl = curl_easy_init();    // 初始化
	if (curl)
	{
		//curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");// 代理
		//curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);// 改协议头
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_URL, "https://192.158.4.5/Test.aspx");
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); //将返回的http头输出到fp指向的文件
		//curl_easy_setopt(curl, CURLOPT_HEADERDATA, fp); //将返回的html主体数据输出到fp指向的文件
		CURLcode res_code = curl_easy_perform(curl);   // 执行
		LOGI("res_code := %d \n",res_code);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}
}
*/

#ifdef __cplusplus
extern "C"
{
#endif
	/*
	* Class:     com_ndk_test_JniClient
	* Method:    AddStr
	* Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
	*/

	JNIEXPORT jstring JNICALL Java_com_vcyber_shngi2_VDsdk_AddStr
		(JNIEnv *env, jclass arg, jstring instringA, jstring instringB)
	{
		LOGI("call from jni!");
		return env->NewStringUTF("abc");
	}

	/*
	* Class:     com_ndk_test_JniClient
	* Method:    AddInt
	* Signature: (II)I
	*/
	JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_AddInt
		(JNIEnv *env, jclass arg, jint a, jint b)
	{
		//getUrl();
		//LOGE(" hello word! "); 
		return a + b;
	}

	//Java_com_vcyber_shngi2_VDsdk_

	JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDInit(JNIEnv * env, jobject obj, jstring configs)
	{
		LOGE("%s \n", "test!");
		//testf();
		//return 0;
		const char *con = env->GetStringUTFChars(configs, 0);
		return CloudVDInit(con);
	}


	JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDStartSession(JNIEnv * env, jobject obj, jstring params)
	{
		SESSION_HANDLE session;
		const char *str = env->GetStringUTFChars(params, 0);
		CloudVDStartSession(str, &session);
		return (jint)session;
	}

	/*
	// state 1: 表示开始 2:表示继续 3:表示结束
	JNIEXPORT jint JNICALL Java_com_example_testndk_JniClient_CloudVDPostData(JNIEnv * env, jobject obj,
	jint session, jstring waveData, jint len, jint state)
	{
	const char *audio = env->GetStringUTFChars(waveData, 0);
	return CloudVDPostData((SESSION_HANDLE)session, audio, len, (eAudioStatus)state);
	}
	*/


	// state 1: 表示开始 2:表示继续 3:表示结束
	JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDPostData(JNIEnv * env, jobject obj,
		jint session, jbyteArray src, jint src_len, jint state)
	{
		jbyte *jbarray = (jbyte*)malloc(src_len);
		env->GetByteArrayRegion(src, 0, src_len, jbarray);

		int res = CloudVDPostData((SESSION_HANDLE)session, (char*)jbarray, src_len, (eAudioStatus)state);
		free(jbarray);

		return res;
	}

	JNIEXPORT jstring JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVWGetResult(JNIEnv * env, jobject obj,
		jint session)
	{
		ResultData *rd;
		CloudVDGetResult((SESSION_HANDLE)session, &rd);
		return env->NewStringUTF(rd->audio_text);
	}

	// 音频返回结果为base64编码的字符串。 type 1: 表示结果 2：表示语音数据
	JNIEXPORT jstring JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDGetResult(JNIEnv * env, jobject obj,
		jint session)
	{
		ResultData *rd;
		CloudVDGetResult((SESSION_HANDLE)session, &rd);
		return env->NewStringUTF(rd->result);
	}

	JNIEXPORT jbyteArray JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDGetAudio(JNIEnv * env, jobject obj,
		jint session)
	{
		ResultData *rd;
		CloudVDGetResult((SESSION_HANDLE)session, &rd);
		if (rd->audio == NULL)
		{
			jbyteArray temp;
			return temp;
		}
		int len = strlen(rd->audio);
		jbyte *audio = new jbyte[len];
		unsigned int out_len;
		Base64::base64_decode(rd->audio, len, (char*)audio, &out_len);

		jbyteArray jarray = env->NewByteArray(out_len);
		env->SetByteArrayRegion(jarray, 0, out_len, audio);
		delete[] audio;
		return jarray;
	}

	// type 1: 表示结果 2：表示语音数据
	JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDEndSession(JNIEnv * env, jobject obj,
		jint session)
	{
		return CloudVDEndSession((SESSION_HANDLE)session);
	}

	JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDFini(JNIEnv * env, jobject obj)
	{
		return CloudVDFini();
	}


#ifdef __cplusplus
}
#endif

#endif