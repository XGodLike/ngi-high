#include "stdafx.h"

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


static Configs g_configs;
static ASRConfigs g_ASRConfigs;
extern bool b_Log = false;
extern bool b_libcul_log = false;
static CTimeLog* p_Timelog = NULL;
static CURL *curl = NULL;


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
	if (g_configs.m_libcurl_log == 1) b_libcul_log = true;


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
	g_ASRConfigs.m_server_port = Get_Port((void*)g_ASRConfigs.m_server_addr.c_str());
	g_ASRConfigs.m_server_ip =  DNS_pod((void*)g_ASRConfigs.m_server_addr.c_str());
	if (g_ASRConfigs.m_server_ip != "")
	{
		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDInit(success)]DNS_pod Etime\n");
		printf("[ASRInit]DNS_pod IP:Port--->%s\n",g_ASRConfigs.m_server_ip.c_str());
		g_ASRConfigs.m_server_addr = "http://" + g_ASRConfigs.m_server_ip + ":" + g_ASRConfigs.m_server_port;
	}
	else if (b_Log)
	{
		set_share_handle(curl);
		p_Timelog->PrintTimeLog("[%s][ASRInit(unsuccess)]DNS_pod Etime\n");
	}


	CURLcode res = curl_easy_setopt(curl, CURLOPT_URL, g_configs.m_server_addr.c_str());
	if (CURLE_OK == res){
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
		printf("[CloudVDPostData]{audio_index:%d   is_last:%d}\n", sp->m_audio_index, dataStatus == AUDIO_STATUS_LAST ? 1 : 0);
	}

	//char *base64 = nullptr;
	std::shared_ptr<char> base64;
	std::shared_ptr<unsigned char> temp_data;
	if (dataStatus != AUDIO_STATUS_LAST)
	{
		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDPostData]SilkEncode Stime\n");
		if (sp->m_audio_quality != "0")
		{
			int len = tsilk_in(sp->tsilk_enc, waveData, dataLen);
			temp_data = std::shared_ptr<unsigned char>(new unsigned char[len]);
			if (len == 0)
			{
				return CLOUDVD_SUCCESS;
			}
			dataLen = tsilk_out(sp->tsilk_enc, temp_data.get(), len);
			waveData = temp_data.get();
		}
		unsigned int out_len = 0;
		int base64_len = (dataLen * 4 / 3) + (dataLen % 3 ? 4 : 0) + 1;
		base64 = std::shared_ptr<char>(new char[base64_len]);
		if (base64 == nullptr)
		{
			return CLOUDVD_ERR_NOMEMORY;
		}
		Base64::base64_encode((char*)waveData, dataLen, base64.get(), &out_len);
		base64.get()[out_len] = '\0';
		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDPostData]SilkEncode Etime\n");
	}
	std::string data = get_data(sp, &g_configs, "PutAudio", base64.get(), dataStatus);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
	std::vector<char*> vc;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &vc);

	if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDPostData]curl_easy_perform Stime\n");
	CURLcode res = curl_easy_perform(curl);
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

	if (!jsonobj["result"].isNull())
	{
		std::string out = jsonobj["result"].toStyledString();
		unsigned int len = out.size() + 1;
		sp->m_rd->result = (char*)realloc(sp->m_rd->result, len + 1);
		strcpy(sp->m_rd->result, out.c_str());

		char sid[64] = {0};
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
		//base64[out_len] = '\0';
		//sp->m_rd->audioLenth = (int)audio.size();
		//sp->m_rd->audio = (char*)realloc(sp->m_rd->audio, sp->m_rd->audioLenth + 1);
		//sp->m_rd->audio[sp->m_rd->audioLenth] = '\0';
		//memcpy(sp->m_rd->audio, audio.c_str(), sp->m_rd->audioLenth);

		char sid[64] = { 0 };
		time_t lt = time(NULL);
		sprintf(sid, "%s%d", g_configs.m_vin.c_str(), lt);
		strcpy(sp->m_rd->uniqueSessionID, sid);
	}
	if (CURLE_OK == res){
		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDPostData]CloudVDPostData End Time\n");
		return  (eReturnCode)error_code;
		// CLOUDVD_SUCCESS;
	} else {
		if (b_Log) p_Timelog->PrintTimeLog("[%s][CloudVDPostData]CloudVDPostData End Time\n");
		return CLOUDVD_ERR_SYSTEM_FAILED;
	}
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

