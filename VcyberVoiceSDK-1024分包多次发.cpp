#include "stdafx.h"

//#include "vld.h"
#include <time.h>
#include <memory>

#include <json/json.h>
#include <curl/curl.h>
#include "base64.hpp"
#include "tsilk.h"

#include "VcyberVoiceSDK.h"
#include "VcyberSpeex.h"
#include "DNS.h"
#include "Configs.h"
#include "SdkLib.h"
#include "TimeLog.h"

const int MIN_PACKET = 1024;
char aduioData[MIN_PACKET*MIN_PACKET] = {0};
int packet_size = 1024;
int audio_len = 0;
int audio_size = 0;
static eAudioStatus audioStatus;

static Configs g_configs;
extern bool b_Log = false;
static CTimeLog* p_Timelog = NULL;
static CURL *curl = NULL;

eReturnCode CloudVDPostAudio(SESSION_HANDLE handle,const void* waveData, std::shared_ptr<char> base64, unsigned int dataLen, eAudioStatus dataStatus)
{
	SessionParam *sp = (SessionParam*)handle;
	unsigned int out_len = 0;
	int base64_len = (dataLen * 4 / 3) + (dataLen % 3 ? 4 : 0) + 1;
	base64 = std::shared_ptr<char>(new char[base64_len]);
	if (base64 == nullptr)
	{
		return CLOUDVD_ERR_NOMEMORY;
	}
	Base64::base64_encode((char*)waveData, dataLen, base64.get(), &out_len);
	base64.get()[out_len] = '\0';
	std::string data = get_data(sp, &g_configs, "PutAudio", base64.get(), dataStatus);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
	std::vector<char*> vc;
	CURLcode res;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &vc);
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDPostData]curl_easy_perform Stime\n");
	res = curl_easy_perform(curl);
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDPostData]curl_easy_perform Etime\n");

	Json::Value jsonobj = get_data(vc);
	sp->m_packet_id = jsonobj["packet_id"].asInt() + 1;
	sp->m_business = jsonobj["business"].asString();

	int error_code = jsonobj["error_code"].asInt();
	std::string error_message = jsonobj["error_message"].asString();
	if (error_code > 0)
	{
		sp->m_error_code = error_code;
	}
	if (error_message.size() > 0)
	{
		sp->m_error_message = error_message;
	}

	//获取语音输入法的结果
	if (!jsonobj["audio_text"].isNull())
	{
		std::string audio_text = jsonobj["audio_text"].toStyledString();
		unsigned int len = audio_text.size() + 1;
		sp->m_rd->audio_text = (char*)realloc(sp->m_rd->audio_text, len + 1);
		strcpy(sp->m_rd->audio_text, audio_text.c_str());
	}

	if (!jsonobj["result"].isNull())
	{
		std::string out = jsonobj["result"].toStyledString();
		unsigned int len = out.size() + 1;
		sp->m_rd->result = (char*)realloc(sp->m_rd->result, len + 1);
		strcpy(sp->m_rd->result, out.c_str());

		char sid[64] = { 0 };
		time_t lt = time(NULL);
		sprintf(sid, "%s%d", g_configs.m_vin.c_str(), lt);
		strcpy(sp->m_rd->uniqueSessionID, sid);
	}

	if (!jsonobj["audio"].isNull())
	{
		std::string audio = jsonobj["audio"].asString();
		if (audio.size() > 0)
		{
			sp->m_rd->audio = (char*)realloc(sp->m_rd->audio, audio.size());
			unsigned int len;
			Base64::base64_decode((char*)audio.c_str(), audio.size(), sp->m_rd->audio, &len);
			sp->m_rd->audioLenth = len;

			if (sp->m_audio_quality != "0")
			{
				len = tsilk_in(sp->tsilk_dec, sp->m_rd->audio, len);
				if (len > 0)
				{
					std::shared_ptr<unsigned char> temp_data(new unsigned char[len]);
					sp->m_rd->audioLenth = tsilk_out(sp->tsilk_enc, temp_data.get(), len);
					sp->m_rd->audio = (char*)realloc(sp->m_rd->audio, sp->m_rd->audioLenth);
					memcpy(sp->m_rd->audio, temp_data.get(), sp->m_rd->audioLenth);
				}
				else
				{
					sp->m_rd->audioLenth = 0;
				}
			}
		}

		char sid[64] = { 0 };
		time_t lt = time(NULL);
		sprintf(sid, "%s%d", g_configs.m_vin.c_str(), lt);
		strcpy(sp->m_rd->uniqueSessionID, sid);
	}
	if (CURLE_OK == res){
		
		return  (eReturnCode)error_code;
	}
	else {
		return CLOUDVD_ERR_SYSTEM_FAILED;
	}
}

