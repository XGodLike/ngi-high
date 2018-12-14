/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_example_testndk_JniClient */

#ifndef _Included_com_vcyber_shngi2_VDsdk
#define _Included_com_vcyber_shngi2_VDsdk

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
  (JNIEnv *env, jclass arg, jstring instringA, jstring instringB);

/*
* Class:     com_ndk_test_JniClient
* Method:    AddInt
* Signature: (II)I
*/
	JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_AddInt
  (JNIEnv *env, jclass arg, jint a, jint b);

//Java_com_vcyber_shngi2_VDsdk_

JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDInit(JNIEnv * env, jobject obj, jstring configs);


JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDStartSession(JNIEnv * env, jobject obj, jstring params);


JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDPostData(JNIEnv * env, jobject obj, 
	jint session, jbyteArray src, jint src_len, jint state);


JNIEXPORT jstring JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVWGetResult(JNIEnv * env, jobject obj,
		jint session)

// 音频返回结果为base64编码的字符串。 type 1: 表示结果 2：表示语音数据
JNIEXPORT jstring JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDGetResult(JNIEnv * env, jobject obj, 
	jint session);

JNIEXPORT jbyteArray JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDGetAudio(JNIEnv * env, jobject obj, 
	jint session);

JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDEndSession(JNIEnv * env, jobject obj,
	jint session);

JNIEXPORT jint JNICALL Java_com_vcyber_shngi2_VDsdk_CloudVDFini(JNIEnv * env, jobject obj);


#ifdef __cplusplus
}
#endif
