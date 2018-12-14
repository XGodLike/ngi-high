#pragma once

#include <queue>
#include <string>
#include <curl/curl.h>

#include "VcyberVoiceSDK.h"
using namespace std;

typedef struct _QueueBuf
{
	string buf;
	bool b_Last;
}qBuf;
typedef struct _Databuf
{
	queue<qBuf> q_buf;
	ResultData m_result;//vd结果
	bool b_start;//线程是否启动
	bool b_run;//线程是否要结束
	bool b_last;//是否发送了最后一个音频
	CURLcode c_code;//网络返回码
}dataBuf;

class Configs {
public:
	// 必选 服务器地址
	std::string m_server_addr;
	std::string m_server_ip;
	std::string m_server_port;
	// 必选 应用程序ID
	std::string m_app_id;
	// 可选。终端发起请求时的定位信息。
	// 格式是“纬度, 经度”（注意顺序），例如：39.983085, 116.80619。
	std::string m_gps;
	// 必选 车辆vin码。
	std::string m_vin;
	// 必选 终端mac地址。
	std::string m_mac;
	// 可选。终端蓝牙地址。
	std::string m_bluetooth;
	// 可选。用户手机号码。
	std::string m_tel;
	// 可选。车牌号。
	std::string m_carnum;
	// 可选。发动机号。
	std::string m_engine;
	// 可选。当前城市名称。 
	std::string m_city;
	//可选。默认输出日志
	bool b_log;
	//可选。默认选择智能DNS
	bool b_dns;


	bool Init(const char *str);

};

class SessionParam
{
public:
	//包含队列，线程状态，音频状态，网络状态的结构体
	dataBuf* m_data;
	// 
	curl_slist* m_http_headers;
	// 包数据id
	int m_packet_id;
	// 语音数据ID
	int m_audio_index;
	// 错误码
	int m_error_code;
	// 错误消息	
	std::string m_error_message;

	/////////////////////begin时候有可能会更新的内容/////////////////////////////////////
	// 可选。终端发起请求时的定位信息。
	// 格式是“纬度, 经度”（注意顺序），例如：39.983085, 116.80619。
	std::string m_gps;
	// 必选 车辆vin码。
	std::string m_vin;
	// 必选 终端mac地址。
	std::string m_mac;
	// 可选。终端蓝牙地址。
	std::string m_bluetooth;
	// 可选。用户手机号码。
	std::string m_tel;
	// 可选。车牌号。
	std::string m_carnum;
	// 可选。发动机号。	
	std::string m_engine;
	// 可选。当前城市名称。 
	std::string m_city;

	//用于扩展;
	std::string m_other;

/////////////////////////////////////////////////
	// 可选。发送音频的格式。
	// 1：8Khz，16bit，1channel
	// 2：16Khz，16bit，1channel（默认）
	int m_audio_format;
	
	//可选。音频压缩等级。范围：[0 - 10]。0表示不压缩。
	// 默认10。
	// 只用来控制语音识别库与云端之间的交互，不影响接口出入的音频数据。
	int m_audio_quality;

	// 可选。是否返回语音播报数据。
	// 0：不返回（默认）
	// 1：返回
	std::string m_tts;

	// 可选。网络连接超时时间，单位秒。默认5秒。
	//std::string m_time_out;
	int m_time_out;
	
	// 可选。当前业务类型：“poi”、“tel”，“music”、“radio”...。
	// 应用确定当前业务时，可以通过该参数指定，以便提高服务器识别效率。
	std::string m_business;
	

	
	// 加密
	void *tsilk_enc;
	// 解密
	void *tsilk_dec;

	SessionParam(const char *str);
	~SessionParam();
};