eReturnCode CloudVDInit(const char * configs) {
	
	bool b_res = g_configs.Init(configs);

	if (b_res == false) {
		return CLOUDVD_ERR_INVALID_PARAM;
	}
	//日志控制
	if (g_configs.m_log == 1)
	{
		b_Log = true;
		p_Timelog = new CTimeLog();
	}


	if (g_configs.m_server_addr.size() == 0	|| g_configs.m_app_id.size() == 0
		|| g_configs.m_vin.size() == 0 || g_configs.m_mac.size() == 0) {
		return CLOUDVD_ERR_INVALID_PARAM;
	}
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDInit]curl_easy_init  Stime\n");
	curl = curl_easy_init();
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDInit]curl_easy_init  Etime\n");
	if (curl == nullptr) {
		return CLOUDVD_ERR_SYSTEM_FAILED;
	}

	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDInit(success)]DNS_pod Stime\n");
	g_configs.m_server_port = Get_Port((void*)g_configs.m_server_addr.c_str());
	g_configs.m_server_ip =  DNS_pod((void*)g_configs.m_server_addr.c_str());
	if (g_configs.m_server_ip != "")
	{
		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDInit(success)]DNS_pod Etime\n");
		printf("[CloudVDInit]DNS_pod IP:Port--->%s\n",g_configs.m_server_ip.c_str());
		g_configs.m_server_addr = "http://" + g_configs.m_server_ip + ":" + g_configs.m_server_port;
	}
	else if (b_Log)
	{
		set_share_handle(curl);
		p_Timelog->PrintTimeLog("[%s][CloudVDInit(unsuccess)]DNS_pod Etime\n");
	}

	CURLcode res = curl_easy_setopt(curl, CURLOPT_URL, g_configs.m_server_addr.c_str());
	if (CURLE_OK == res){
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, Debug);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");		

		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDInit]CloudVDInit End Time\n");
		return CLOUDVD_SUCCESS;
	} else {
		// TODO 目前没有成功的都是系统错误（不在细分，用日志记录）
		printf("[CloudVDInit]curl_easy_setopt return code--->%s\n", res);
		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDInit]CloudVDInit End Time\n");
		return CLOUDVD_ERR_SYSTEM_FAILED;
	}
}

eReturnCode CloudVDStartSession(const char * params, SESSION_HANDLE * handle) {

	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDStartSession]CloudVDStartSession Start Time\n");
	SessionParam *sp = new SessionParam(params);
	*handle = sp;

	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDStartSession]POST Stime\n");
	sp->m_http_headers = curl_slist_append(nullptr, "Content-Type:");
	sp->m_http_headers = curl_slist_append(sp->m_http_headers, "Accept:");
	sp->m_http_headers = curl_slist_append(sp->m_http_headers, "Expect:");
	sp->m_http_headers = curl_slist_append(sp->m_http_headers, "Connection: Keep-Alive");

	CURLcode res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, sp->m_http_headers);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, sp->m_time_out);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, sp->m_time_out);

	std::string data = get_data(sp, &g_configs, "SessionBegin", nullptr, AUDIO_STATUS_CONTINUE);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDStartSession]POST Etime\n");

	std::vector<char*> vc;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &vc);

	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDStartSession]curl_easy_perform Stime\n");
	res = curl_easy_perform(curl);
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDStartSession]curl_easy_perform Etime\n");

	Json::Value jsonobj = get_data(vc);
	sp->m_packet_id = jsonobj["packet_id"].asInt() + 1;
	
	int error_code = jsonobj["error_code"].asInt();
	std::string error_message = jsonobj["error_message"].asString();
	if (error_code > 0)
	{
		sp->m_error_code = error_code;
	}
	if (error_message.size() > 0)
	{
		sp->m_error_message = error_message;
	}
	if (CURLE_OK == res){
		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDStartSession]CloudVDStartSession  End Time\n");
		return (eReturnCode)sp->m_error_code;
		// CLOUDVD_SUCCESS;
	} else {
		delete sp;
		*handle = nullptr;
		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDStartSession]CloudVDStartSession  End Time\n");
		return CLOUDVD_ERR_SYSTEM_FAILED;
	}
}