//可以连接vw接口的sdk
eReturnCode ASRInit(const char* configs)
{
	bool b_res = g_ASRConfigs.ASRInit(configs);
	if (g_ASRConfigs.m_log == 1)
	{
		b_Log = true;
		p_Timelog = new CTimeLog();
	}
	if (g_ASRConfigs.m_libcurl_log == 1) b_libcul_log = true;

	if (b_res == false) {
		return CLOUDVD_ERR_INVALID_PARAM;
	}
	if (g_ASRConfigs.m_server_addr.size() == 0	|| g_ASRConfigs.m_app_id.size() == 0){
		return CLOUDVD_ERR_INVALID_PARAM;
	}

	curl_global_init(CURL_GLOBAL_ALL);
	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRInit]curl_easy_init Stime\n");
	curl = curl_easy_init();
	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRInit]curl_easy_init Etime\n");

	if (curl == nullptr) {
		return CLOUDVD_ERR_SYSTEM_FAILED;
	}

	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRInit(success)]DNS_pod Stime\n");
	g_ASRConfigs.m_server_port = Get_Port((void*)g_ASRConfigs.m_server_addr.c_str());
	g_ASRConfigs.m_server_ip =  DNS_pod((void*)g_ASRConfigs.m_server_addr.c_str());
	if (g_ASRConfigs.m_server_ip != "")
	{
		if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRInit(success)]DNS_pod Etime\n");
		printf("[ASRInit]DNS_pod IP:Port--->%s\n",g_ASRConfigs.m_server_ip.c_str());
		g_ASRConfigs.m_server_addr = "http://" + g_ASRConfigs.m_server_ip + ":" + g_ASRConfigs.m_server_port;
	}
	else if (b_Log)
	{
		set_share_handle(curl);
		p_Timelog->PrintTimeLog("[%s][ASRInit(unsuccess)]DNS_pod Etime\n");
	}
	
	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRInit]ASRInit End Time\n");
	return CLOUDVD_SUCCESS;
}

eReturnCode ASRStartSession(const char * params, SESSION_HANDLE * handle) 
{
	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRStartSession]ASRStartSession Start Time\n");

	ASRParam *sp = new ASRParam(params);	
	*handle = sp;

	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRStartSession]POST Stime\n");
	sp->m_http_headers = nullptr;
	sp->m_http_headers = curl_slist_append(sp->m_http_headers, "Connection: close");
	sp->m_http_headers = curl_slist_append(sp->m_http_headers, "Accept:");		
	sp->m_http_headers = curl_slist_append(sp->m_http_headers, "Content-Type:");	
	sp->m_http_headers = curl_slist_append(sp->m_http_headers, "Expect:");
	
	//http消息报头
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, sp->m_http_headers);
	curl_easy_setopt(curl, CURLOPT_URL, g_ASRConfigs.m_server_addr.c_str());
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, sp->m_time_out);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, sp->m_time_out);    	
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, Debug);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	//请求正文	
	std::string data = get_data(sp, &g_ASRConfigs, "QASRSessionBegin", nullptr, 0, AUDIO_STATUS_CONTINUE);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRStartSession]POST Etime\n");

	//获得服务端data
	std::vector<char*> vc;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &vc);

	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRStartSession]curl_easy_perform Stime\n");
	CURLcode res = curl_easy_perform(curl);
	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRStartSession]curl_easy_perform Etime\n");

	if (CURLE_OK == res || res == CURLE_PARTIAL_FILE)
	{
		Json::Value jsonobj = get_data(vc);

		int error_code = (int)CLOUDVD_ERR_INVALID_XML;
		if (jsonobj["ErrorCode"].isInt())
		{
			error_code = jsonobj["ErrorCode"].asInt();
			printf("[ASRStartSession]--->error_code: %d\n", error_code);
			if (error_code > 0) sp->m_rd->ret_Code = error_code;
		}
		if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRStartSession]ASRStartSession End Time\n");
		return (eReturnCode)error_code;
	}
	else
	{
		delete sp;
		*handle = nullptr;
		printf("[ASRStartSession]--->error_code:%d\n",res);
		if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRStartSession]ASRStartSession End Time\n");
		return CLOUDVD_ERR_SYSTEM_FAILED;
	}	
}

