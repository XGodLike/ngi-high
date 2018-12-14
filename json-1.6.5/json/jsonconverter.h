//####################################################################
//    Created:    2012/7/27   13:33
//    Filename:   jsonconverter.h
//    Author:     viki
//    Copyright:  Aoshuo Tech. 2012-2013
//********************************************************************
//    Comments:    
//
//    UpdateLogs:    
//####################################################################

#ifndef CPPTL_JSON_CONVERTER_H_INCLUDED
#define CPPTL_JSON_CONVERTER_H_INCLUDED

#include "forwards.h"
#include "value.h"
#include <string>
#include <vector>

#ifndef JSON_USE_CPPTL_SMALLMAP
#include <map>
#else
#include <cpptl/smallmap.h>
#endif
#ifdef JSON_USE_CPPTL
#include <cpptl/forwards.h>
#endif

class json_convert_exception : public std::exception
{
public:
    json_convert_exception(const char * msg):m_msg(msg) {}
    ~json_convert_exception() throw() {}

    virtual const char* what() const throw() {return m_msg.c_str();}

private:
    string m_msg;
};

template <typename InputT>
void json_convert(Json::Value v, InputT& o);

//nullValue = 0, ///< 'null' value
//intValue,      ///< signed integer value
//uintValue,     ///< unsigned integer value
//realValue,     ///< double value
//stringValue,   ///< UTF-8 string value
//booleanValue,  ///< bool value
//arrayValue,    ///< array value (ordered list)
//objectValue    ///< object value (collection of name/value pairs).

template <>
void json_convert(Json::Value v, VAF::Int32& o)
{
    //intValue  
    //uintValue  
    //booleanValue
    if (v.isInt())
    {
        o = v.asInt();
    }
    else
    {
        throw json_convert_exception("Not Int32!");
    }
}

template <>
void json_convert(Json::Value v, VAF::UInt32& o)
{
    //intValue  
    //uintValue  
    //booleanValue
    if (v.isUInt())
    {
        o = v.asUInt();
    }
    else
    {
        throw json_convert_exception("Not UInt32!");
    }
}

template <>
void json_convert(Json::Value v, VAF::Int64& o)
{
	//intValue  
	//uintValue  
	//booleanValue
	if (v.isInt64())
	{
		o = v.asLargestInt();
	}
	else
	{
		throw json_convert_exception("Not Int64!");
	}
}

template <>
void json_convert(Json::Value v, VAF::UInt64& o)
{
	//intValue  
	//uintValue  
	//booleanValue
	if (v.isUInt64())
	{
		o = v.asLargestUInt();
	}
	else
	{
		throw json_convert_exception("Not UInt64!");
	}
}

template <>
void json_convert(Json::Value v, long& o)
{
    if (v.isInt())
    {
        o = v.asInt();
    }
    else if (v.isDouble())
    {
        o = (long)(v.asDouble());
    }
    else
    {
        throw json_convert_exception("Not Integral!");
    }
}

template <>
void json_convert(Json::Value v, unsigned long& o)
{
    if (v.isUInt())
    {
        o = v.asUInt();
    }
    else if (v.isDouble())
    {
        o = (unsigned long)(v.asDouble());
    }
    else
    {
        throw json_convert_exception("Not Integral!");
    }
}

template <>
void json_convert(Json::Value v, double& o)
{
    if (v.isNumeric())
    {
        o = v.asDouble();
    }
    else
    {
        throw json_convert_exception("Not Numeric!");
    }
}

template <>
void json_convert(Json::Value v, float& o)
{
    if (v.isNumeric())
    {
        o = (float)(v.asDouble());
    }
    else
    {
        throw json_convert_exception("Not Numeric!");
    }
}

template <>
void json_convert(Json::Value v, bool& o)
{
    if (v.isBool())
    {
        o = v.asBool();
    }
    else
    {
        throw json_convert_exception("Not Bool!");
    }
}

template <>
void json_convert(Json::Value v, string& o)
{
    if (v.isString())
    {
        o = v.asString();
    }
    else
    {
        throw json_convert_exception("Not String!");
    }
}