eReturnCode CloudVDPostData(SESSION_HANDLE handle, const void * waveData, unsigned int dataLen, eAudioStatus dataStatus) {

	SessionParam *sp = (SessionParam*)handle;
	if (b_Log)
	{
		p_Timelog->PrintTimeLog("[%s][CloudVDPostData]CloudVDPostData Start Time\n");
	}

	eReturnCode ret = eReturnCode(0);
	std::shared_ptr<char> base64;
	std::shared_ptr<unsigned char> temp_data;
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDPostData]SilkEncode Stime\n");

	//压缩
	if (sp->m_audio_quality != "0")
	{
		int len = tsilk_in(sp->tsilk_enc, waveData, dataLen);
		temp_data = std::shared_ptr<unsigned char>(new unsigned char[len]);
		dataLen = tsilk_out(sp->tsilk_enc, temp_data.get(), len);
		waveData = (char*)temp_data.get();
	}
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDPostData]SilkEncode Etime\n");
	if (g_configs.m_packet != 0)
	{
		packet_size = g_configs.m_packet;
	}

	//压缩后长度为0，不发送给VD，直接返回;数据包较小时，组合成一个较大的包再发送
	if (dataStatus == AUDIO_STATUS_LAST)
	{
		audio_len += dataLen;
		memcpy(aduioData + audio_size,(const char*)waveData,dataLen);
		dataLen = audio_len;
		waveData = aduioData;
		ret = CloudVDPostAudio(sp,waveData, base64, dataLen, dataStatus);
		audio_len = 0;
		audio_size = 0;
		memset(aduioData,NULL,sizeof(char)*MIN_PACKET*MIN_PACKET);
		return ret;
	}

	if (dataLen == 0)
	{
		return CLOUDVD_SUCCESS;
	}
	else
	{
		audio_len += dataLen;
		if(audio_len < packet_size)
		{
			memcpy(aduioData + audio_size,(const char*)waveData,dataLen);
			audio_size += dataLen;
			return CLOUDVD_SUCCESS;
		}
		else
		{
			memcpy(aduioData + audio_size,(const char*)waveData,dataLen);
		}
		dataLen = audio_len;
		waveData = aduioData;
	}

	
	int Data_len = dataLen;
	char* pAudio = (char*)waveData;
	int bag_index = 0;
	//分包发送
	if (g_configs.m_packet == 0)
	{
		//ret = CloudVDPostAudio(sp,waveData, base64, dataLen, dataStatus);
		g_configs.m_packet = packet_size;
	}
	do
	{		
		int EACH_BAG = g_configs.m_packet;
		char* audio = (char*)(pAudio+bag_index*EACH_BAG);
		if(dataLen > EACH_BAG)
		{
			if (dataStatus == AUDIO_STATUS_FIRST)
			{
				if (dataLen == Data_len)
				{
					ret = CloudVDPostAudio(sp,audio, base64, EACH_BAG, dataStatus);
				}
				else if (Data_len <= EACH_BAG)
				{
					ret = CloudVDPostAudio(sp,audio, base64, Data_len, AUDIO_STATUS_CONTINUE);
				}else
				{
					ret = CloudVDPostAudio(sp,audio, base64, EACH_BAG, AUDIO_STATUS_CONTINUE);
				}
			}
			else if (dataStatus == AUDIO_STATUS_LAST)
			{
				if (Data_len <= EACH_BAG)
				{
					ret = CloudVDPostAudio(sp,audio, base64, Data_len, AUDIO_STATUS_LAST);
				}
				else
				{
					ret = CloudVDPostAudio(sp,audio, base64, EACH_BAG, AUDIO_STATUS_CONTINUE);
				}
			}
			else
			{
				if (Data_len <= EACH_BAG)
				{
					ret = CloudVDPostAudio(sp,audio, base64, Data_len, AUDIO_STATUS_CONTINUE);
				}
				else
				{
					ret = CloudVDPostAudio(sp,audio, base64, EACH_BAG, AUDIO_STATUS_CONTINUE);
				}
			}
	  	
			Data_len -= EACH_BAG;
			bag_index++;
		}else
		{
				ret = CloudVDPostAudio(sp,waveData, base64, dataLen, dataStatus);
				Data_len -= EACH_BAG;
		}
	} while (Data_len > 0);

	audio_len = 0;
	audio_size = 0;
	memset(aduioData,NULL,sizeof(char)*MIN_PACKET*MIN_PACKET);
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDPostData]CloudVDPostData End Time\n");
	return ret;
}

eReturnCode CloudVDGetResult(SESSION_HANDLE handle, ResultData ** resultData) {
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDGetResult]CloudVDGetResult Start Time\n");
	SessionParam *sp = (SessionParam*)handle;
	*resultData = sp->m_rd;
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDGetResult]CloudVDGetResult End Time\n");
	return CLOUDVD_SUCCESS;
}

eReturnCode CloudVDEndSession(SESSION_HANDLE handle) {
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDEndSession]CloudVDEndSession Start Time\n");

	SessionParam *sp = (SessionParam*)handle;
	
	std::string data = get_data((SessionParam*)handle, &g_configs, "SessionEnd", "", AUDIO_STATUS_CONTINUE);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
	std::vector<char*> vc;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &vc);

	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDEndSession]curl_easy_perform Stime\n");
	CURLcode res = curl_easy_perform(curl);
	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDEndSession]curl_easy_perform Stime\n");

	Json::Value jsonobj = get_data(vc);
	curl_slist_free_all(sp->m_http_headers);
	delete sp;
	sp = nullptr;
	if (CURLE_OK == res){
		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDEndSession]CloudVDEndSession End Time\n");
		return CLOUDVD_SUCCESS;
	} else {
		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDEndSession]CloudVDEndSession End Time\n");
		return CLOUDVD_ERR_SYSTEM_FAILED;
	}
}

eReturnCode CloudVDFini() {

	curl_easy_cleanup(curl);
	curl = nullptr;
	if (b_Log)
	{
		p_Timelog->PrintTimeLog("[%s][CloudVDFini]CloudVDFini End Time\n");
		delete p_Timelog;
		p_Timelog = nullptr;
	}
	return CLOUDVD_SUCCESS;
}