eReturnCode ASRPostData(SESSION_HANDLE handle, const void * waveData, unsigned int dataLen, eAudioStatus dataStatus)
{
	ASRParam *sp = (ASRParam*)handle;
	if (b_Log)
	{
		p_Timelog->PrintTimeLog("[%s][ASRPostData]ASRPostData Start Time\n");
		printf("[ASRPostData]{audio_index:%d   is_last:%s}\n", sp->m_audio_index, dataStatus == AUDIO_STATUS_LAST ? "true":"false");
	}
	
	std::shared_ptr<char> base64;
	std::shared_ptr<unsigned char> temp_data;
	std::string data = "";
	char* pOut = nullptr;

	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRPostData]SilkEncode Stime\n");
	//silk压缩
	if (sp->m_audio_quality >= 0)
	{
		void* pCompressHandle = nullptr;
		int nEncodeWaveLen = 0;
		bool bEnd = false;
		if (dataStatus == AUDIO_STATUS_LAST) bEnd = true;
		pOut = CVcyberSpeex::SilkEncode(&pCompressHandle, bEnd, (char*)waveData, dataLen, sp->m_audio_BandMode, sp->m_audio_quality, &nEncodeWaveLen);
		waveData = pOut;
		dataLen = nEncodeWaveLen;
	}
	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRPostData]SilkEncode Etime\n");
	//是否采用base64编码
	if (sp->m_Isbase64)
	{
		unsigned int out_len = 0;
		int base64_len = (dataLen * 4 / 3) + (dataLen % 3 ? 4 : 0) + 1;
		base64 = std::shared_ptr<char>(new char[base64_len]);
		if (base64 == nullptr)
		{
			return CLOUDVD_ERR_NOMEMORY;
		}
		Base64::base64_encode((char*)waveData, dataLen, base64.get(), &out_len);
		base64.get()[out_len] = '\0';
		data = get_data(sp, &g_ASRConfigs, "QASRPutAudio", base64.get(), dataLen, dataStatus);
		data = url_encode(data);
	}
	else
	{
		data = get_data(sp, &g_ASRConfigs, "QASRPutAudio", (const char*)waveData, dataLen, dataStatus);
	}
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
	
	std::vector<char*> vc;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &vc);

	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRPostData]curl_easy_perform Stime\n");
	CURLcode res = curl_easy_perform(curl);
	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRPostData]curl_easy_perform Etime\n");

	if (CURLE_OK == res|| res == CURLE_PARTIAL_FILE)
	{
		int ret_code = (int)CLOUDVD_ERR_INVALID_XML;

		Json::Value jsonobj = get_data(vc);

		if (!jsonobj["ErrorCode"].isNull()  && jsonobj["ErrorCode"].isInt())
		{
			ret_code = jsonobj["ErrorCode"].asInt();
			printf("[ASRStartSession]--->error_code: %d\n", ret_code);
			if (ret_code > 0) sp->m_rd->ret_Code = ret_code;
		}
		if (!jsonobj["Status"].isNull() && jsonobj["Status"].isString())
		{
			sp->m_rd->ret_Regstatus = jsonobj["Status"].asInt();
		}

		if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRPostData]ASRPostData End Time\n");
		return  (eReturnCode)ret_code;
	}
	else 
	{
		if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRPostData]ASRPostData End Time\n");
		return CLOUDVD_ERR_SYSTEM_FAILED;
	}
}