template <typename VectorT>
void json_convert_vector(Json::Value v, vector<VectorT>& o)
{
    if (v.isArray())
    {
        for (Json::Value::iterator it = v.begin(); it != v.end(); ++it)
        {
            VectorT x;
            try
            {
                json_convert(*it, x);
                o.push_back(x);
            }
            catch (...)
            {
                throw json_convert_exception("Array item may be wrong!");
            }
        }
    }
    else
    {
        throw json_convert_exception("Not Array!");
    }
}

template <>
void json_convert(Json::Value v, vector<VAF::Int32>& o)
{
    json_convert_vector<VAF::Int32>(v, o);
}

template <>
void json_convert(Json::Value v, vector<VAF::UInt32>& o)
{
    json_convert_vector<VAF::UInt32>(v, o);
}

template <>
void json_convert(Json::Value v, vector<VAF::Int64>& o)
{
	json_convert_vector<VAF::Int64>(v, o);
}

template <>
void json_convert(Json::Value v, vector<VAF::UInt64>& o)
{
	json_convert_vector<VAF::UInt64>(v, o);
}

template <>
void json_convert(Json::Value v, vector<long>& o)
{
    json_convert_vector<long>(v, o);
}

template <>
void json_convert(Json::Value v, vector<unsigned long>& o)
{
    json_convert_vector<unsigned long>(v, o);
}

template <>
void json_convert(Json::Value v, vector<float>& o)
{
    json_convert_vector<float>(v, o);
}

template <>
void json_convert(Json::Value v, vector<double>& o)
{
    json_convert_vector<double>(v, o);
}

template <>
void json_convert(Json::Value v, vector<bool>& o)
{
    json_convert_vector<bool>(v, o);
}

template <>
void json_convert(Json::Value v, vector<string>& o)
{
    json_convert_vector<string>(v, o);
}


template <typename MapT>
void json_convert_map(Json::Value v, map<string, MapT>& o)
{
    if (v.isObject())
    {
        Json::Value::Members members = v.getMemberNames();
        for (Json::Value::Members::iterator iter = members.begin(); iter != members.end(); iter++)
        {
            MapT x;
            try
            {
                json_convert(v[(*iter)], x);
                o[(*iter)] = x;
            }
            catch (...)
            {
                throw json_convert_exception("Object item may be wrong!");
            }
        }
    }
    else
    {
        throw json_convert_exception("Not Object!");
    }
}

template <>
void json_convert(Json::Value v, map<string, VAF::Int32>& o)
{
    json_convert_map<VAF::Int32>(v, o);
}

template <>
void json_convert(Json::Value v, map<string, VAF::UInt32>& o)
{
    json_convert_map<VAF::UInt32>(v, o);
}

template <>
void json_convert(Json::Value v, map<string, VAF::Int64>& o)
{
	json_convert_map<VAF::Int64>(v, o);
}

template <>
void json_convert(Json::Value v, map<string, VAF::UInt64>& o)
{
	json_convert_map<VAF::UInt64>(v, o);
}

template <>
void json_convert(Json::Value v, map<string, long>& o)
{
    json_convert_map<long>(v, o);
}

template <>
void json_convert(Json::Value v, map<string, unsigned long>& o)
{
    json_convert_map<unsigned long>(v, o);
}

template <>
void json_convert(Json::Value v, map<string, float>& o)
{
    json_convert_map<float>(v, o);
}

template <>
void json_convert(Json::Value v, map<string, double>& o)
{
    json_convert_map<double>(v, o);
}

template <>
void json_convert(Json::Value v, map<string, bool>& o)
{
    json_convert_map<bool>(v, o);
}

template <>
void json_convert(Json::Value v, map<string, string>& o)
{
    json_convert_map<string>(v, o);
}

template <typename InputT>
void json_convert(Json::Value v, vector<InputT>& o)
{
    try
    {
        json_convert_vector<InputT>(v, o);
    }
    catch (...)
    {
        throw json_convert_exception("Cannot convert!");
    }
}

template <typename InputT>
void json_convert(Json::Value v, map<string, InputT>& o)
{
    try
    {
        json_convert_map<InputT>(v, o);
    }
    catch (...)
    {
        throw json_convert_exception("Cannot convert!");
    }
}

template <typename InputT>
void json_convert(Json::Value v, InputT& o)
{
    throw json_convert_exception("Cannot convert!");
}

#endif // CPPTL_JSON_CONVERTER_H_INCLUDED
