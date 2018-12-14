#include "stdafx.h"
#include <stdlib.h>
#include "Configs.h"
#include <json/json.h>
#include "tsilk.h"
#include "VcyberSpeex.h"

bool Configs::Init(const char *str)
{
	Json::Value jsonobj;
	Json::Reader jsonreader;

	bool res = jsonreader.parse(str, jsonobj, false);
	if (res == false) {
		return false;
	}
	m_server_ip = "";
	m_server_port = "";
	m_server_addr = jsonobj["server_addr"].asString();
	m_app_id = jsonobj["app_id"].asString();
	//可能更新的参数
	m_city = jsonobj["city"].asString();
	m_gps = jsonobj["gps"].asString();
	m_vin = jsonobj["vin"].asString();
	m_mac = jsonobj["mac"].asString();
	m_bluetooth = jsonobj["bluetooth"].asString();
	m_tel = jsonobj["tel"].asString();
	m_carnum = jsonobj["carnum"].asString();
	m_engine = jsonobj["engine"].asString();
	
	std::string m_log = jsonobj["log"].asString();
	std::string m_dns = jsonobj["dns"].asString();
	b_dns = m_dns != "0" ? true : false;//默认做DNS
	b_log = m_log != "0" ? true : false;//默认输出log
	return true;
}

SessionParam::SessionParam(const char *str)
{
	m_data = new dataBuf();
	Json::Value jsonobj;
	Json::Reader jsonreader;

	bool res = jsonreader.parse(str, jsonobj, false);
	if (res == false) {
		return;
	}
	m_packet_id = 1;
	m_audio_index = 0;
	m_error_code = 0;

	if(jsonobj.isMember("audio_format"))
	{
		m_audio_format = jsonobj["audio_format"].asInt();
		jsonobj.removeMember("audio_format");
	}

	if(jsonobj.isMember("audio_quality"))
	{
		m_audio_quality = jsonobj["audio_quality"].asInt();
		jsonobj.removeMember("audio_quality");
	}
	
	if (m_audio_quality <= 0 || m_audio_quality > 10)
	{
		m_audio_quality = 10;
	}

	int q = m_audio_quality;
	int hz = 0;
	if (m_audio_format == 1)
	{
		hz = 8000;
	}
	else if (m_audio_format == 2)
	{
		hz = 16000;
	}
	tsilk_enc = tsilk_init(NULL, 1, hz, q);
	tsilk_dec = tsilk_init(NULL, 2, hz, q);
	if (tsilk_enc == NULL || tsilk_dec == NULL)
	{
		return;
		//throw "tsilk_init fail";
	}
	if (jsonobj.isMember("tts"))
	{
		m_tts = jsonobj["tts"].asString();
		jsonobj.removeMember("tts");
	}

	if (jsonobj.isMember("time_out"))
	{
		m_time_out = jsonobj["time_out"].asInt();
		if (m_time_out <= 0)
		{
			m_time_out = 2;
		}
		jsonobj.removeMember("time_out");
	}else
	{
		m_time_out = 2;
	}


	if (jsonobj.isMember("business"))
	{
		m_business = jsonobj["business"].asString();
		jsonobj.removeMember("business");
	}
	
	//可能会更新的参数
	if(jsonobj.isMember("city"))
	{
		m_city = jsonobj["city"].asString();
		jsonobj.removeMember("city");
	}

	if(jsonobj.isMember("gps"))
	{
		m_gps = jsonobj["gps"].asString();
		jsonobj.removeMember("gps");
	}

	if(jsonobj.isMember("vin"))
	{
		m_vin = jsonobj["vin"].asString();
		jsonobj.removeMember("vin");
	}

	if(jsonobj.isMember("mac"))
	{
		m_mac = jsonobj["mac"].asString();
		jsonobj.removeMember("mac");
	}

	if(jsonobj.isMember("bluetooth"))
	{
		m_bluetooth = jsonobj["bluetooth"].asString();
		jsonobj.removeMember("bluetooth");
	}

	if(jsonobj.isMember("tel"))
	{
		m_tel = jsonobj["tel"].asString();
		jsonobj.removeMember("tel");
	}

	if(jsonobj.isMember("carnum"))
	{
		m_carnum = jsonobj["carnum"].asString();
		jsonobj.removeMember("carnum");
	}

	if(jsonobj.isMember("engine"))
	{
		m_engine = jsonobj["engine"].asString();
		jsonobj.removeMember("engine");
	}
	//扩展字段
	Json::Value::Members  mem = jsonobj.getMemberNames();
	for (auto iter = mem.begin(); iter != mem.end(); iter++)
	{
		m_other.append((*iter));
		m_other.append("=");
		m_other.append(jsonobj[*iter].asString());
		m_other.append(";");
	}
}

SessionParam::~SessionParam()
{
	tsilk_fini(tsilk_enc);
	tsilk_fini(tsilk_dec);
	delete m_data;
	m_data = nullptr;
}
