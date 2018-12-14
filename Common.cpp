#include "Common.h"
//#include "json/json.h"
//#ifdef WIN32
//#include "stdafx.h"
//#pragma warning(disable:4996)

//#endif

CCommon::CCommon(void)
{
}

CCommon::~CCommon(void)
{
}


BYTE CCommon::toHex(const BYTE &x)
    {
        return x > 9 ? x -10 + 'a': x + '0';
    }

string CCommon::URLEncode(string sIn)
    {
        string sOut;
        //string sIn(ssIn);
        for( size_t ix = 0; ix < sIn.size(); ix++ )
        {
            BYTE buf[4];
            memset( buf, 0, 4 );
            if( isalnum( (BYTE)sIn[ix] ) )
            {
                buf[0] = sIn[ix];
            }
            else if ( isspace( (BYTE)sIn[ix] ) ) //璨屼技鎶婄┖鏍肩紪鐮佹垚%20鎴栬€?閮藉彲浠?
            {
                buf[0] = '+';
            }
            else if(isalpha(sIn[ix]) || isdigit(sIn[ix]) || sIn[ix] == '-' || sIn[ix] == '.' || sIn[ix] == '~')
            {
            	buf[0] = (BYTE)sIn[ix];
            }
            else
            {
                buf[0] = '%';
                buf[1] = toHex( (BYTE)sIn[ix] >> 4 );
                buf[2] = toHex( (BYTE)sIn[ix] % 16);
            }
            sOut += (char *)buf;
        }
        return sOut;
    };

string CCommon::EncryptPefect(string encrypt,string key)
{
	if (encrypt.size() == 0) return "";
	if (key.size() == 0) return "";
	
	string sEncryptEncode = CCommon::URLEncode(encrypt);
	string sKeyEncode = CCommon::URLEncode(key);

	int nEncryptLength = sEncryptEncode.size();
	int nKeyLength = sKeyEncode.size();

	//锟斤拷锟斤拷锟姐法
	BYTE* pEncryptValue = new BYTE[nEncryptLength];
	for (int i = 0, j = 0; i < nEncryptLength; i++, j++)
	{
		*(pEncryptValue + i) = (sEncryptEncode[i] ^ sKeyEncode[j]);
		if (j == nKeyLength - 1) j = -1;
	}
	BYTE hight = 0, low = 0;
	for (int k = 0; k < nEncryptLength; k++)
	{
		hight = (BYTE)((pEncryptValue[k] & 0xf0) >> 4);
		low = (BYTE)((pEncryptValue[k] & 0x0f) << 4);
		pEncryptValue[k] = (BYTE)(hight | low);
	}
	string sDecrypt = "";
	for (int i = 0; i < nEncryptLength; i++)
	{
		char temp[4] = "0";
		char val[3] = {0};
		//锟斤拷一锟斤拷锟街斤拷转锟斤拷16锟斤拷锟狡碉拷锟街凤拷
		sprintf(val, "%x", *(pEncryptValue + i));
		if (strlen(val) < 2)
		{
			strcat(temp,val);
		}
		else
		{
			strcpy(temp,val);
		}
		int tempLen = strlen(temp);
		sDecrypt.append(temp+(tempLen-2));
	}
	delete []pEncryptValue;

	return sDecrypt;
}