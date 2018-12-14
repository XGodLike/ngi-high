#include "stdafx.h"
#include <stdlib.h>
#include <memory>
#include "SdkLib.h"
#include "crypt.h"
#include "TimeLog.h"
//vw的加密
#include "Common.h"
#include "VcyberVoiceSDK.h"

#include <string.h>
#include <vector>


//static char* user_info = "5413721411a020575107309077512060b010801767746544c13000804747b15730608060207607077000d6d7817740e0578070572434245264654580a0170235d3d1b54264";

 //公司默认测试
//#define PROGRAM_NAME	"test"
//#define APP_KEY "0BB349CD-F38D-4721-B479-90A9EB5A1686"

 //亚飞的项目
#define PROGRAM_NAME	"shngi2"
#define APP_KEY "45d79d27-fafe-47fd-8c45-57f1e9a800d8"

//VW的key
#define ASR_KEY "0BB349CD-F38D-4721-B479-90A9EB5A1686"
#define VERSION	"v1.0 Beta"

static CTimeLog p_Timelog;


std::string url_encode(std::string data)
{
	std::string out_string = "";
	int nInSize =  data.length();
	char buf[4]={0};
	char x = 0;
	for(int i=0; i<nInSize; i++)
	{
		memset(buf, 0, 4);
		x = data[i];
	
		if(isalnum(x))
		{
			buf[0] = (char)x;
		}
		else
		{
			sprintf(buf, "%c%02X", '%', x);
		}
		out_string += buf;
	}
	return out_string;
}

