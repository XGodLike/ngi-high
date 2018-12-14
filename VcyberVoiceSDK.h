#pragma once

#ifdef WIN32
#ifdef VCYBERVOICESDK_EXPORTS
#define VCYBERVOICESDK_API __declspec(dllexport)
#else
#define VCYBERVOICESDK_API __declspec(dllimport)
#endif // VCYBERINPUTSDK_EXPORTS
#else
#define VCYBERVOICESDK_API __attribute__ ((visibility ("default")))
#endif // WIN32


#ifdef __cplusplus
extern "C"
{
#endif

#define SESSION_HANDLE void*

	/**
	 * CloudVDSDK params
	 */

	typedef enum _eReturnCode
	{
		CLOUDVD_SUCCESS = 0,							//成功
		CLOUDVD_ERR_NULL_POINTER = 10001,				//空指针/句柄无效
		CLOUDVD_ERR_PARAM = 10002,						//参数错误
		CLOUDVD_ERR_SERVER = 10003,						//服务端错误
		CLOUDVD_ERR_NET = 10004,						//网络错误
	} eReturnCode;


	typedef enum _eAudioStatus
	{
		AUDIO_STATUS_FIRST = 1,		// first audio data section
		AUDIO_STATUS_CONTINUE,		// midile audio data section
		AUDIO_STATUS_LAST,			// last audio data section
	} eAudioStatus;


	typedef struct _ResultData
	{
		char*	audio_text;				//语音输入法返回结果
		char* 	result;					//识别结果json字符串
		char*	audio;					//返回的音频
		int		audioLenth;				//音频长度
		char*   uniqueSessionID;		//本次识别唯一标识id（vin+时间戳构成，保证唯一且能正常提取详细时间信息）
		eReturnCode		error_code;
		_ResultData(){
			audio = NULL;
			audioLenth = 0;
			audio_text = new char[256];
			memset(audio_text,'\0',256);
			result = new char[256];
			memset(result,'\0',256);
			uniqueSessionID = new char[64];
			memset(uniqueSessionID, '\0', 64);
			error_code = CLOUDVD_SUCCESS;
		}
		~_ResultData(){
			audio = NULL;
			audioLenth = 0;
			delete audio_text;
			audio_text = NULL;
			delete result;
			result = NULL;
			delete uniqueSessionID;
			uniqueSessionID = NULL;
			error_code = CLOUDVD_SUCCESS;
		}
	} ResultData;
	
	/**
	 * VW params
	 */
	#define CYV_ASR_STATUS_IN_PROCESS			0				//识别正在进行中
	#define CYV_ASR_STATUS_RETRIEVED			1				//可以检索识别结果
	#define	CYV_ASR_STATUS_NO_MATCH				2				//识别结束，没有匹配的结果
	#define CYV_ASR_STATUS_NO_SPEECH			3				//没有检测到语音
	#define CYV_ASR_STATUS_COMPLETE				4				//识别结束
	#define CYV_ASR_STATUS_DISCERN				5				//语音开始识别正在进行中

	typedef struct _ASRresultData
	{
		int ret_Code; //识别错误码
		unsigned int ret_Regstatus;	//	识别的状态
		char* ret_Text;	//识别结果
		char*   uniqueSessionID;		//本次识别唯一标识id（vin+时间戳构成，保证唯一且能正常提取详细时间信息）
	}ASRresultData;


	/**
	* 描述：
	*		初始化云连接库。
	*		使用该库之前（例如程序启动）首先调用该接口。
	*		如参数发生变化，需要先调用CloudVDFini之后，再调用该接口。
	*
	* 参数：
	*		【in】const char* configs：连接参数列表，为Json格式的字符串。
	*
	* 返回：函数执行结果。详见eReturnCode定义。
	*
	**/
	VCYBERVOICESDK_API eReturnCode CloudVDInit(const char * configs);
	typedef eReturnCode(*p_CloudVDInit) (const char * configs);

	/**
	* 描述：
	*		开启一次语音识别。
	*		每次开始语音识别时进行调用。设置此次语音识别所用参数。
	*
	* 参数：
	*		【in】 const char* params：参数列表，为Json格式的字符串。
	*		【out】 SESSION_HANDLE & handle：会话启动成功时，返回会话句柄。
	*
	* 返回：函数执行结果。详见eReturnCode定义。
	*
	**/
	VCYBERVOICESDK_API eReturnCode CloudVDStartSession(const char * params, SESSION_HANDLE * handle);
	typedef eReturnCode(*p_CloudVDStartSession) (const char * params, SESSION_HANDLE * handle);
	/**
	* 描述：
	*		推送VR语音数据到云端。
	*
	* 参数：
	*		【in】 SESSION_HANDLE handle：CloudVDStartSession返回的会话句柄
	*		【in】 const void * waveData：执行语音数据的指针
	*		【in】 unsigned int dataLen：语音数据的长度
	*		【in】 int dataStatus：音频数据块状态
	*
	* 返回：函数执行结果。详见eReturnCode定义。
	*
	**/
	VCYBERVOICESDK_API eReturnCode CloudVDPostData(SESSION_HANDLE handle, const void * waveData, unsigned int dataLen, eAudioStatus dataStatus);
	typedef eReturnCode(*p_CloudVDPostData) (SESSION_HANDLE handle, const void * waveData, unsigned int dataLen, eAudioStatus dataStatus);
	/**
	* 描述：
	*		获取语音识别的结果。
	*
	* 参数：
	*		【in】 SESSION_HANDLE handle：CloudVDStartSession返回的会话句柄
	*		【out】ResultData **resultData：获得识别结果结构体指针。
	*				接口返回后，调用者应该在再次调用SDK任何接口之前，
	*				立刻缓存返回结构体数据，不可缓存结构体指针。
	*				结构体所有权归SDK所有。调用者只能读取其中数据，
	*				不能修改数据、释放内存。
	*
	* 返回：函数执行结果。详见eReturnCode定义。
	*
	**/
	VCYBERVOICESDK_API eReturnCode CloudVDGetResult(SESSION_HANDLE handle, ResultData ** resultData);
	typedef eReturnCode(*p_CloudVDGetResult) (SESSION_HANDLE handle, ResultData ** resultData);

	/**
	* 描述：
	*		获取搜索信息的结果。
	*
	* 参数：
	*		【in】 const char * params：json 格式的参数
	*		【out】SearchResult **SearchResult：获得查询信息结构体指针。
	*				接口返回后，调用者应该在再次调用SDK任何接口之前，
	*				立刻缓存返回结构体数据，不可缓存结构体指针。
	*				结构体所有权归SDK所有。调用者只能读取其中数据，
	*				不能修改数据、释放内存。
	*
	* 返回：函数执行结果。详见eReturnCode定义。
	*
	**/
	//VCYBERVOICESDK_API eReturnCode CloudVDSearchInfo(const char * params, SearchResult ** SearchResult);
	//typedef eReturnCode(*p_CloudVDSearchInfo) (const char * params, SearchResult ** SearchResult);
	/**
	* 描述：
	*		结束一次语音识别，并将本次识别所用资源释放。
	*
	* 参数：
	*		【in】 SESSION_HANDLE handle：CloudVDStartSession返回的会话句柄
	*		【in】 int handle：本次语音识别的句柄
	*
	* 返回：函数执行结果。详见eReturnCode定义。
	*
	**/
	VCYBERVOICESDK_API eReturnCode CloudVDEndSession(SESSION_HANDLE handle);
	typedef eReturnCode(*p_CloudVDEndSession) (SESSION_HANDLE handle);

	/**
	* 描述：
	*		释放云连接库。
	*		不再使用该库时（例如程序退出），调用该接口。
	*
	* 参数：
	*
	* 返回：函数执行结果。详见eReturnCode定义。
	*
	**/
	VCYBERVOICESDK_API eReturnCode CloudVDFini();
	typedef eReturnCode(*p_CloudVDFini) ();
#ifdef __cplusplus
}
#endif