eReturnCode ASRGetResult(SESSION_HANDLE handle, ASRresultData ** resultData) 
{
	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRGetResult]ASRGetResult Start Time\n");
	ASRParam *sp = (ASRParam*)handle;

	std::string data = get_data(sp, &g_ASRConfigs, "QASRGetResult", "", 0, AUDIO_STATUS_LAST);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

	std::vector<char*> vc;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &vc);

	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRGetResult]curl_easy_perform Stime\n");
	CURLcode res = curl_easy_perform(curl);
	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRGetResult]curl_easy_perform Etime\n");

	if (CURLE_OK == res|| res == CURLE_PARTIAL_FILE)
	{
		int ret_code = (int)CLOUDVD_ERR_INVALID_XML;
		Json::Value jsonobj = get_data(vc);
		if (!jsonobj["Status"].isNull() && jsonobj["Status"].isString())
		{
			sp->m_rd->ret_Regstatus = jsonobj["Status"].asInt();
			if (sp->m_rd->ret_Regstatus == CYV_ASR_STATUS_IN_PROCESS || sp->m_rd->ret_Regstatus == CYV_ASR_STATUS_DISCERN)
			{
				return ASRGetResult(handle, resultData);
			}
		}
		if (!jsonobj["ErrorCode"].isNull()  && jsonobj["ErrorCode"].isInt())
		{
			ret_code = jsonobj["ErrorCode"].asInt();
			sp->m_rd->ret_Code = ret_code;
		}
		if (!jsonobj["Text"].isNull() && jsonobj["Text"].isString())
		{
			std::string result = jsonobj["Text"].asString();
			std::string::size_type pos = result.find("[generic_ex]=");
			if (pos != std::string::npos)
			{
				result = result.substr(pos+std::string("[generic_ex]=").length(),jsonobj["Text"].asString().length());
			}
			//将ret_Text封装成json格式
			Json::Value json_Root;
			Json::Value json_Result;			
			json_Root["_rc"] =  Json::Value(0);
			json_Root["_text"] =  Json::Value(result);
			json_Root["_conf"] = Json::Value(0);
			json_Result["_name"] = Json::Value("IME_Result");
			json_Result["_score"] = Json::Value(0);
			json_Root["_topic"] = json_Result;
			std::string json2string = json_Root.toStyledString();
			strncpy(sp->m_rd->ret_Text,json2string.c_str(),json2string.length());

			//strncpy(sp->m_rd->ret_Text,result.c_str(),result.length());
		}

		char sid[64] = { 0 };
		time_t lt = time(NULL);
		sprintf(sid, "%s%d", g_ASRConfigs.m_vin.c_str(), lt);
		strcpy(sp->m_rd->uniqueSessionID, sid);
		*resultData = sp->m_rd;
		if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRGetResult]ASRGetResult End Time\n");
		return  (eReturnCode)ret_code;
	}
	else 
	{
		if (b_Log) p_Timelog->PrintTimeLog("[%s][ASRGetResult]ASRGetResult End Time\n");
		return CLOUDVD_ERR_SYSTEM_FAILED;
	}	
}

eReturnCode ASREndSession(SESSION_HANDLE handle) 
{
	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASREndSession]ASREndSession Start Time\n");
	ASRParam *sp = (ASRParam*)handle;
	
	std::string data = get_data((ASRParam*)handle, &g_ASRConfigs, "QASRSessionEnd", "",0, AUDIO_STATUS_CONTINUE);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

	std::vector<char*> vc;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &vc);

	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASREndSession]curl_easy_perform Stime\n");
	CURLcode res = curl_easy_perform(curl);
	if (b_Log) p_Timelog->PrintTimeLog("[%s][ASREndSession]curl_easy_perform Etime\n");

	if (CURLE_OK == res|| res == CURLE_PARTIAL_FILE){
		//Json::Value jsonobj = get_data(vc);
		curl_slist_free_all(sp->m_http_headers);
		delete sp;
		sp = nullptr;
		if (b_Log) p_Timelog->PrintTimeLog("[%s][ASREndSession]ASREndSession End Time\n");
		return CLOUDVD_SUCCESS;
	} else {
		if (b_Log) p_Timelog->PrintTimeLog("[%s][ASREndSession]ASREndSession End Time\n");
		return CLOUDVD_ERR_SYSTEM_FAILED;
	}
	
}

eReturnCode ASRFini() 
{
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	curl = nullptr;
	if (b_Log)
	{
		p_Timelog->PrintTimeLog("[%s][ASRFini]ASRFini End Time\n");
		delete p_Timelog;
		p_Timelog = nullptr;
	}
	return CLOUDVD_SUCCESS;	
}