std::string get_data(SessionParam *sp, Configs *config, const char* method,
	const char* audio, eAudioStatus dataStatus)
{
	Json::Value data;
	data["packet_id"] = Json::Value(sp->m_packet_id);
	data["method"] = Json::Value(method);
	if (strcmp("SessionBegin", method) == 0)
	{
		data["business"] = Json::Value(sp->m_business.c_str());	
		data["app_id"] = Json::Value(config->m_app_id.c_str());

		std::string user_info;
		GetUserInfo(user_info, config, sp);
		data["user_info"] = Json::Value(user_info.c_str());

		data["tts"] = Json::Value(atoi(sp->m_tts.c_str()));
		data["audio_format"] = Json::Value(sp->m_audio_format);
		data["audio_quality"] = Json::Value(sp->m_audio_quality);
	}
	else if (strcmp("PutAudio", method) == 0)
	{
		if (AUDIO_STATUS_FIRST == dataStatus)
		{
			sp->m_audio_index = 0;
		}
		data["audio_index"] = Json::Value(sp->m_audio_index);
		if (AUDIO_STATUS_LAST == dataStatus)
		{
			data["audio_last"] = Json::Value(1);
		}
		else
		{
			data["audio_last"] = Json::Value(0);
		}
		sp->m_audio_index++;
		if (audio == nullptr)
		{
			data["audio"] = Json::Value("");
		}
		else
		{
			data["audio"] = Json::Value(audio);
		}
	}
	else if (strcmp("SessionEnd", method) == 0)
	{
		data["business"] = Json::Value("");
		data["error_code"] = Json::Value(sp->m_error_code);
		data["error_message"] = Json::Value(sp->m_error_message);
	}
	Json::FastWriter writer;
	std::string str_data = writer.write(data);
	if (str_data.size() > 0)
	{
		char *res = (char*)str_data.c_str();
		res[str_data.size() - 1] = '\0';
	}
	return str_data;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{	
	std::vector<char*>* vc = (std::vector<char*>*)stream;
	size_t stDataLen = size * nmemb;
	char* str = new char[stDataLen + 1];
	str[stDataLen] = '\0';
	memcpy(str, ptr, stDataLen);
	vc->push_back(str);
	
	return size * nmemb;
}

size_t write_data_str(void *ptr, size_t size, size_t nmemb, void *stream)
{	
	size_t len = size * nmemb;  
	string *strdata  = (string *) stream;
	if (strdata)
	{
		strdata->append((const char*)ptr,len);
	}
	 return len;  
}

size_t get_header( void *ptr, size_t size,size_t nmemb, void *stream)
{
	std::vector<char*>* vc = (std::vector<char*>*)stream;
	size_t stDataLen = size * nmemb;
	char* str = new char[stDataLen + 1];
	str[stDataLen] = '\0';
	memcpy(str, ptr, stDataLen);
	vc->push_back(str);

	return size * nmemb;
}

size_t get_header_str( void *ptr, size_t size,size_t nmemb, void *stream)
{
	size_t len = size * nmemb;  
	string *strdata  = (string *) stream;
	if (strdata)
	{
		strdata->append((const char*)ptr,len);
	}
	 return len;  
}

size_t Debug(void *curl, int nType, char * pData, size_t size, void *)
{
	switch (nType)
	{
	case CURLINFO_DATA_IN:
		p_Timelog.PrintTimeLog("[%s][DATA_IN]Start Time\n");
		//printf("[DATA_IN]%s\n", pData);
		break;
	case CURLINFO_DATA_OUT:
		p_Timelog.PrintTimeLog("[%s][DATA_OUT]Start Time\n");
		//printf("[DATA_OUT]%s\n", pData);
		break;
	case CURLINFO_HEADER_IN:
		p_Timelog.PrintTimeLog("[%s][HEADER_IN]Start Time\n");
		//printf("[HEADER_IN]%s\n", pData);
		break;
	case CURLINFO_HEADER_OUT:
		p_Timelog.PrintTimeLog("[%s][HEADER_OUT]Start Time\n");
		//printf("[HEADER_OUT]%s\n", pData);
		break;
	}

	return CURLE_OK;
}

string get_cookie(std::vector<char*> &vc)
{
	std::string res = "";
	char cookie[128] = {0};
	for (int i = 0; i < vc.size(); i++)
	{
		res = vc[i];
		sscanf(res.c_str(),"Set-Cookie: %s; path=/",cookie);
		delete vc[i];	
	}
	return cookie;
}

string get_cookie_str(string &vc)
{
	string cookie = "";
	string header = vc;
	int pos = header.find("\r\nSet-Cookie:");
	if (pos != string::npos)
	{
		string set_cookie = header.substr(pos+2,header.length()-pos-2);
		pos = set_cookie.find_first_of("\r\n");
		if (pos != string::npos)
		{
			string session_id = set_cookie.substr(0,pos);
			pos = session_id.find("session_id=");
			if (pos != string::npos)
			{
				string unsession_id = session_id.substr(pos,session_id.length()-pos);
				pos = unsession_id.find(";");
				if (pos != string::npos)
				{
					cookie = unsession_id.substr(0,pos);
				}
				else
				{
					cookie = unsession_id;
				}
			}
		}
	}
	return cookie;
}

Json::Value get_data(std::vector<char*> &vc)
{
	std::string res = "";
	for (int i = 0; i < vc.size(); i++)
	{
		res += vc[i];
		delete vc[i];
	}

	Json::Value jsonobj;
	Json::Reader jsonreader;
	jsonreader.parse(res.c_str(), jsonobj, false);

	return jsonobj;
}

Json::Value get_data_str(string &vc)
{
	string res = vc;
	Json::Value jsonobj;
	Json::Reader jsonreader;
	jsonreader.parse(res.c_str(), jsonobj, false);

	return jsonobj;
}

bool GetUserInfo(std::string & ciphertext, Configs *con, SessionParam *sp)
{
	std::string strOrg;
	strOrg.append("program=");
	strOrg.append(PROGRAM_NAME);
	strOrg.append(";");

	strOrg.append("gps=");
	sp->m_gps.empty() ? strOrg.append(con->m_gps):strOrg.append(sp->m_gps);
	//strOrg.append(sp->m_gps);
	strOrg.append(";");

	strOrg.append("vin=");
	sp->m_vin.empty() ? strOrg.append(con->m_vin):strOrg.append(sp->m_vin);
	//strOrg.append(con->m_vin);
	strOrg.append(";");

	strOrg.append("mac=");
	sp->m_mac.empty() ? strOrg.append(con->m_mac):strOrg.append(sp->m_mac);
	//strOrg.append(con->m_mac);
	strOrg.append(";");

	strOrg.append("tel=");
	sp->m_tel.empty() ? strOrg.append(con->m_tel):strOrg.append(sp->m_tel);
	//strOrg.append(con->m_tel);
	strOrg.append(";");

	strOrg.append("bluetooth=");
	sp->m_bluetooth.empty() ? strOrg.append(con->m_bluetooth):strOrg.append(sp->m_bluetooth);
	//strOrg.append(con->m_bluetooth);
	strOrg.append(";");

	strOrg.append("carnum=");
	sp->m_carnum.empty() ? strOrg.append(con->m_carnum):strOrg.append(sp->m_carnum);
	//strOrg.append(con->m_carnum);
	strOrg.append(";");

	strOrg.append("engine=");
	sp->m_engine.empty() ? strOrg.append(con->m_engine):strOrg.append(sp->m_engine);
	//strOrg.append(con->m_engine);
	strOrg.append(";");

	strOrg.append("city=");
	sp->m_city.empty() ? strOrg.append(con->m_city):strOrg.append(sp->m_city);
	//strOrg.append(sp->m_city);
	strOrg.append(";");

	strOrg.append(sp->m_other);

	// 加密
	int chipBufLen = strOrg.length() * 4;
	char * pChipBuf = new char[chipBufLen];
	if (NULL == pChipBuf)
	{
		return false;
	}
	memset(pChipBuf, 0, chipBufLen);
	int nEncRet = vcyber_encrypt(APP_KEY, strOrg.c_str(), strOrg.length(), pChipBuf, chipBufLen);
	if (0 == nEncRet)
	{
		return false;

	}
	pChipBuf[nEncRet] = '\0';
	ciphertext = pChipBuf;
	delete[]pChipBuf;
	return true;
}
