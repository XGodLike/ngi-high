#pragma once
#include <iostream>

using namespace std;
typedef unsigned char BYTE;

class CCommon
{
public:
	CCommon(void);
	~CCommon(void);

	static string URLEncode(string sIn);

	static string EncryptPefect(string encrypt,string key);	

private:
	static BYTE toHex(const BYTE &x);
	
};
